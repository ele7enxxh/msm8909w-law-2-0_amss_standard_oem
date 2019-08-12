#ifndef MCFG_MAP_I_H
#define MCFG_MAP_I_H
/*==============================================================================

                M O D E M   C O N F I G   M A P
 
GENERAL DESCRIPTION
  Internal header for MCFG MAP

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_map_i.h#1 $ 
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
#include "mcfg_map.h"

/*==============================================================================

                PRIVATE DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
/* Paths to store config files */
#define MCFG_MAP_PATH_META         "/nvm/mcfg"
#define MCFG_MAP_SIGNATURE         0x4D634667  /* Signature "McFg" */
#define MCFG_MAP_SIGNATURE_REMOTE  0x4D636667  /* Signature "Mcfg" for remote */
#define MCFG_MAP_DEFAULT_FILE_HW   "mcfg_hw.mbn"
#define MCFG_MAP_DEFAULT_FILE_SW   "mcfg_sw.mbn"
#define MCFG_MAP_DEFAULT_ID_HW     MCFG_MAP_DEFAULT_FILE_HW
#define MCFG_MAP_DEFAULT_ID_SW     MCFG_MAP_DEFAULT_FILE_SW
#define MCFG_MAP_FILE_HW_PREFIX    "mcfg_hw"
#define MCFG_MAP_FILE_SW_PREFIX    "mcfg_sw"

/* Maximum filename length for config files */
#define MCFG_MAP_RECORD_FILENAME_MAX MCFG_UTILS_RECORD_FILENAME_MAX
#define MCFG_MAP_RECORD_FILEPATH_MAX (MCFG_MAP_RECORD_FILENAME_MAX + 20)

/*==============================================================================
  Typedefs
==============================================================================*/

typedef list_type mcfg_map_record_map_node_list_type;

typedef struct
{
  list_link_type         link;
  mcfg_map_record_s_type record;
}mcfg_map_record_map_node_type;

typedef struct
{
  mcfg_config_id_s_type               active_id[MCFG_MAX_RESERVED_SUB_ID];
  mcfg_config_id_s_type               pending_id[MCFG_MAX_RESERVED_SUB_ID];
  mcfg_map_record_map_node_list_type  map;
}mcfg_map_list_s_type;

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

  FUNCTION mcfg_utils_is_config_id_unique

  DESCRIPTION
    This function returns whether the specified unique ID exists on the device.

  DEPENDENCIES
    None

  PARAMETERS
    type    [in]  type of ID to query
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
);

/*===========================================================================

  FUNCTION mcfg_map_list_configs

  DESCRIPTION
    This function returns a list of unique ID's of the specified type that are
    available.

  DEPENDENCIES
    None

  PARAMETERS
    type          [in]  type of ID to query (max returns all types)
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
);

/*===========================================================================

  FUNCTION mcfg_map_add_config

  DESCRIPTION
    This function is used to store a new config file on the device.  

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
boolean mcfg_map_add_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
);

/*===========================================================================

  FUNCTION mcfg_map_delete_config

  DESCRIPTION
    This function deletes the config file with the specified unique ID from the
    device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of the config to delete
    id        [in]  unique ID struct to fill with selected config (if NULL then
                    delete all config files of the given type)

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_delete_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
);

/*===========================================================================

  FUNCTION mcfg_map_get_config

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
boolean mcfg_map_get_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
);


/*===========================================================================

  FUNCTION mcfg_map_get_active_config

  DESCRIPTION
    This function returns unique ID of the currently activated config file.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [out] unique ID struct to fill with active config

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
);

/*===========================================================================

  FUNCTION mcfg_map_set_active_config

  DESCRIPTION
    This function sets the currently active config file to the specified
    unique ID.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  unique ID struct to fill with active config

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
);

boolean mcfg_map_rmv_active_config
(
  mcfg_config_type_e_type type,
  mcfg_sub_id_type_e_type sub
);

/*===========================================================================

  FUNCTION mcfg_map_get_selected_config

  DESCRIPTION
    This function returns unique ID of the currently selected config file.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [out] unique ID struct to fill with selected config

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
);

/*===========================================================================

  FUNCTION mcfg_map_set_selected_config

  DESCRIPTION
    This function sets the currently selected config file to the specified
    unique ID.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  unique ID struct to fill with selected config

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
);

/*===========================================================================

  FUNCTION mcfg_map_get_config_path

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
boolean mcfg_map_get_config_path
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  char                   *path
);

/*===========================================================================

  FUNCTION mcfg_map_get_config_filename

  DESCRIPTION
    This function translates the config ID into the efs file name which
    stores the config file

  DEPENDENCIES
    None

  PARAMETERS
    id        [in]  unique ID struct
    path      [out] file path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_get_config_filename
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  char                   *filename,
  size_t                  size
);

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
);

/*===========================================================================

  FUNCTION mcfg_map_empty_map

  DESCRIPTION
    This function cleans up the map list

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  config ID to del
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_empty_map
(
  mcfg_config_type_e_type type
);

/*===========================================================================

  FUNCTION mcfg_map_read_config_map

  DESCRIPTION
    This function reads the ID and file name mapping for a specified config
    type into a link list

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_read_config_map
(
  mcfg_config_type_e_type   type
);


/*===========================================================================

  FUNCTION mcfg_map_store_config_map

  DESCRIPTION
    This function stores config record map list into EFS
    for a specified config

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_store_config_map
(
  mcfg_config_type_e_type type
);

/*===========================================================================

  FUNCTION mcfg_map_store_config_in_path

  DESCRIPTION
    This function stores config record into EFS with specified path

  DEPENDENCIES
    None

  PARAMETERS
    config    [in]  config to be stored
    path      [in]  config path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_store_config_in_path
(
  mcfg_config_s_type *config,
  char               *path
);

/*===========================================================================

  FUNCTION mcfg_map_delete_config_in_path

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
boolean mcfg_map_delete_config_in_path
(
  char  *path
);


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
);


/*===========================================================================

  FUNCTION mcfg_map_del_record_in_map

  DESCRIPTION
    This function stores config record map list into EFS
    for a specified config

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  config ID to del
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
);

boolean mcfg_map_init_record_by_id
(
  mcfg_map_record_s_type *record,
  mcfg_config_id_s_type *id
);

boolean mcfg_map_add_record
(
  mcfg_config_type_e_type type,
  mcfg_map_record_s_type *record
);

boolean mcfg_map_read_record
(
  mcfg_config_type_e_type type,
  mcfg_map_record_s_type *record
);

boolean mcfg_map_delete_record
(
  mcfg_config_type_e_type type,
  mcfg_map_record_s_type *record
);

void mcfg_map_print_map
(
  void
);

#endif /* MCFG_MAP_I_H */

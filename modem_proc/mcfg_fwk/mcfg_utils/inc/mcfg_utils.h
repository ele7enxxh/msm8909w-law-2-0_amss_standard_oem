#ifndef MCFG_UTILS_H
#define MCFG_UTILS_H
/*==============================================================================

                M O D E M   C O N F I G   U T I L S
 
GENERAL DESCRIPTION
  Header MCFG Utility library

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/inc/mcfg_utils.h#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
09/27/13 hzhi Added declaration of mcfg_utils_reset_modem(void) for segment loading. 
05/08/12 cy   Initial creation

==============================================================================*/

#include "mcfg_common.h"
#include "mcfg_task.h"
#include "sys_m_reset.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
#define MCFG_UTILS_RECORD_FILENAME_MAX (65)
#define MCFG_UTILS_RECORD_FILEPATH_MAX (MCFG_UTILS_RECORD_FILENAME_MAX + 20)

/*==============================================================================
  Typedefs
==============================================================================*/
/* Struct for config file record */
typedef struct 
{
  /* Unique ID for this config */
  mcfg_config_id_s_type id;
  
  mcfg_storage_mode_e_type storage;
  int size;
} mcfg_map_record_s_type;

typedef enum mcfg_utils_cmd_e {
  MCFG_UTILS_CMD_MIN = 0,

  MCFG_UTILS_CMD_RESET_NOW = 0,
  MCFG_UTILS_CMD_RESET_DEBOUNCE,
  MCFG_UTILS_CMD_RESET_RETRY,
  MCFG_UTILS_CMD_CLEAR_RESET_TIMER,
  MCFG_UTILS_CMD_ACTIVATE_NOW,
  MCFG_UTILS_CMD_ACTIVATE_DEBOUNCE,
  MCFG_SEGLOAD_RESET,
  MCFG_UTILS_CMD_MAX
} mcfg_utils_cmd_e_type;

typedef enum mcfg_activation_source_e {
  MCFG_ACTIVATION_SOURCE_MIN = 0,
  MCFG_ACTIVATION_ICCID_SELECT = 0,
  MCFG_ACTIVATION_CROSSMAPPING,
  MCFG_ACTIVATION_BY_PDC,
  MCFG_ACTIVATION_DEBOUNCE,
  MCFG_ACTIVATION_IMSI_SELECT,
  MCFG_ACTIVATION_SUBSI_CARRIER,
  MCFG_ACTIVATION_SOURCE_MAX
} mcfg_activation_source_e_type;

#if !defined(FEATURE_MCFG_SSREQ_W_REASON)
typedef enum _ssreq_qmi_request_res_code_type{
   SSREQ_QMI_RES_NONE=0,
   SSREQ_QMI_RES_MODEM_CONF_CHANGE = 0x10,/*SSREQ_QMI_RES_MODEM_CONF_CHANGE_V01*/
   SSREQ_QMI_RES_SEGMENT_LOADING/*SSREQ_QMI_RES_SEGMENT_LOADING_V01*/
}ssreq_qmi_request_res_code_type;
#endif

/*==============================================================================
  Variables
==============================================================================*/


/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/


/*===========================================================================

  FUNCTION mcfg_utils_alloc_config_buffer

  DESCRIPTION
    This function is used to return a pointer to memory allocated to store 
    a new config file onto the device.

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]       type of the new config to store
    config [in/out]   config structure including size to be allocated and the 
                      config buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_alloc_config_buffer
(
  mcfg_config_type_e_type type,
  mcfg_config_s_type     *config
);


/*===========================================================================

  FUNCTION mcfg_utils_free_config_buffer

  DESCRIPTION
    This function is used to free memory that was allocated to store
    a new config file.

  DEPENDENCIES
    None

  PARAMETERS
    config [in/out] config structure including size allocated and
                    the config buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_free_config_buffer
(
  mcfg_config_s_type *config
);


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
boolean mcfg_utils_is_config_id_unique
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
);

/*===========================================================================

  FUNCTION mcfg_utils_config_id_matches

  DESCRIPTION
    This function returns whether the two specified IDs match or not.

  DEPENDENCIES
    None

  PARAMETERS
    id1     [in]  config ID struct
    id2     [in]  config ID struct

  RETURN VALUE
    TRUE if two IDs match, FALSE if not

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_config_id_matches
(
  mcfg_config_id_s_type  *id1,
  mcfg_config_id_s_type  *id2
);

/*===========================================================================

  FUNCTION mcfg_utils_list_configs

  DESCRIPTION
    This function returns a list of unique ID's of the specified type that are
    available.

  DEPENDENCIES
    None

  PARAMETERS
    type          [in]  type of ID to query (max returns all types)
    max_list_size [in]  size of the id_list array (number of elements)
    id_list[]    [out] list of unique ID struct to fill with selected config

  RETURN VALUE
    number of configs

  SIDE EFFECTS
    None

===========================================================================*/
uint32 mcfg_utils_list_configs
(
  mcfg_config_type_e_type type,
  int                     max_list_size,
  mcfg_config_id_s_type  *id_list
);

/*===========================================================================

  FUNCTION mcfg_utils_print_config

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_print_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
);

/*===========================================================================

  FUNCTION mcfg_utils_add_config

  DESCRIPTION
    This function is used to store a new config file on the device.  

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the new config
    id     [in]  ID of the new config
    config [in/out] config structure including config size and buffer pointer

  RETURN VALUE
	MODEM_CFG_PROC_SUCCESS when the configuration is stored successfully
	MODEM_CFG_PROC_LOAD_FAIL when there is an internal error saving the config
	MODEM_CFG_PROC_AUTH_FAIL when the configuration fails authentication

  SIDE EFFECTS
    Config file is stored into device

===========================================================================*/
mcfg_error_e_type mcfg_utils_add_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config,
  mcfg_storage_mode_e_type storage
);


/*===========================================================================

  FUNCTION mcfg_utils_delete_config

  DESCRIPTION
    This function deletes the config file with the specified unique ID from the
    device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of the config to delete
    id        [in]  unique ID struct to fill with selected config

  RETURN VALUE
    mcfg_error_e_type

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_utils_delete_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
);

/*===========================================================================

  FUNCTION mcfg_utils_delete_all

  DESCRIPTION
    This function deletes the config file with the specified type from the
    device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of the config to delete

  RETURN VALUE
    mcfg_error_e_type

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_utils_delete_all
(
  mcfg_config_type_e_type type
);


/*===========================================================================

  FUNCTION mcfg_utils_get_config

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
boolean mcfg_utils_get_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
);


/*===========================================================================

  FUNCTION mcfg_utils_get_config_info

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
boolean mcfg_utils_get_config_info
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  mcfg_config_info_s_type *info
);


/*===========================================================================

  FUNCTION mcfg_utils_get_config_limits

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
boolean mcfg_utils_get_config_limits
(
  mcfg_config_type_e_type type,
  uint64                 *max_size,
  uint64               *curr_size
);

/*===========================================================================

  FUNCTION mcfg_utils_get_config_size

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
boolean mcfg_utils_get_config_size
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  int                    *size
);



/*===========================================================================

  FUNCTION mcfg_utils_get_active_config

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
boolean mcfg_utils_get_active_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
);

/*===========================================================================

  FUNCTION mcfg_utils_set_active_config

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
boolean mcfg_utils_set_active_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
);


/*===========================================================================

  FUNCTION mcfg_utils_get_selected_config

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
boolean mcfg_utils_get_selected_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
);


/*===========================================================================

  FUNCTION mcfg_utils_set_selected_config

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
boolean mcfg_utils_set_selected_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
);

/*===========================================================================

  FUNCTION mcfg_utils_get_default_config

  DESCRIPTION
    This function is used to get the size and address of default config
    ata segment from DAL device config. 

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the default config
    config [in/out] config structure including config size and buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_get_default_config
(
  mcfg_config_type_e_type type,
  mcfg_config_s_type     *config
);

/*===========================================================================

  FUNCTION mcfg_utils_get_default_config_info

  DESCRIPTION
    This function is used to read extended information for default config
    from DAL device config.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    info      [out] info for the specified config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_get_default_config_info
(
  mcfg_config_type_e_type  type,
  mcfg_config_info_s_type *info
);

/*===========================================================================

  FUNCTION mcfg_utils_activate_config

  DESCRIPTION
    This function performs the necessary action for the currently selected
    config to become the active config.

  DEPENDENCIES
    None

  PARAMETERS
    type   [in] type of the config to activate

  RETURN VALUE
    MCFG_ERR_NONE if success, error code otherwise

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_utils_activate_config
(
  mcfg_config_type_e_type        type,
  mcfg_activation_source_e_type  source
);

/*===========================================================================

  FUNCTION mcfg_utils_activate_refreshonly

  DESCRIPTION
    This function performs refresh only activation.

  DEPENDENCIES
    None

  PARAMETERS
    slot   [in] slot to refresh
    sub    [in] sub to refresh

  RETURN VALUE
    mcfg_error_enum

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_utils_activate_refreshonly
(
  uint8                   slot,
  mcfg_sub_id_type_e_type sub
);

/*===========================================================================

  FUNCTION mcfg_utils_deactivate_config

  DESCRIPTION
    This function deactivates the currently active config 

  DEPENDENCIES
    None

  PARAMETERS
    type   [in] type of the config to deactivate

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_deactivate_config
(
  mcfg_config_type_e_type type,
  mcfg_sub_id_type_e_type sub,
  boolean                 activating
);

/*===========================================================================

  FUNCTION mcfg_utils_cmd_proc

  DESCRIPTION
    Processes a mcfg_utils command in the context of the MCFG task.

  DEPENDENCIES
    None

  PARAMETERS
    cmd      [in]  command struct

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_cmd_proc
(
  mcfg_task_cmd_s_type *cmd
);

/*===========================================================================

  FUNCTION mcfg_utils_init

  DESCRIPTION
    This function initializes the utils library

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_init
(
  void
);

/*===========================================================================

  FUNCTION mcfg_utils_reset_modem

  DESCRIPTION
    This function uses ATCOP Reset fowarding as workaround for
    device reset. It also tries sys_m poweroff since it's currently
    not supported for all targets. ATCOP workaround should be removed
    once sys_m is fully implemented.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_reset_modem
(
  mcfg_power_down_reason_e_type res_code
);

/*===========================================================================

  FUNCTION mcfg_utils_cmd_send

  DESCRIPTION
    This function sends mcfg utils commands for processing. 

  DEPENDENCIES
    None

  PARAMETERS
    utils_cmd    [in]  utils commands

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
 void mcfg_utils_cmd_send(mcfg_utils_cmd_e_type utils_cmd);
 
/*===========================================================================

  FUNCTION mcfg_utils_cmd_send

  DESCRIPTION
    This function sends mcfg utils commands for processing. 

  DEPENDENCIES
    None

  PARAMETERS
    utils_cmd    [in]  utils commands

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_cmd_send
(
   mcfg_utils_cmd_e_type utils_cmd
);

/*===========================================================================
                                  
FUNCTION  mcfg_compat_check() 

DESCRIPTION
  Compatibility check for the downloaded MCFG

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 mcfg_compat_check
(
  uint8 cfg_type, 
  uint32* ram_mcfg_version_ptr
);



/*===========================================================================

  FUNCTION mcfg_utils_parse_config_info

  DESCRIPTION
    This function is used to read extended information for config 

  DEPENDENCIES
    None

  PARAMETERS
    config    [in]  config pointer
    info      [out] info for the specified config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_parse_config_info
(
  mcfg_config_s_type      *config,
  mcfg_config_info_s_type *info
);

mcfg_error_e_type mcfg_utils_selected_config_available
(
  mcfg_config_type_e_type type,
  mcfg_sub_id_type_e_type sub
);


boolean mcfg_setting_init(void);

void mcfg_utils_saved_power_down_reason(void);

ssreq_qmi_request_res_code_type mcfg_to_ssreq_qmi_map_res_code
(
   mcfg_power_down_reason_e_type res_code
);

boolean mcfg_utils_get_phrase
(
  char   *line,
  uint32 *offset,
  uint32  maxsize,
  char   *buf
);
#endif /* MCFG_UTILS_H */
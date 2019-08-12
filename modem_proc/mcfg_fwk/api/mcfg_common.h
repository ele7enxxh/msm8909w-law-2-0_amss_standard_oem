#ifndef MCFG_COMMON_H
#define MCFG_COMMON_H
/*==============================================================================

                M O D E M   C O N F I G   C O M M O N
 
GENERAL DESCRIPTION
  MCFG common header file

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/api/mcfg_common.h#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/14/12 cy   Initial creation

==============================================================================*/
#include "mcfg_feature_config.h"

#include "mcfg_refresh.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
/* Maximum size of each config file */
#define MCFG_CONFIG_SIZE_MAX              (73728)

/* Maximum size of each frame used in load_config_req */
#define MCFG_CONFIG_FRAME_SIZE_MAX        (8192)

/* Maximum size of the unique ID for each config file */
#define MCFG_CONFIG_ID_SIZE_MAX           (124)

/* Maximum size of the description for each config file */
#define MCFG_CONFIG_DESCRIPTION_SIZE_MAX  (512)

/* Maximum size of each config file lists */
#define MCFG_CONFIG_LIST_SIZE_MAX         (25)

/* Maximum length of the path for a remote config file */
#define MCFG_CONFIG_PATH_SIZE_MAX         (255)  

#define MODEM_CFG_UINT_MAX                   0xFFFFFFFF

/* Check if interger overflows with addition. start, offset must be
   unsigned integer types of upto 32 bit width for this check to work. */
#define MCFG_INTERGER_OVERFLOWED(start, offset) \
          ((uint32)(offset) > (MODEM_CFG_UINT_MAX - (uint32)(start)))

/* Check size plus offset are witnin the limit */
#define MCFG_SIZE_OFFSET_WITHIN_LIMIT(size, offset, limit) \
          (!MCFG_INTERGER_OVERFLOWED((size), (offset)) && \
             ((size)+(offset) <= (limit)))

#define IS_VALID_CONFIG_TYPE(type) \
          ((type) == MCFG_TYPE_SW || (type) == MCFG_TYPE_HW)

/*==============================================================================
  Typedefs
==============================================================================*/
/* Enum for types of config files stored */
typedef enum mcfg_config_type_e{
  MCFG_TYPE_FIRST = 0,
  MCFG_TYPE_HW = 0,
  MCFG_TYPE_SW,
  MCFG_NUM_OF_TYPES
} mcfg_config_type_e_type;

typedef enum mcfg_storage_mode_e{
  MCFG_STORAGE_FIRST = 0,
  MCFG_STORAGE_LOCAL = 0,
  MCFG_STORAGE_REMOTE,
  MCFG_NUM_OF_STORAGE_MODE
} mcfg_storage_mode_e_type;

typedef enum mcfg_sub_id_type_e{
  MCFG_SUB_ID_FIRST = MCFG_REFRESH_INDEX_0,

  #ifdef FEATURE_MCFG_MULTISIM_SUPPORT
  MCFG_SUB_ID_SECOND = MCFG_REFRESH_INDEX_1,
  MCFG_SUB_ID_THIRD = MCFG_REFRESH_INDEX_2,
  #endif

  MCFG_NUM_OF_SUB_ID,
  MCFG_MAX_RESERVED_SUB_ID = 5,
  MCFG_SUB_ID_INVALID = MCFG_REFRESH_INDEX_INVALID
} mcfg_sub_id_type_e_type;

typedef enum mcfg_slot_index_type_e{
  MCFG_SLOT_INDEX_FIRST = MCFG_REFRESH_INDEX_0,

  #ifdef FEATURE_MCFG_MULTISIM_SUPPORT
  MCFG_SLOT_INDEX_SECOND = MCFG_REFRESH_INDEX_1,
  MCFG_SLOT_INDEX_THIRD = MCFG_REFRESH_INDEX_2,
  #endif

  MCFG_NUM_OF_SLOT_INDEX,
  MCFG_NUM_OF_UIM_SLOT = MCFG_NUM_OF_SLOT_INDEX,
} mcfg_slot_index_type_e_type;

/* MCFG Error Codes */
typedef enum mcfg_error_e{
  MCFG_ERR_NONE = 0x0000, // QMI_ERR_NONE_V01
  MODEM_CFG_PROC_SUCCESS = 0x0000,
  MCFG_ERR_REFRESH_FAILED = 0x0004, //QMI_ERR_ABORTED_V01
  MCFG_ERR_DEVICE_IN_USE = 0x0017,  //QMI_ERR_DEVICE_IN_USE_V01
  MCFG_ERR_NO_EFFECT = 0x001A, // QMI_ERR_NO_EFFECT_V01
  MCFG_ERR_AUTHENTICATION_FAILED = 0x0022, //QMI_ERR_AUTHENTICATION_FAILED_V01
  MCFG_ERR_NOT_FOUND = 0x0032, //QMI_ERR_DEVICE_STORAGE_FULL_V01
  MCFG_ERR_OUT_OF_SPACE = 0x0044, //QMI_ERR_INSUFFICIENT_RESOURCES_V01
  MCFG_ERR_NOT_SUPPORTED = 0x004D, //QMI_ERR_BUNDLING_NOT_SUPPORTED_V01
  MCFG_ERR_INCOMPATIBLE = 0x005A, //QMI_ERR_INCOMPATIBLE_STATE_V01
  MCFG_ERR_NON_QMI = 0x1000,
  MCFG_ERR_INTERNAL,
  MCFG_ERR_LOAD_FAIL,
  MCFG_ERR_INVALID_PARAMETER,
  MCFG_ERR_NULL_POINTER,
  MCFG_ERR_INVALID_SEGMENT,
  MCFG_ERR_INVALID_HASH,
  MCFG_ERR_INVALID_SIGNATURE,
  MCFG_ERR_DUPLICATE_ID,
  MCFG_ERR_CURRENT_ACTIVE,
  MCFG_ERR_ACCESS_DENIED,
  MCFG_ERR_INVALID_STATE,
  MCFG_ERR_INVALID_REFERENCE,
  MODEM_CFG_PROC_LOAD_FAIL,
  MODEM_CFG_PROC_AUTH_FAIL,
  MODEM_CFG_HEADER_INVALID,
  MODEM_CFG_UPDATE_NVEFS_FAIL,
  MODEM_CFG_VERSION_UPDATE_FAIL,
  MODEM_CFG_MUXD_VERSION_UPDATE_FAIL,
  MODEM_CFG_UPDATE_DEVCONFIG_FAIL,
  MODEM_CFG_UPDATE_NOT_REQUIRED,
  MODEM_CFG_LOAD_STATUS_INVALID = 0xFFFF,
} mcfg_error_e_type;

typedef enum mcfg_power_down_reason_e{
  MCFG_PWRDOWN_RES_NONE=0,
  MCFG_PWRDOWN_RES_MODEM_CONF_CHANGE = 0x10,/*SSREQ_QMI_RES_MODEM_CONF_CHANGE_V01*/
  MCFG_PWRDOWN_RES_SEGMENT_LOADING/*SSREQ_QMI_RES_SEGMENT_LOADING_V01*/
} mcfg_power_down_reason_e_type;

/* Struct for config ID */
typedef struct 
{
  /* Length of config ID */
  uint32 id_len;
  
  /* Buffer containing config ID (binary) */
  uint8 id[MCFG_CONFIG_ID_SIZE_MAX];
} mcfg_config_id_s_type;

/* Struct for config content */
typedef struct 
{
  /* Total size of the config file */
  int config_len;
  
  /* Pointer to the actual config buffer */
  void *config_addr;
} mcfg_config_s_type;

/* Struct for extended config file info */
typedef struct 
{
  /* Total size of the config file */
  int config_len;
  
  /* Length of description */
  int description_len;
  
  /* Buffer containing description (binary) */
  uint8 description[MCFG_CONFIG_DESCRIPTION_SIZE_MAX];

  /* Storage type of the config file*/
  mcfg_storage_mode_e_type  storage;

  /* Buffer containing path of the remote config file */
  uint16 path[MCFG_CONFIG_PATH_SIZE_MAX];
} mcfg_config_info_s_type;

/* Function prototypes*/
/*===========================================================================

FUNCTION  mcfg_pwr_down_reason() 

DESCRIPTION
  This method is used to provide the power down reason. 

DEPENDENCIES
  None. 

RETURN VALUE
  mcfg_power_down_reason_e_type - returns the power down reason.

SIDE EFFECTS
  None

===========================================================================*/
mcfg_power_down_reason_e_type mcfg_pwr_down_reason(void);

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
uint8 mcfg_get_muxd_index(void);
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */

#endif /* MCFG_COMMON_H */


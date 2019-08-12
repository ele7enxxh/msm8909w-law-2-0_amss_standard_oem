#ifndef _PS_DPM_UTILS
#define _PS_DPM_UTILS
/*===========================================================================

                     PS_DPM_UTILS . H
DESCRIPTION
Header file describing global DPM config definitions.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/inc/ps_dpm_utils.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
                            
when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/10/13    bvd     Initial Version
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dserrno.h"
#include "sys.h"
#include "modem_mem.h"
#include "data_msg.h"
#include "dsutil.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/**
  @brief  Enum type for DPM CONFIG ITEMs 
*/
typedef enum 
{
  PS_DPM_CFG_DISABLE_HW_BRIDGING,
  PS_DPM_CFG_ILLEGAL
} ps_dpm_cfg_item_e;


#define PS_DPM_EFS_CONFIGURATION_FILE_PATH  \
               "/nv/item_files/conf/data_config_info.conf"

/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/

/**
  @brief  Sets configuration item in non-volatile store (EFS).

  @param[in] item    configuration item to get
  @param[in] val   location of bytes to write to store
  @param[in] len   size of the configuration item in bytes
  
  @return  TRUE   if item was set successfully
  @return  FALSE  if item was not set successfully
  
  @code
  @endcode  
*/
boolean ps_dpm_cfg_set
(
    ps_dpm_cfg_item_e  item, 
    unsigned char     * val, 
    size_t              len
);

/**
  @brief  Gets configuration item from non-volatile store (EFS).

  @param[in] item    configuration item to get
  @param[in] val   location where value read should be written
  @param[in] len   size of the configuration item in bytes
  
  @return  Number of bytes read successfully from the file
  
  @code
  @endcode  
*/
int ps_dpm_cfg_get
(
    ps_dpm_cfg_item_e  item, 
    unsigned char     * val, 
    size_t             len
);

/**
  @brief  Puts all the required nv item file paths in the data_config_info.conf 
          file. Would be called during powerup.

  @param[in] file_paths_buf      data_config_info file path.
  @param[in] file_path_buf_len   length of above string
  
  @return  0   On success
  @return -1   On failure
  
  @code
  @endcode  
*/
int32 ps_dpm_update_data_config_info
(
  char  *file_paths_buf,
  uint32 file_paths_buf_len
);

#define PS_DPM_DATA_CONF_BUF_SIZE ps_dpm_get_data_conf_buf_size()


/**
  @brief  This function returns the buffer size to allocate for storing 
          DPM EFS Item File paths.

  @return  Buffer Size (uint32)
  
  @code
  @endcode  
*/
uint32 ps_dpm_get_data_conf_buf_size 
(
  void
);

/**
  @brief  Write all required item files to the config file.
          Any new item file must be added directly within this init 
          function.
          This function optimizes multiple calls for item file write from
          different modules into a single efs write call.
  
  @return  0   On success
  @return -1   On failure
  
  @code
  @endcode  
*/
int32 ps_dpm_init_efs_config_file
(
  void
);

#endif /* _PS_DPM_UTILS */
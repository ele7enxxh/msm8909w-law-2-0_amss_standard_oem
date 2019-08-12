#ifndef DS_APPSRV_UTILS_H
#define DS_APPSRV_UTILS_H
/*===========================================================================

                           DS_APPSRV_UTILS.H

DESCRIPTION
   
   Collection of utility functions being used by various modules in APPSRV.
                
EXTERNALIZED FUNCTIONS

 Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================
                       EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/appsrv/utils/inc/ds_appsrv_utils.h#1 $
  $Author: mplcsds1 $  $DateTime: 2016/12/13 08:00:03 $
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/17/15   hr      Initial version.
===========================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "datamodem_variation.h"

/*===========================================================================
                    EXTERNALIZED FUNCTIONS DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS_APPSRV_EFS_ITEM_FILE_PATH_SIZE

DESCRIPTION   Returns the total APPSRV EFS item file paths size
 
PARAMETERS    None
 
DEPENDENCIES  None

RETURN VALUE  Total APPSRV EFS item file paths size

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_appsrv_efs_item_file_path_size
(
  void
);

/*===========================================================================
FUNCTION      DS_APPSRV_UPDATE_DATA_CONFIG_INFO

DESCRIPTION   Puts all the required nv item file paths in the 
              data_config_info.conf file. Would be called during powerup.
 
DEPENDENCIES  None

RETURN VALUE  length of string added to buffer for success. 
              -1 for failure.

SIDE EFFECTS  None
===========================================================================*/
int32 ds_appsrv_update_data_config_info
(
  char   *file_paths_buf
);

/*===========================================================================
FUNCTION      DS_APPSRV_UTILS_INIT

DESCRIPTION   This function initializes all APPSRV common services.
              The function is called during power up.
 
PARAMETERS    None 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_appsrv_utils_init
(
  void
);

#endif /* DS_APPSRV_UTILS_H */

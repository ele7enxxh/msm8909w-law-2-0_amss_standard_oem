#ifndef SIMLOCK_CONFIG_H
#define SIMLOCK_CONFIG_H
/*===========================================================================


            S I M   L O C K   C O N F I G   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_config.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/14   vv      Relocate the simlock data files
09/29/14   vv      Added support for the remote SFS
09/29/14   vv      Added support for emergency only mode
09/17/14   tl      Add support for perso and deperso secure
06/20/14   vv      Added support for diag protection
04/14/14   vv      Added support for identifying conflicting codes
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_SIMLOCK

#include "simlock.h"

/*=============================================================================

                   DATA DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_STATUS_ENUM_TYPE

   DESCRIPTION:
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_DISABLED,
  SIMLOCK_ENABLED
} simlock_status_enum_type;

typedef enum {
  SIMLOCK_CK_NOT_PRESENT,
  SIMLOCK_CK_PRESENT
} simlock_ck_present_type;

typedef struct {
  simlock_status_enum_type    status;
  simlock_ck_present_type     ck_present;
} simlock_category_status_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_CONFIG_DATA_TYPE

   DESCRIPTION:
     This structure contains the config data

     sfs_type          : Local or remote SFS
     device_mode       : device operation mode (normal or emergency only)
     slot_policy       : slot policy
     sub_policy        : subscription pollicy
     category_status   : information regarding each category on a slot
-------------------------------------------------------------------------------*/
typedef struct {
  simlock_sfs_enum_type                     sfs_type;
  simlock_device_mode_enum_type             device_mode;
  simlock_slot_policy_enum_type             slot_policy;
  simlock_subscription_policy_enum_type     sub_policy;
  simlock_category_status_type              category_status[SIMLOCK_SLOT_COUNT_MAX][SIMLOCK_CATEGORY_COUNT_MAX];
} simlock_config_data_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_CONFIG_CREATE_CONFIG_FILE

DESCRIPTION
  This function creates a default config file during the power-up. All the
  categories are disabled by default

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_config_create_config_file
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_CONFIG_CREATE_DIR

DESCRIPTION
  This function creates the simlock directory in the EFS or SFS

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_config_create_dir
(
  simlock_dir_enum_type         dir_type
);

/*===========================================================================
FUNCTION SIMLOCK_CONFIG_CHECK_CONFIG_FILE

DESCRIPTION
  This function checks if the config file is present and is of the expected
  size

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_config_check_config_file
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_CONFIG_READ_CONFIG_DATA

DESCRIPTION
  This function reads the configuration data present in a file

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_config_read_config_data
(
  simlock_config_data_type  * config_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CONFIG_UPDATE_AND_WRITE_POLICIES

DESCRIPTION
  This function writes the slot and subscription policies in the config file

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_config_update_and_write_policies
(
  const simlock_set_config_policy_msg_req_type  * config_policy_ptr,
  simlock_config_data_type                      * config_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CONFIG_UPDATE_AND_WRITE_CATEGORY_STATUS

DESCRIPTION
  This function updates and writes the status of a category in the config
  file

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_config_update_and_write_category_status
(
  uint8                       category_index,
  uint8                       slot_index,
  simlock_status_enum_type    status,
  simlock_ck_present_type     ck_present,
  simlock_config_data_type  * config_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CONFIG_CHECK_DIR

DESCRIPTION
  This function checks if the directory type requested exists in the EFS

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_config_check_dir
(
  simlock_dir_enum_type         dir_type
);

/*===========================================================================
FUNCTION SIMLOCK_CONFIG_UPDATE_AND_WRITE_DEVICE_MODE

DESCRIPTION
  This function updates and writes the device mode to the config
  file

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_config_update_and_write_device_mode
(
  simlock_device_mode_enum_type    device_mode,
  simlock_config_data_type       * config_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CONFIG_UPDATE_AND_WRITE_SFS_TYPE

DESCRIPTION
  This function updates and writes the SFS type in the config file

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_config_update_and_write_sfs_type
(
  simlock_sfs_enum_type       update_sfs_type,
  simlock_config_data_type  * config_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CONFIG_READ_SFS_TYPE

DESCRIPTION
  This function that reads the config file and retrieves the
  type of SFS (LOCAL/REMOTE) being used for storing the simlock data.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_sfs_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_sfs_enum_type simlock_config_read_sfs_type
(
  void
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_CONFIG_H */


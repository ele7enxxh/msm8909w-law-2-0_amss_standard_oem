/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M   L O C K   C O N F I G   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock configuration functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_config.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/16   bcho    Added support for ICCID simlock
06/17/16   vdc     Added support for SPN based SIMLOCK feature
03/19/15   at      Introducing new relock feature in SIMLock
12/17/14   vv      Relocate the simlock data files
11/14/14   hh      Fix compilation warnings
09/29/14   vv      Added support for the remote SFS
09/29/14   vv      Added support for emergency only mode
09/17/14   tl      Add support for perso and deperso secure
06/20/14   vv      Added support for diag protection
04/15/14   vv      Added support for quad SIM
04/14/14   vv      Added support for identifying conflicting codes
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock.h"
#include "simlock_config.h"
#include "simlock_file.h"
#include "simlock_util.h"
#include "simlock_efs.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#else
static char *safe_efs_dir           =    "safe";
static char *sfs_efs_dir            =    "safe/sfs";
static char *uim_efs_dir            =    "safe/sfs/uim";
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

static char *simlock_sfs_dir           =    "/safe/sfs/uim/simlock";
static char *simlock_sfs_config_file   =    "/safe/sfs/uim/simlock/config";

/* Version 0 - Size: 46 bytes */
#define SIMLOCK_CONFIG_FILE_SIZE_V0      (2 + ((SIMLOCK_CATEGORY_COUNT_MAX - 3) * SIMLOCK_SLOT_NUM))

/* Version 1 - Stores device operation mode and sfs type in config file along with version 0 - Size: 48 bytes */
#define SIMLOCK_CONFIG_FILE_SIZE_V1      (SIMLOCK_CONFIG_FILE_SIZE_V0 + 2)

/* Version 2 - Number of categories increased to 12 - Size: 52 bytes */
#define SIMLOCK_CONFIG_FILE_SIZE_V2      (4 + ((SIMLOCK_CATEGORY_COUNT_MAX - 2) * SIMLOCK_SLOT_NUM))

/* Version 4 - Number of categories increased to 14 - Size: 60 bytes */
#define SIMLOCK_CONFIG_FILE_SIZE_V4      (4 + (SIMLOCK_CATEGORY_COUNT_MAX * SIMLOCK_SLOT_NUM))

#define SIMLOCK_CONFIG_FILE_SIZE         SIMLOCK_CONFIG_FILE_SIZE_V4

#define SIMLOCK_CATEGORY_STATUS_MASK           1
#define SIMLOCK_CATEGORY_CK_PRESENT_MASK       2

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_CONFIG_CONVERT_CATEGORY_STATUS_TO_BITMASK

DESCRIPTION
  This function converts category status to a bitmask format to be
  stored in the configuration file in EFS.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_config_convert_category_status_to_bitmask
(
  simlock_category_status_type     category_status,
  uint8                          * bitmask_ptr
)
{
  if(bitmask_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  *bitmask_ptr = 0;

  if(category_status.status == SIMLOCK_ENABLED)
  {
    *bitmask_ptr |= SIMLOCK_CATEGORY_STATUS_MASK;
  }

  /* Need to update ck_present mask seperately because of relock functionality
     where a valid CK could be present even if status is disabled  */
  if(category_status.ck_present == SIMLOCK_CK_PRESENT)
  {
    *bitmask_ptr |= SIMLOCK_CATEGORY_CK_PRESENT_MASK;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_config_convert_category_status_to_bitmask */


/*===========================================================================
FUNCTION SIMLOCK_CONFIG_CONVERT_BITMASK_TO_CATEGORY_STATUS

DESCRIPTION
  This function converts the category status stored in the configuration file
  from EFS to category status.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_config_convert_bitmask_to_category_status
(
  uint8                            bitmask,
  simlock_category_status_type   * category_status_ptr
)
{
  if(category_status_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  category_status_ptr->ck_present = SIMLOCK_CK_NOT_PRESENT;

  if(bitmask & SIMLOCK_CATEGORY_STATUS_MASK)
  {
    category_status_ptr->status = SIMLOCK_ENABLED;
  }
  else
  {
    category_status_ptr->status = SIMLOCK_DISABLED;
  }

  /* Need to update ck_present mask seperately because of relock functionality
     where a valid CK could be present even if status is disabled  */
  if(bitmask & SIMLOCK_CATEGORY_CK_PRESENT_MASK)
  {
    category_status_ptr->ck_present = SIMLOCK_CK_PRESENT;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_config_convert_bitmask_to_category_status */


/*===========================================================================
FUNCTION SIMLOCK_CONFIG_SET_POLICIES_AND_DEFAULT_STATUS

DESCRIPTION
  This function sets the configuration policies and the default status for
  all the categories on supported slots

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_config_set_policies_and_default_status
(
  simlock_slot_policy_enum_type            slot_policy,
  simlock_subscription_policy_enum_type    sub_policy,
  simlock_sfs_enum_type                    sfs_type,
  simlock_config_data_type              *  config_data_ptr
)
{
  simlock_result_enum_type    simlock_status  = SIMLOCK_SUCCESS;
  simlock_slot_enum_type      slot            = SIMLOCK_SLOT_1;
  simlock_category_enum_type  category        = SIMLOCK_CATEGORY_3GPP_NW;
  uint8                       slot_index      = 0;
  uint8                       category_index  = 0;

  if(config_data_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  config_data_ptr->sfs_type    = sfs_type;
  config_data_ptr->slot_policy = slot_policy;
  config_data_ptr->sub_policy  = sub_policy;

  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM;
      slot++)
  {
    simlock_status = simlock_util_get_slot_index(slot, &slot_index);
    if((simlock_status != SIMLOCK_SUCCESS) ||
       (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
    {
      SIMLOCK_MSG_ERR_1("failed to get slot index for slot, 0x%x", slot);
      return SIMLOCK_GENERIC_ERROR;
    }
    for(category = SIMLOCK_CATEGORY_3GPP_NW; category < SIMLOCK_CATEGORY_COUNT_MAX;
        category++)
    {
      simlock_status = simlock_util_get_category_index(category, &category_index);
      if((simlock_status != SIMLOCK_SUCCESS) ||
         (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
      {
        SIMLOCK_MSG_ERR_1("failed to get category index for category, 0x%x", category);
        return SIMLOCK_GENERIC_ERROR;
      }
      config_data_ptr->category_status[slot_index][category_index].status = SIMLOCK_DISABLED;
    }
  }

  return SIMLOCK_SUCCESS;
} /* simlock_config_set_policies_and_default_status */


/*===========================================================================
FUNCTION SIMLOCK_CONFIG_BUILD_DATA

DESCRIPTION
  This function builds the config data from the raw config data buffer

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_config_build_data
(
  uint16                       data_len,
  uint8                     *  data_ptr,
  simlock_config_data_type  *  config_data_ptr
)
{
  simlock_result_enum_type    simlock_status  = SIMLOCK_SUCCESS;
  simlock_slot_enum_type      slot            = SIMLOCK_SLOT_1;
  simlock_category_enum_type  category        = SIMLOCK_CATEGORY_3GPP_NW;
  uint8                       slot_index      = 0;
  uint8                       category_index  = 0;
  uint8                       data_index      = 0;

  if((data_ptr == NULL) ||
     (data_len != SIMLOCK_CONFIG_FILE_SIZE_V0 &&
      data_len != SIMLOCK_CONFIG_FILE_SIZE_V1 &&
      data_len != SIMLOCK_CONFIG_FILE_SIZE_V2 &&
      data_len != SIMLOCK_CONFIG_FILE_SIZE_V4) ||
     (config_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(data_len == SIMLOCK_CONFIG_FILE_SIZE_V1 ||
     data_len == SIMLOCK_CONFIG_FILE_SIZE_V2 ||
     data_len == SIMLOCK_CONFIG_FILE_SIZE_V4)
  {
    config_data_ptr->sfs_type    = data_ptr[data_index++];
    config_data_ptr->device_mode = data_ptr[data_index++];
  }

  if((data_index + 2) < data_len)
  {
    config_data_ptr->slot_policy = data_ptr[data_index++];
    config_data_ptr->sub_policy = data_ptr[data_index++];
  }

  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
  {
    simlock_status = simlock_util_get_slot_index(slot, &slot_index);
    if((simlock_status != SIMLOCK_SUCCESS) ||
       (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
    {
      SIMLOCK_MSG_ERR_1("failed to get slot index for slot, 0x%x", slot);
      return SIMLOCK_GENERIC_ERROR;
    }
    for(category = SIMLOCK_CATEGORY_3GPP_NW; category < SIMLOCK_CATEGORY_COUNT_MAX;
        category++)
    {
      simlock_status = simlock_util_get_category_index(category, &category_index);
      if((simlock_status != SIMLOCK_SUCCESS) ||
         (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
      {
        SIMLOCK_MSG_ERR_1("failed to get category index for category, 0x%x", category);
        return SIMLOCK_GENERIC_ERROR;
      }

      /* V0 and V1 versions of the configuration don't have support for SPN category.
         V0, V1 and V2 don't have support for ICCID category*/
      if (((data_len == SIMLOCK_CONFIG_FILE_SIZE_V0 ||
            data_len == SIMLOCK_CONFIG_FILE_SIZE_V1) &&
          (category == SIMLOCK_CATEGORY_3GPP_SPN))
                             ||
          ((data_len == SIMLOCK_CONFIG_FILE_SIZE_V0 ||
            data_len == SIMLOCK_CONFIG_FILE_SIZE_V1 ||
            data_len == SIMLOCK_CONFIG_FILE_SIZE_V2) &&
          (category == SIMLOCK_CATEGORY_3GPP_ICCID)))
      {
        config_data_ptr->category_status[slot_index][category_index].status = SIMLOCK_DISABLED;
        config_data_ptr->category_status[slot_index][category_index].ck_present = SIMLOCK_CK_NOT_PRESENT;
        continue;
      }

      if(data_index < data_len)
      {
        simlock_status = simlock_config_convert_bitmask_to_category_status(
                           data_ptr[data_index],
                           &config_data_ptr->category_status[slot_index][category_index]);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          return SIMLOCK_GENERIC_ERROR;
        }

        data_index++;
      }
    }
  }

  if(data_index != data_len)
  {
    SIMLOCK_MSG_ERR_1("unexpected data buffer size, 0x%x",
                       data_index);
    return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_config_build_data */


/*===========================================================================
FUNCTION SIMLOCK_CONFIG_WRITE_DATA

DESCRIPTION
  This function writes the configuration data to the config file

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_config_write_data
(
  simlock_config_data_type  * config_data_ptr,
  simlock_sfs_enum_type       sfs_type
)
{
  simlock_result_enum_type    simlock_status  = SIMLOCK_SUCCESS;
  simlock_slot_enum_type      slot            = SIMLOCK_SLOT_1;
  simlock_category_enum_type  category        = SIMLOCK_CATEGORY_3GPP_NW;
  uint8                       slot_index      = 0;
  uint8                       category_index  = 0;
  uint8                       data_buf_index  = 0;
  uint8                       data_buf[SIMLOCK_CONFIG_FILE_SIZE];

  if(config_data_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(data_buf, 0x00, sizeof(data_buf));

  if((data_buf_index + 4) < SIMLOCK_CONFIG_FILE_SIZE)
  {
    data_buf[data_buf_index++]  = config_data_ptr->sfs_type;
    data_buf[data_buf_index++]  = config_data_ptr->device_mode;
    data_buf[data_buf_index++]  = config_data_ptr->slot_policy;
    data_buf[data_buf_index++]  = config_data_ptr->sub_policy;
  }

  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM;
      slot++)
  {
    simlock_status = simlock_util_get_slot_index(slot, &slot_index);
    if((simlock_status != SIMLOCK_SUCCESS) ||
       (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
    {
      SIMLOCK_MSG_ERR_1("failed to get slot index for slot, 0x%x", slot);
      return SIMLOCK_GENERIC_ERROR;
    }
    for(category = 0; category < SIMLOCK_CATEGORY_COUNT_MAX;
        category++)
    {
      simlock_status = simlock_util_get_category_index(category, &category_index);
      if((simlock_status != SIMLOCK_SUCCESS) ||
         (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
      {
        SIMLOCK_MSG_ERR_1("failed to get category index for category, 0x%x", category);
        return SIMLOCK_GENERIC_ERROR;
      }
      if(data_buf_index < SIMLOCK_CONFIG_FILE_SIZE)
      {
        simlock_status = simlock_config_convert_category_status_to_bitmask(
                           config_data_ptr->category_status[slot_index][category_index],
                           &data_buf[data_buf_index]);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          return SIMLOCK_GENERIC_ERROR;
        }

        data_buf_index++;
      }
    }
  }

  if(data_buf_index != SIMLOCK_CONFIG_FILE_SIZE)
  {
    SIMLOCK_MSG_ERR_1("unexpected data buffer size, 0x%x",
                       data_buf_index);
    return SIMLOCK_GENERIC_ERROR;
  }

  return simlock_file_write(data_buf,
                            SIMLOCK_CONFIG_FILE_SIZE,
                            SIMLOCK_FILE_CONFIG,
                            sfs_type);
} /* simlock_config_write_data */


/*===========================================================================
FUNCTION SIMLOCK_CONFIG_CREATE_CONFIG_FILE

DESCRIPTION
  This function creates a default config file during the power-up. All the
  categories are disabled by default. It then writes the config file with
  the default data.

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
)
{
  simlock_config_data_type  config_data;

  memset(&config_data, 0x00, sizeof(simlock_config_data_type));

  if(simlock_config_set_policies_and_default_status(SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS,
                                                    SIMLOCK_SUBSCRIPTION_POLICY_INDEPENDENT,
                                                    SIMLOCK_SFS_LOCAL,
                                                    &config_data) != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  return simlock_config_write_data(&config_data, SIMLOCK_SFS_LOCAL);
} /* simlock_config_create_config_file */


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
)
{
  char  * simlock_dir_ptr = NULL;

  switch (dir_type)
  {
    case SIMLOCK_DIR_CONFIG:
      simlock_dir_ptr = simlock_sfs_dir;
      break;

    case SIMLOCK_DIR_UIM:
      simlock_dir_ptr = uim_efs_dir;
      break;

    case SIMLOCK_DIR_SFS:
      simlock_dir_ptr = sfs_efs_dir;
      break;

    /* we don't create /safe directory.
       It is always expected to be present */
    case SIMLOCK_DIR_SAFE:
    default:
      break;
  }

  if(simlock_dir_ptr == NULL)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* config directory is created on the SFS */
  if(dir_type == SIMLOCK_DIR_CONFIG)
  {
    return simlock_file_mkdir(simlock_dir_ptr);
  }
  else
  {
    return simlock_file_mkdir_efs(simlock_dir_ptr);
  }
} /* simlock_config_create_dir */


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
)
{
  simlock_result_enum_type simlock_status = SIMLOCK_SUCCESS;
  uint32                   sfs_file_len   = 0;

  /* the config file size should be either
     SIMLOCK_CONFIG_FILE_SIZE_V0 (without device_mode and sfs type indicator bytes) or
     SIMLOCK_CONFIG_FILE_SIZE_V1 (with device_mode and sfs type indicator bytes) or
     SIMLOCK_CONFIG_FILE_SIZE_V2 (with device_mode and sfs type indicator bytes and 12 categories) or
     SIMLOCK_CONFIG_FILE_SIZE_V4 (with device_mode and sfs type indicator bytes and 14 categories)*/
  simlock_status = simlock_file_check(simlock_sfs_config_file, &sfs_file_len);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (sfs_file_len != SIMLOCK_CONFIG_FILE_SIZE_V0 &&
      sfs_file_len != SIMLOCK_CONFIG_FILE_SIZE_V1 &&
      sfs_file_len != SIMLOCK_CONFIG_FILE_SIZE_V2 &&
      sfs_file_len != SIMLOCK_CONFIG_FILE_SIZE_V4))
  {
    SIMLOCK_MSG_ERR_2("config file check failed, status: 0x%x, len: 0x%x",
                       simlock_status, sfs_file_len);
    return SIMLOCK_GENERIC_ERROR;
  }
  return SIMLOCK_SUCCESS;
} /* simlock_config_check_config_file */


/*===========================================================================
FUNCTION SIMLOCK_CONFIG_READ_CONFIG_DATA_FROM_SFS

DESCRIPTION
  This function reads the configuration data from the requested SFS 
  (LOCAL/REMOTE)

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_config_read_config_data_from_sfs
(
  simlock_sfs_enum_type       sfs_type,
  simlock_config_data_type  * config_data_ptr
)
{
  simlock_result_enum_type  simlock_status      = SIMLOCK_SUCCESS;
  uint32                    data_len            = 0;
  uint8                   * data_buf_ptr        = NULL;

  if(config_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  do
  {
    simlock_status = simlock_file_read(&data_buf_ptr,
                                       &data_len,
                                       SIMLOCK_FILE_CONFIG,
                                       sfs_type);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("config file read failed, status: 0x%x", simlock_status);
      break;
    }

    if((data_len != SIMLOCK_CONFIG_FILE_SIZE_V0  &&
        data_len != SIMLOCK_CONFIG_FILE_SIZE_V1  &&
        data_len != SIMLOCK_CONFIG_FILE_SIZE_V2  &&
        data_len != SIMLOCK_CONFIG_FILE_SIZE_V4) ||
       (data_buf_ptr == NULL))
    {
      SIMLOCK_MSG_ERR_1("incorrect config file len: 0x%x", data_len);
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
    }

    simlock_status = simlock_config_build_data((uint16)data_len,
                                                data_buf_ptr,
                                                config_data_ptr);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("config file build data failed, status: 0x%x", simlock_status);
      break;
    }
  }while (0);

  SIMLOCK_MEM_FREE(data_buf_ptr);
  return simlock_status;
} /* simlock_config_read_config_data_from_sfs */


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
)
{
  simlock_result_enum_type  simlock_status      = SIMLOCK_SUCCESS;

  if(config_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  simlock_status = simlock_config_read_config_data_from_sfs(SIMLOCK_SFS_LOCAL,
                                                            config_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* local SFS is being used to store data, so return */
  if(config_data_ptr->sfs_type == SIMLOCK_SFS_LOCAL)
  {
    return SIMLOCK_SUCCESS;
  }

  /* in case remote SFS storage is being used,
     read the config file from the remote SFS */
  simlock_status = simlock_config_read_config_data_from_sfs(SIMLOCK_SFS_REMOTE,
                                                            config_data_ptr);

  return simlock_status;
} /* simlock_config_read_config_data */


/*===========================================================================
FUNCTION SIMLOCK_CONFIG_UPDATE_AND_WRITE_POLICIES

DESCRIPTION
  This function writes the slot and subscription policy in the config file.
  This function is expected to be called before any SIMLock category is
  enabled.

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
)
{
  simlock_sfs_enum_type  sfs_type = SIMLOCK_SFS_LOCAL;

  if((config_policy_ptr == NULL) ||
     (config_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* retrieve the type of SFS being used to store the data */
  sfs_type = simlock_config_read_sfs_type();

  SIMLOCK_MSG_HIGH_2("slot_policy: 0x%x, sub_policy: 0x%x",
                     config_policy_ptr->slot_policy, config_policy_ptr->sub_policy);

  if(simlock_config_set_policies_and_default_status(config_policy_ptr->slot_policy,
                                                    config_policy_ptr->sub_policy,
                                                    sfs_type,
                                                    config_data_ptr) != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  return simlock_config_write_data(config_data_ptr, sfs_type);
} /* simlock_config_update_and_write_policies */


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
)
{
  simlock_slot_policy_enum_type  slot_policy  = SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS;
  uint8                          other_index  = 0;

  SIMLOCK_MSG_MED_0("simlock_config_update_and_write_category_status");

   if(config_data_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(slot_index >= SIMLOCK_SLOT_COUNT_MAX)
  {
    SIMLOCK_MSG_ERR_1("invalid slot index, 0x%x", slot_index);
    return SIMLOCK_GENERIC_ERROR;
  }

  if(category_index >= SIMLOCK_CATEGORY_COUNT_MAX)
  {
    SIMLOCK_MSG_ERR_1("invalid category index, 0x%x",
                      category_index);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* set the category information to identify whether SimLock is enabled
     and whether the CK is present. */
  config_data_ptr->category_status[slot_index][category_index].status = status;
  config_data_ptr->category_status[slot_index][category_index].ck_present = ck_present;

  slot_policy = config_data_ptr->slot_policy;

  /* if the slot policy is same for all slots, then update the
     status of the category, corresponding to other slots as well */
  if(slot_policy == SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS)
  {
    for (other_index = 0; other_index < SIMLOCK_SLOT_COUNT_MAX;
         other_index++)
    {
      if(other_index == slot_index)
      {
        continue;
      }

      /* set the category information to identify whether SimLock is enabled
         and whether the CK is present. */
      config_data_ptr->category_status[other_index][category_index].status     = status;
      config_data_ptr->category_status[other_index][category_index].ck_present = ck_present;
    }
  }

  return simlock_config_write_data(config_data_ptr, config_data_ptr->sfs_type);
} /* simlock_config_update_and_write_category_status */


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
)
{
  char  * simlock_dir_ptr = NULL;

  switch (dir_type)
  {
    case SIMLOCK_DIR_SAFE:
      simlock_dir_ptr = safe_efs_dir;
      break;

    case SIMLOCK_DIR_UIM:
      simlock_dir_ptr = uim_efs_dir;
      break;

    case SIMLOCK_DIR_SFS:
      simlock_dir_ptr = sfs_efs_dir;
      break;

    case SIMLOCK_DIR_CONFIG:
    default:
      break;
  }

  if(simlock_dir_ptr == NULL)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  return simlock_efs_name_test(simlock_dir_ptr);
} /* simlock_config_check_dir */


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
)
{
  if(config_data_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_config_update_and_write_device_mode");

  /* set the device mode */
  config_data_ptr->device_mode = device_mode;

  return simlock_config_write_data(config_data_ptr, config_data_ptr->sfs_type);
} /* simlock_config_update_and_write_device_mode */


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
)
{
  simlock_result_enum_type  simlock_status  = SIMLOCK_SUCCESS;

  SIMLOCK_MSG_MED_0("simlock_config_update_and_write_sfs_type");

  if(config_data_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* set the SFS type */
  config_data_ptr->sfs_type = update_sfs_type;

  simlock_status = simlock_config_write_data(config_data_ptr, update_sfs_type);

  if((update_sfs_type == SIMLOCK_SFS_REMOTE) &&
     (simlock_status == SIMLOCK_SUCCESS))
  {
    /* update the local config indicating that the
       remote config file is being used for simlock */
    simlock_status = simlock_config_write_data(config_data_ptr, SIMLOCK_SFS_LOCAL);
  }

  return simlock_status;
} /* simlock_config_update_and_write_sfs_type */


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
)
{
  simlock_result_enum_type  simlock_status = SIMLOCK_SUCCESS;
  simlock_config_data_type  config_data;

  memset(&config_data, 0x00, sizeof(simlock_config_data_type));

  simlock_status = simlock_config_read_config_data_from_sfs(SIMLOCK_SFS_LOCAL,
                                                            &config_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_SFS_LOCAL;
  }

  return config_data.sfs_type;
} /* simlock_config_read_sfs_type */

#endif /* FEATURE_SIMLOCK */


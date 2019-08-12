/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   S W I T C H   S F S   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock functions to switch usage of 
  local to remote SFS or vice-versa


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS 


                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_switch_sfs.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
09/29/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_switch_sfs.h"
#include "simlock.h"
#include "simlock_modem_p.h"
#include "simlock_util.h"
#include "simlock_file.h"
#include "simlock_config.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_SWITCH_SFS_FOR_CATEGORY_DATA

DESCRIPTION
  This function reads the simlock data for the locked cateories, from a
  SFS (LOCAL/REMOTE) and writes to another SFS (REMOTE/LOCAL)

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static simlock_result_enum_type simlock_switch_sfs_for_category_data
(
  const simlock_config_data_type       * config_data_ptr,
  simlock_sfs_enum_type                  read_from_sfs,
  simlock_sfs_enum_type                  write_to_sfs
)
{
  simlock_result_enum_type       simlock_status          = SIMLOCK_SUCCESS;
  simlock_slot_enum_type         slot                    = SIMLOCK_SLOT_1;
  simlock_category_enum_type     category                = SIMLOCK_CATEGORY_3GPP_NW;
  simlock_slot_policy_enum_type  slot_policy             = SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS;
  simlock_data_file_enum_type    category_file_enum      = SIMLOCK_FILE_3GPP_NW_1;
  uint32                         category_file_size      = 0;
  uint8                       *  category_file_data_ptr  = NULL;

  if(config_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  slot_policy = config_data_ptr->slot_policy;

  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM;
      slot++)
  {
    /* if the slot policy is same on all the available slots, then
       it is expected to have one category file per each enabled category
       i.e. all the codes for that category are in one file */
    if((slot_policy == SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS) &&
       (slot != SIMLOCK_SLOT_1))
    {
      break;
    }

    for(category = 0; category < SIMLOCK_CATEGORY_COUNT_MAX;
        category++)
    {
       /* if the category is not enabled, nothing to do */
      if(!simlock_util_check_category_enabled(config_data_ptr, category, slot))
      {
        continue;
      }

      /* compose the category file */
      category_file_enum = simlock_util_compose_category_file_enum(category, slot);
      if(category_file_enum == SIMLOCK_FILE_ENUM_INVALID)
      {
        return SIMLOCK_GENERIC_ERROR;
      }

      /* this allocates memory for the data read */
      simlock_status = simlock_file_read(&category_file_data_ptr,
                                         &category_file_size,
                                         category_file_enum,
                                         read_from_sfs);
      if((simlock_status != SIMLOCK_SUCCESS) ||
         (category_file_size == 0) ||
         (category_file_data_ptr == NULL))
      {
        SIMLOCK_MSG_ERR_2("read category file failed, status: 0x%x, file len: 0x%x",
                           simlock_status, category_file_size);
        SIMLOCK_MEM_FREE(category_file_data_ptr);
        return simlock_status;
      }

      /* write the lock data to the catefory file */
      simlock_status = simlock_file_write(category_file_data_ptr,
                                          category_file_size,
                                          category_file_enum,
                                          write_to_sfs);

      SIMLOCK_MEM_FREE(category_file_data_ptr);

      if(simlock_status != SIMLOCK_SUCCESS)
      {
        SIMLOCK_MSG_ERR_1("failed to write category file, status: 0x%x",
                           simlock_status);
        return simlock_status;
      }
    }
  }

  return SIMLOCK_SUCCESS;
} /* simlock_switch_sfs_for_category_data */


/*===========================================================================
FUNCTION SIMLOCK_SWITCH_SFS

DESCRIPTION
  This function switches the simlock data storage from local to remote
  SFS or vice-versa

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static simlock_result_enum_type simlock_switch_sfs
(
  simlock_config_data_type              * config_data_ptr,
  simlock_remote_sfs_request_enum_type    switch_sfs_request
)
{
  simlock_result_enum_type    simlock_status    = SIMLOCK_SUCCESS;
  simlock_sfs_enum_type       read_sfs          = SIMLOCK_SFS_LOCAL;
  simlock_sfs_enum_type       write_sfs         = SIMLOCK_SFS_LOCAL;

  switch(switch_sfs_request)
  {
    case SIMLOCK_REMOTE_SFS_ENABLE:
      read_sfs = SIMLOCK_SFS_LOCAL;
      write_sfs = SIMLOCK_SFS_REMOTE;
      break;

    case SIMLOCK_REMOTE_SFS_DISABLE:
      read_sfs = SIMLOCK_SFS_REMOTE;
      write_sfs = SIMLOCK_SFS_LOCAL;
      break;

    default:
      return SIMLOCK_INCORRECT_PARAMS;
  }

  simlock_status = simlock_switch_sfs_for_category_data(config_data_ptr, read_sfs, write_sfs);
  if(simlock_status == SIMLOCK_SUCCESS)
  {
    /* this indicates either all enabled category data is successfully
       switched */
    simlock_status = simlock_config_update_and_write_sfs_type(write_sfs, config_data_ptr);
  }

  return simlock_status;
} /* simlock_switch_sfs */


/*===========================================================================
FUNCTION SIMLOCK_SET_REMOTE_SFS_REQUEST

DESCRIPTION
  This function processes the request to enable or disable the remote SFS
  storage

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_set_remote_sfs_request
(
  const simlock_set_remote_sfs_msg_req_type * msg_request_ptr
)
{
  simlock_result_enum_type  simlock_status = SIMLOCK_SUCCESS;
  simlock_sfs_enum_type     sfs_type       = SIMLOCK_SFS_LOCAL;
  simlock_config_data_type  config_file_data;

  if(msg_request_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_1("simlock_set_remote_sfs_request, req_type: 0x%x",
                    msg_request_ptr->request_type);

  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* retrieve the type of SFS being used */
  sfs_type = simlock_config_read_sfs_type();

  if(((sfs_type == SIMLOCK_SFS_REMOTE) &&
      (msg_request_ptr->request_type == SIMLOCK_REMOTE_SFS_ENABLE))
                  ||
     ((sfs_type == SIMLOCK_SFS_LOCAL) &&
      (msg_request_ptr->request_type == SIMLOCK_REMOTE_SFS_DISABLE)))
  {
    SIMLOCK_MSG_ERR_0("remote sfs request invalid");
    return SIMLOCK_INVALID_STATE;
  }

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("read config file failed");
    return simlock_status;
  }

  simlock_status = simlock_switch_sfs(&config_file_data,
                                      msg_request_ptr->request_type);

  return simlock_status;
} /* simlock_set_remote_sfs_request */

#endif /* FEATURE_SIMLOCK */


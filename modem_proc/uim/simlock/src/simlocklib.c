/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   L I B R A R Y   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock library functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS 


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlocklib.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
12/31/15   stv     Send events to client from engine msg handling
07/01/15   bcho    Support for whitelist and blacklist codes in same category
07/07/15   stv     Perform efs sync for remote sfs command
03/19/15   at      Introducing new relock feature in SIMLock
12/11/14   vv      Perform efs_sync() before writing the data
09/29/14   vv      Added support for emergency only mode
09/09/14   vv      Perform efs_sync() after writing the data
08/29/14   vv      Added support for temporary unlock
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlocklib.h"
#include "simlock.h"
#include "simlock_file.h"
#include "simlock_efs.h"
#include "simlock_modem.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_PROCESS_MESSAGE

DESCRIPTION
  This is the function which processes the requests to the SIMLock feature. It
  is the entry point to the various requests the SIMLock feature supports

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_process_message
(
  const simlock_message_request_data_type * msg_req_ptr,
  simlock_message_response_data_type      * msg_resp_ptr
)
{
  simlock_result_enum_type simlock_status = SIMLOCK_SUCCESS;

  if(msg_req_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(msg_resp_ptr != NULL)
  {
    msg_resp_ptr->msg_type = msg_req_ptr->msg_type;
  }

  /* Other requests perform a write to the file system. So need to
     ensure that the file system is ready for use. In case of
     init, we perform write if config file is not present anyway */
  if(msg_req_ptr->msg_type != SIMLOCK_RUN_ALGORITHM_MSG        &&
     msg_req_ptr->msg_type != SIMLOCK_GET_STATUS_MSG           &&
     msg_req_ptr->msg_type != SIMLOCK_GET_CATEGORY_DATA_MSG    &&
     msg_req_ptr->msg_type != SIMLOCK_INIT_MSG)
  {
    if(simlock_efs_check_efs_status() != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_0("simlock_efs_check_efs_status failed");
      return SIMLOCK_INVALID_STATE;
    }
  }

  switch(msg_req_ptr->msg_type)
  {
    case SIMLOCK_INIT_MSG:
      simlock_status = simlock_init();
      break;

    case SIMLOCK_SET_SIMLOCK_POLICY_MSG:
      simlock_status = simlock_set_simlock_policy(&msg_req_ptr->message.config_policy_msg);
      break;

    case SIMLOCK_RUN_ALGORITHM_MSG:
      if(msg_resp_ptr == NULL)
      {
        return SIMLOCK_INCORRECT_PARAMS;
      }
      simlock_status = simlock_run_algorithm(&msg_req_ptr->message.algorithm_msg,
                                             &msg_resp_ptr->message.run_algo_resp);
      break;

    case SIMLOCK_SET_LOCK_CK_MSG:
      simlock_status = simlock_set_lock_ck(&msg_req_ptr->message.lock_ck_msg);
      break;

    case SIMLOCK_SET_LOCK_HCK_MSG:
      simlock_status = simlock_set_lock_hck(&msg_req_ptr->message.lock_hck_msg);
      break;

    case SIMLOCK_UNLOCK_DEVICE_MSG:
      if(msg_resp_ptr == NULL)
      {
        return SIMLOCK_INCORRECT_PARAMS;
      }
      simlock_status = simlock_unlock_device(&msg_req_ptr->message.unlock_msg,
                                             &msg_resp_ptr->message.unlock_resp);
      break;

    case SIMLOCK_GET_STATUS_MSG:
      if(msg_resp_ptr == NULL)
      {
        return SIMLOCK_INCORRECT_PARAMS;
      }
      simlock_status = simlock_get_lock_status(&msg_resp_ptr->message.get_status_resp);
      break;

    case SIMLOCK_GET_CATEGORY_DATA_MSG:
      if(msg_resp_ptr == NULL)
      {
        return SIMLOCK_INCORRECT_PARAMS;
      }
      simlock_status = simlock_get_lock_category_data(&msg_req_ptr->message.category_data_msg,
                                                      &msg_resp_ptr->message.get_category_data_resp);
      break;

    case SIMLOCK_TEMPORARY_UNLOCK_MSG:
      simlock_status = simlock_temporary_unlock_request(&msg_req_ptr->message.temporary_unlock_msg);
      break;

    case SIMLOCK_SET_DEVICE_MODE_MSG:
      simlock_status = simlock_set_device_mode_request(&msg_req_ptr->message.device_mode_msg);
      break;

    case SIMLOCK_ADD_LOCK_CODES_CK_MSG:
      simlock_status = simlock_add_lock_codes_ck_request(&msg_req_ptr->message.add_lock_codes_ck_msg);
      break;

    case SIMLOCK_ADD_LOCK_CODES_HCK_MSG:
      simlock_status = simlock_add_lock_codes_hck_request(&msg_req_ptr->message.add_lock_codes_hck_msg);
      break;

    case SIMLOCK_RELOCK_DEVICE_MSG:
      simlock_status = simlock_relock_device(&msg_req_ptr->message.relock_msg,
                                             &msg_resp_ptr->message.relock_resp);
      break;

    default:
      SIMLOCK_MSG_ERR_1("invalid request type, 0x%x",
                        msg_req_ptr->msg_type);
      return SIMLOCK_GENERIC_ERROR;
  }

  /* Send event to client if client is registered
     and the processed command requires an event
     to be sent to client i.e. MMGSDI */
  simlock_send_event(simlock_status,
                     msg_req_ptr,
                     msg_resp_ptr);

  SIMLOCK_MSG_HIGH_1("simlock process message, status: 0x%x", simlock_status);
  return simlock_status;
} /* simlock_process_message */

#endif /* FEATURE_SIMLOCK */


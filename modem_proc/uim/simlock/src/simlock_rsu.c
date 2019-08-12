/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   R S U   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock RSU functions.


                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_rsu.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/06/16   stv     Remote simlock support
07/07/15   stv     Fix to perform efs sync operation for critical file updates
05/15/15   vv      Support for RSU set configuration in simlock
05/15/15   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#if defined (FEATURE_SIMLOCK) && (defined (FEATURE_SIMLOCK_RSU) || defined (FEATURE_SIMLOCK_QC_REMOTE_LOCK))

/* The following is to avoid enabling both Remote SimLock features at a time */
#if defined (FEATURE_SIMLOCK_RSU) && defined (FEATURE_SIMLOCK_QC_REMOTE_LOCK)
#error "Two separate Remote SimLock features are defined"
#endif /* FEATURE_SIMLOCK_RSU && FEATURE_SIMLOCK_QC_REMOTE_LOCK */

#include "simlock.h"
#include "simlock_file.h"
#include "simlock_efs.h"
#include "simlock_rsu.h"
#include "simlock_rsu_key.h"
#include "simlock_rsu_config.h"
#include "simlock_modem.h"


/*===========================================================================
FUNCTION SIMLOCK_RSU_PROCESS_MESSAGE

DESCRIPTION
  This is the function which processes the RSU requests in the SIMLock.
  It is the entry point to the various requests in RSU feature.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_process_message
(
  const simlock_rsu_msg_req_type          * msg_req_ptr,
  simlock_data_type                       * msg_resp_ptr
)
{
  simlock_result_enum_type simlock_status = SIMLOCK_SUCCESS;

  if(msg_req_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  switch(msg_req_ptr->msg_type)
  {
    case SIMLOCK_RSU_GET_KEY_MSG:
#ifdef FEATURE_SIMLOCK_RSU
      if(msg_resp_ptr == NULL)
      {
        return SIMLOCK_INCORRECT_PARAMS;
      }
      simlock_status = simlock_rsu_keymod_generate_and_encrypt(msg_resp_ptr);
#else
      simlock_status = SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SIMLOCK_RSU */
      break;

    case SIMLOCK_RSU_SET_CONFIGURATION_MSG:
      simlock_status = simlock_rsu_config_set_data(msg_req_ptr->message.rsu_data.data_len,
                                                   msg_req_ptr->message.rsu_data.data_ptr);
      break;

    default:
      SIMLOCK_MSG_ERR_1("invalid request type, 0x%x",
                        msg_req_ptr->msg_type);
      return SIMLOCK_UNSUPPORTED;
  }

  SIMLOCK_MSG_MED_1("simlock process RSU message, status: 0x%x", simlock_status);
  return simlock_status;
} /* simlock_rsu_process_message */

#endif /* FEATURE_SIMLOCK && (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) */

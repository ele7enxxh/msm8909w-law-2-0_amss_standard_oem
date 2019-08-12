#ifndef SNS_RH_RESTRICTED_SVC_H
#define SNS_RH_RESTRICTED_SVC_H

/*============================================================================

  @file sns_rh_restricted_svc.h

  @brief
  This file contains definition for the Restricted service support in
  the Request Handler

 Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_restricted_svc.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-04-23  pn    Initial version
============================================================================*/
#include "sns_rh.h"

/*===========================================================================

                   SMGR MACRO

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                   STRUCTURE DEFINITIONS

===========================================================================*/

/*===========================================================================

                          FUNCTIONS

===========================================================================*/
extern sns_err_code_e sns_rh_process_restricted_smgr_request(
  sns_rh_mr_req_q_item_s**  msg_ptr_ptr);

extern void sns_rh_process_restricted_smgr_response(
  sns_smgr_request_response_s* req_resp_ptr);

extern void sns_rh_cancel_restricted_service(void* connection_handle);

#endif /* SNS_RH_RESTRICTED_SVC_H */


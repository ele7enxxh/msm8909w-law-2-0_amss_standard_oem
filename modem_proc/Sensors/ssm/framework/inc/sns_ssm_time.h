#ifndef SNS_SSM_TIME_H
#define SNS_SSM_TIME_H
/*=============================================================================
  @file sns_ssm_time

  Sensors Service Manager's Time Service header file

  This header file contains the functions required by the Time Service

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/ssm/framework/inc/sns_ssm_time.h#1 $  */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2014-01-30  dk   Initial version

=============================================================================*/


/*-----------------------------------------------------------------------------
* Include Files
* ---------------------------------------------------------------------------*/

#include "sns_common.h"
#include "sns_common_v01.h"
#include "sns_em.h"
#include "sns_time_api_v02.h"

/*-----------------------------------------------------------------------------
* sns_ssm_time_init
* ---------------------------------------------------------------------------*/
sns_err_code_e sns_ssm_time_init(void);

/*-----------------------------------------------------------------------------
* sns_ssm_time_proc_req
* ---------------------------------------------------------------------------*/
sns_err_code_e sns_ssm_time_proc_req(qmi_req_handle req_handle);

#endif /* #ifndef SNS_SSM_TIME_H */

#ifndef SNS_SSM_SYNC_PROXY_H
#define SNS_SSM_SYNC_PROXY_H
/*=============================================================================
  @file sns_ssm_sync_proxy

  Sensors Service Manager's Sync_Proxy Service header file

  This header file contains the functions required by the Sync_Proxy Service

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/ssm/framework/inc/sns_ssm_sync_proxy.h#1 $  */
/* $DateSync_Proxy: 2014/09/22 15:13:41 $ */
/* $Author: mplcsds1 $ */

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2015-06-21  hw    Port sync proxy feature to SLPI
  2015-01-28  hw    Adding related API for supporting synchronous mode of Sync Proxy
  2014-09-23  jtl   Initial version

=============================================================================*/


/*-----------------------------------------------------------------------------
* Include Files
* ---------------------------------------------------------------------------*/

#include "sns_common.h"
#include "sns_common_v01.h"
#include "sns_sync_proxy_v01.h"
#include "sns_ssm_priv.h"

/*-----------------------------------------------------------------------------
* sns_ssm_sync_proxy_init
* ---------------------------------------------------------------------------*/
sns_err_code_e sns_ssm_sync_proxy_init(void);

/*-----------------------------------------------------------------------------
* sns_ssm_sync_proxy_proc_req
* ---------------------------------------------------------------------------*/
sns_err_code_e sns_ssm_sync_proxy_proc_req
(
 sns_ssm_qmi_connection_s *connection_handle,
 qmi_req_handle req_handle,
 unsigned int msg_id,
 void *req_c_struct
);

/*-----------------------------------------------------------------------------
* sns_ssm_sync_proxy_proc_discon
*
* Process the disconnect request. Thus release the smr connection with
* Fast-AMD and SMGR.
* ---------------------------------------------------------------------------*/
sns_err_code_e sns_ssm_sync_proxy_proc_discon(void);

/*-----------------------------------------------------------------------------
* sns_ssm_sync_proxy_proc_signals
*
* Check the signals received by SSM and process them accordingly
* ---------------------------------------------------------------------------*/
void sns_ssm_sync_proxy_proc_signals(OS_FLAGS sigFlag);

#endif /* #ifndef SNS_SSM_SYNC_PROXY_H */

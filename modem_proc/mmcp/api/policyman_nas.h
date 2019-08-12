#ifndef _POLICYMAN_NAS_H_
#define _POLICYMAN_NAS_H_

/**
  @file policyman_nas.h

  @brief Interface to Policy Manager for NAS APIs
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/policyman_nas.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include <sys/types.h>
#include "sys.h"


#ifdef __cplusplus
}
#endif


/*=============================================================================
  API to return current MCC.
=============================================================================*/

/*-------- policyman_get_current_mcc --------*/
/**
@brief
  This API returns the current MCC based on which Policy Manager is taking a decision
  to stay in HOME or ROAM configuration. API return value is controlled via XML and
  client should read MCC only when POLICYMAN_STATUS_SUCCESS is returned.
  In actuality this may not be the MCC on which capability is decided for reqeusted
  subscription but it is still the best guess.

@param[in] subsId: Subscription Id for which MCC is required
@param[in] pMcc  : Pointer to a container to store MCC value

@return
  POLICYMAN_STATUS_SUCCESS if valid MCC is returned.
  Relevant error is returned otherwise.
*/
policyman_status_t  policyman_get_current_mcc(
  sys_modem_as_id_e_type  subsId,
  sys_mcc_type           *pMcc
);


/*=============================================================================
  API to return changes that would result from camping on a given PLMN.
=============================================================================*/

/*-------- policyman_report_plmn_candidate --------*/
/**
@brief  Report a potential PLMN and find out what camping on it will do
        in terms of policy.

  This function runs policyman in a sandbox with the assumption that the
  specified subscription has camped on the specified PLMN, and returns the
  new configuration that would result if this were to happen.

@param[in]  subsId    Subscription Id for which MCC is required
@param[in]  pPlmn     Pointer to the PLMN in question.

@return
  NULL if no change will occur, or a pointer to a MSGR message identical to that
  received by a client of the POLICYMAN_CFG_UPDATE_IND message if the
  configuration would change.
  If a non-NULL value is returned from this function, the caller should call
  policyman_report_msg_free() on that value when done with the message in
  order to free it.
*/
msgr_hdr_s * policyman_report_plmn_candidate(
  sys_modem_as_id_e_type  subsId,
  sys_plmn_id_s_type     *pPlmn
);

#endif /* _POLICYMAN_NAS_H_ */

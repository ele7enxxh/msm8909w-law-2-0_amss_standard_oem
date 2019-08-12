/**
  @file policyman_obsolete_apis.c

  @brief  FIle for obsolete APIs
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_obsolete_apis.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/
#include "policyman.h"


/*-------- policyman_sglte_plmn_is_sglte --------*/
policyman_status_t policyman_sglte_plmn_is_sglte(
  sys_plmn_id_s_type  *pPlmnId,
  boolean             *pIsSglte
)
{
  if (pPlmnId == NULL || pIsSglte == NULL)
  {
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  *pIsSglte = FALSE;
  return POLICYMAN_STATUS_ERR;

}


/*-------- policyman_is_sglte_allowed --------*/
policyman_status_t policyman_is_sglte_allowed(
  boolean             *pIsSglte
)
{
  if (!pIsSglte)
  {
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  *pIsSglte = FALSE;
  return POLICYMAN_STATUS_ERR;
}

#ifndef _POLICYMAN_VOICE_DOMAIN_PREF_H_
#define _POLICYMAN_VOICE_DOMAIN_PREF_H_

/**
  @file policyman_voice_domain_pref.h

  @brief
*/

/*
    Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_voice_domain_pref.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_i.h"
#include "policyman_diag.h"

/*=============================================================================
  Voice Domain Pref APIs
=============================================================================*/

typedef struct policyman_voice_domain_pref_item_t policyman_voice_domain_pref_item_t;

/*-------- policyman_voice_domain_pref_item_new --------*/
/**
@brief

@param

@return

*/
policyman_voice_domain_pref_item_t * policyman_voice_domain_pref_item_new(
  void
);



/*-------- policyman_voice_domain_pref_get_default --------*/
/**
@brief

@param

@return

*/
policyman_item_t * policyman_voice_domain_pref_get_default(
  policy_execute_ctx_t *pCtx
);


/*-------- policyman_voice_domain_pref_compare --------*/
/**
@brief

@param

@return

*/
boolean policyman_voice_domain_pref_compare(
  policyman_item_t  *pItem1,
  policyman_item_t  *pItem2
);

/*-------- policyman_retrieve_voice_domain --------*/
boolean policyman_retrieve_voice_domain(
  sys_modem_as_id_e_type        subsId,
  sys_voice_domain_pref_e_type *pVoiceDomain
);

/*-------- policyman_voice_domain_pref_display --------*/
void policyman_voice_domain_pref_display(
  policyman_item_t  *pItem
);

/*-------- policyman_voice_domain_pref_action_new --------*/
mre_status_t policyman_voice_domain_pref_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
);

/*-------- policyman_condition_voice_domain_pref_new --------*/
mre_status_t policyman_condition_voice_domain_pref_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);

/*-------- policyman_voice_domain_pref_fill_subs_diag_log --------*/
void policyman_voice_domain_pref_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
);

#endif /* _POLICYMAN_VOICE_DOMAIN_PREF_H_ */

#ifndef _POLICYMAN_VOLTE_H_
#define _POLICYMAN_VOLTE_H_

/**
  @file policyman_volte.h

  @brief
*/

/*
    Copyright (c) 2014,2015 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_volte.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

/*=============================================================================
  "<volte />" APIs
=============================================================================*/


/*=============================================================================
  "<volte />" Actions
=============================================================================*/

/*-------- policyman_volte_action_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_volte_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
);

#endif /* _POLICYMAN_VOLTE_H_ */

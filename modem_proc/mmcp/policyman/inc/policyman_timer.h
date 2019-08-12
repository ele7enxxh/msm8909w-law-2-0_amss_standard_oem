#ifndef _POLICYMAN_TIMER_H_
#define _POLICYMAN_TIMER_H_

/**
  @file policyman_timer.h

  @brief
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_timer.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "comdef.h"
#include "timer.h"
#include "policyman_i.h"


typedef enum
{
  POLICYMAN_TIMER_TYPE_DEFERRABLE,
  POLICYMAN_TIMER_TYPE_NONDEFERRABLE
} policyman_timer_type_t;


/* Fixed Timer ids */

#define TIMER_NET_SCAN           1000

typedef void  (*policyman_timer_expirefn_t)(policyman_timer_t *pTimer);


#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */


/*-------- policyman_timer_get_timer --------*/
/**
@brief

@param

@return

*/
policyman_timer_t * policyman_timer_get_timer(
  char const  *pName
);

/*-------- policyman_timer_create_fixed_timer --------*/
/**
@brief

@param

@return

*/
policyman_timer_t * policyman_timer_create_fixed_timer(
  char const                  *pName,
  sys_modem_as_id_e_type       subsId,
  mre_policy_t                *pPolicy,
  uint32                       id,
  timetick_type                interval,
  policyman_timer_expirefn_t   pfnExpire
);


/*-------- policyman_timer_modify_interval --------*/
/**
@brief

@param

@return

*/
void policyman_timer_modify_interval(
  policyman_timer_t *pTimer,
  timetick_type      newInterval
);


/*-------- policyman_timer_get_interval --------*/
/**
@brief

@param

@return

*/
timetick_type policyman_timer_get_interval(
  policyman_timer_t *pTimer
);


/*-------- policyman_timer_get_policy --------*/
/**
@brief

@param

@return

*/
policyman_policy_t * policyman_timer_get_policy(
  policyman_timer_t *pTimer
);


/*-------- policyman_timer_get_id --------*/
/**
@brief

@param

@return

*/
uint32 policyman_timer_get_id(
  policyman_timer_t *pTimer
);


/*-------- policyman_timer_is_stopped --------*/
/**
@brief  Check whether the given timer is stopped.

@param[in]  pTimer  Pointer to the timer to check.

@return TRUE if timer is stopped, FALSE otherwise.
*/
boolean policyman_timer_is_stopped(
  policyman_timer_t *pTimer
);


/*-------- policyman_timer_is_running --------*/
/**
@brief  Check whether the given timer is running.

@param[in]  pTimer  Pointer to the timer to check.

@return TRUE if timer is running, FALSE otherwise.
*/
boolean policyman_timer_is_running(
  policyman_timer_t *pTimer
);


/*-------- policyman_timer_is_expired --------*/
/**
@brief  Check whether the given timer is expired.

@param[in]  pTimer  Pointer to the timer to check.

@return TRUE if timer is expired, FALSE otherwise.
*/
boolean policyman_timer_is_expired(
  policyman_timer_t *pTimer
);



/*-------- policyman_timer_start --------*/
/**
@brief

@param

@return

*/
void policyman_timer_start(
  policyman_timer_t *pTimer
);


/*-------- policyman_timer_stop --------*/
/**
@brief

@param

@return

*/
void policyman_timer_stop(
  policyman_timer_t *pTimer
);

/*-------- policyman_timer_set_expirefn --------*/
/**
@brief

@param

@return

*/
void policyman_timer_set_expirefn(
  policyman_timer_t           *pTimer,
  policyman_timer_expirefn_t   pfnExpire
);


/*-------- policyman_timer_set_handled --------*/
/**
@brief  Mark an expired timer as handled.

This moves a timer in the EXPIRED state to the STOPPED state.

@param[in]  pTimer  Pointer to the timer to mark.
*/
void policyman_timer_set_handled(
  policyman_timer_t *pTimer
);


/*-------- policyman_timer_get_subs --------*/
/**
@brief

@param[in]  pTimer  Pointer to the timer.

@return subscription id for which timer is defined

*/
sys_modem_as_id_e_type policyman_timer_get_subs(
  policyman_timer_t *pTimer
);

/*-------- policyman_timer_set_subs --------*/
/**
@brief

@param[in]  pTimer  Pointer to the timer.
@param[in]  subs     subscription for which timer is running

@return None

*/
void policyman_timer_set_subs(
  policyman_timer_t       *pTimer,
  sys_modem_as_id_e_type   subs
);


/*-------- policyman_condition_timer_expired_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_condition_timer_expired_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
);


/*-------- policyman_named_timer_action_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_named_timer_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
);


/*-------- policyman_expired_timer_handled_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_expired_timer_handled_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
);


/*-------- policyman_timer_start_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_timer_start_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
);


/*-------- policyman_timer_stop_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_timer_stop_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
);

#endif /* _POLICYMAN_TIMER_H_ */

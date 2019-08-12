#ifndef _POLICYMAN_PLMN_H_
#define _POLICYMAN_PLMN_H_

/**
  @file policyman_plmn.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_plmn.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_i.h"
#include "sys.h"

#define PM_EXCLUDE_BAND_MCCS "pm:exclude_bands_mccs"


/*-------- policyman_parse_ascii_plmn --------*/
/**
@brief

@param

@return

*/
boolean policyman_parse_ascii_plmn(
  char const          *plmnStr,
  sys_plmn_id_s_type  *pPlmn
);

/*-------- policyman_plmns_are_equal --------*/
/**
@brief

@param

@return

*/
boolean policyman_plmns_are_equal(
  sys_plmn_id_s_type  *pPlmn1,
  sys_plmn_id_s_type  *pPlmn2
);



/*-------- policyman_plmn_get_mcc --------*/
/**
@brief

@param

@return

*/
uint32 policyman_plmn_get_mcc(
  sys_plmn_id_s_type  *pPlmn
);


/*-------- policyman_plmn_list_read --------*/
/**
@brief

@param

@return

*/
policyman_set_t * policyman_plmn_list_read(
  policyman_xml_element_t const  *pElem,
  char const  *pStr
);


/*-------- policyman_plmn_list_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_plmn_list_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
);


/*-------- policyman_mcc_list_read --------*/
/**
@brief

@param

@return

*/
policyman_set_t * policyman_mcc_list_read(
  policyman_xml_element_t const  *pElem,
  char const  *pStr
);


/*-------- policyman_mcc_list_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_mcc_list_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
);


/*-------- policyman_plmn_from_mcc_mnc --------*/
/**
@brief

@param

@return

*/
void policyman_plmn_from_mcc_mnc(
  sys_plmn_id_s_type  *pPlmn,
  uint32               mcc,
  uint32               mnc
);

/*-------- policyman_plmn_clear --------*/
/**
@brief

@param

@return

*/
void policyman_plmn_clear(
  sys_plmn_id_s_type  *pPlmn
);

/*-------- policyman_plmn_mcc_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_plmn_mcc_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition,
  pfn_evaluate_t                  evaluate
);

/*-------- policyman_plmn_mcc_is_volte_only --------*/
boolean policyman_plmn_mcc_is_volte_only(
  sys_mcc_type            mcc,
  sys_modem_as_id_e_type  subs
);


#define POLICYMAN_LOG_PLMN_STR "(MCC %d, MNC %d, pcs_incl %d)"

#define POLICYMAN_LOG_PLMN_VALS(plmn)                                \
    sys_mcc_type          x_mcc;                                     \
    sys_mnc_type          x_mnc;                                     \
    boolean               x_undef;                                   \
    boolean               x_pcs;                                     \
                                                                     \
    sys_plmn_get_mcc_mnc((plmn), &x_undef, &x_pcs, &x_mcc, &x_mnc)

/*
  Log a 0-argument Policyman MSG with PLMN
*/
#define POLICYMAN_MSG_PLMN_0(plmn_prefix_str, plmn)                  \
  do {                                                               \
    POLICYMAN_LOG_PLMN_VALS(plmn);                                   \
                                                                     \
    POLICYMAN_MSG_HIGH_3( plmn_prefix_str POLICYMAN_LOG_PLMN_STR,    \
                          x_mcc,                                     \
                          x_mnc,                                     \
                          x_pcs );                                   \
                                                                     \
  } while(0)

/*
  Log a 1-argument Policyman MSG with PLMN
*/
#define POLICYMAN_MSG_PLMN_1(plmn_prefix_str, arg1, plmn)            \
  do {                                                               \
    sys_mcc_type          x_mcc;                                     \
    sys_mnc_type          x_mnc;                                     \
    boolean               x_undef;                                   \
    boolean               x_pcs;                                     \
                                                                     \
    sys_plmn_get_mcc_mnc((plmn), &x_undef, &x_pcs, &x_mcc, &x_mnc ); \
                                                                     \
    POLICYMAN_MSG_HIGH_4( plmn_prefix_str POLICYMAN_LOG_PLMN_STR,    \
                          arg1,                                      \
                          x_mcc,                                     \
                          x_mnc,                                     \
                          x_pcs );                                   \
  } while(0)

/*
  Log a 2-argument Policyman MSG with PLMN
*/
#define POLICYMAN_MSG_PLMN_2(plmn_prefix_str, arg1, arg2, plmn)      \
  do {                                                               \
    POLICYMAN_LOG_PLMN_VALS(plmn);                                   \
                                                                     \
    POLICYMAN_MSG_HIGH_5( plmn_prefix_str POLICYMAN_LOG_PLMN_STR,    \
                          arg1,                                      \
                          arg2,                                      \
                          x_mcc,                                     \
                          x_mnc,                                     \
                          x_pcs );                                   \
  } while(0)

#endif /* _POLICYMAN_PLMN_H_ */

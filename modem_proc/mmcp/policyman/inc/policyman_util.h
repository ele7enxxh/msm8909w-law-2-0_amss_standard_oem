#ifndef _POLICYMAN_UTIL_H_
#define _POLICYMAN_UTIL_H_

/**
  @file policyman_util.h

  @brief Policy Manager utility functions.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_util.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "comdef.h"
#include <sys/types.h>
#include "policyman_i.h"
#include "policyman_subs.h"
#include "mre_util.h"
#include "cm.h"
#include "sd.h"



/*=============================================================================
    Memory management functions.
=============================================================================*/

#define policyman_mem_alloc       mre_mem_alloc
#define policyman_mem_free        mre_mem_free
#define POLICYMAN_MEM_FREE_IF     MRE_MEM_FREE_IF

#define policyman_simple_dtor     mre_simple_dtor



#define POLICYMAN_SEPARATOR_HYPHEN    MRE_SEPARATOR_HYPHEN
#define POLICYMAN_SEPARATOR_STAR      MRE_SEPARATOR_STAR
#define POLICYMAN_SEPARATOR_UNDER     MRE_SEPARATOR_UNDER
#define POLICYMAN_SEPARATOR_PLUS      MRE_SEPARATOR_PLUS
#define POLICYMAN_SEPARATOR_EQUAL     MRE_SEPARATOR_EQUAL
#define policyman_msg_separator_t     mre_msg_separator_t
#define policyman_util_msg_separator  mre_util_msg_separator


#define policyman_str_dup       mre_str_dup
#define policyman_get_token_ex  mre_get_token_ex
#define policyman_get_token     mre_get_token

typedef struct
{
  const char          *pStr;
  uint32               mask;
  sys_sys_mode_e_type  rat;
} rat_map;


/*-------- policyman_util_get_base --------*/
/**
@brief  Parse the 'base = "..."' attribute of an XML element.

@param

@return
  
*/
boolean
policyman_util_get_base(
  policyman_xml_element_t const *pElem,
  policyman_base_t              *pBase
  );

/*-------- policyman_util_get_subs --------*/
/**
@brief Parse the 'subs = "..."' attribute of an XML element.

@param[in] pElem    Pointer to XML element to parse
@param[in] policySubs Sub phone policy is running on
@param[out] pSubs  Pointer to asubs ID being returned

@return
  XML parsing result

*/
mre_status_t
policyman_util_get_subs(
  policyman_xml_element_t const *pElem,
  sys_modem_as_id_e_type        policySubs,
  policyman_subs_t              *pSubs
  );


/*-------- policyman_check_stack_is_valid --------*/
boolean 
policyman_check_stack_is_valid(
  sys_modem_stack_id_e_type stack
  );

/*-------- policyman_util_get_rats_from_bands --------*/
void
policyman_util_get_rats_from_bands(
  cm_band_pref_e_type       gwBands,
  sys_lte_band_mask_e_type  lteBands,
  cm_band_pref_e_type       tdsBands,
  uint32                   *pratMask
  );

/*-------- policyman_util_rat_mask_to_sd_mode_pref --------*/
sd_ss_mode_pref_e_type
policyman_util_rat_mask_to_sd_mode_pref(
  uint32 mask
  );

/*-------- policyman_util_check_valid_sys_mode --------*/
boolean policyman_util_check_valid_sys_mode(
  sys_sys_mode_e_type sys_mode
);

/*-------- policyman_util_str_to_rat --------*/
rat_map * policyman_util_str_to_rat(
  const char              *pStr
);

/*-------- policyman_create_mcc_set --------*/
policyman_set_t*  policyman_create_mcc_set(
  sys_mcc_type     *pMccArray,
  size_t            mccArrayLen
);

/*=============================================================================
  Macros to access nibbles of a byte
=============================================================================*/

#define LOW_NIBBLE(x)   ((x) & 0x0f)
#define HIGH_NIBBLE(x)  (((x) >> 4) & 0x0f)



/*=============================================================================
  Critical section functions
=============================================================================*/

#include "rex.h"

typedef rex_crit_sect_type      policyman_crit_sect_t;

#define policyman_init_crit_sect(x)    rex_init_crit_sect(x)
#define policyman_enter_crit_sect(x)   rex_enter_crit_sect(x)
#define policyman_leave_crit_sect(x)   rex_leave_crit_sect(x)
#define policyman_deinit_crit_sect(x) 

#endif /* _POLICYMAN_UTIL_H_ */

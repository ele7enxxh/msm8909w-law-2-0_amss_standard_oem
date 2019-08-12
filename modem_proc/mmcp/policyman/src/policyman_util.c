/**
  @file policyman_util.c

  @brief Policy Manager utility functions.
*/

/*
    Copyright (c) 2013,2014,2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_util.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_util.h"
#include "policyman_dbg.h"
#include "policyman_rf.h"
#include "mre_xml.h"

#include "modem_mem.h"
#include <stringl/stringl.h>



/*=============================================================================
  XML parsing helpers
=============================================================================*/


/*-------- policyman_util_get_base --------*/
boolean
policyman_util_get_base(
  policyman_xml_element_t const *pElem,
  policyman_base_t              *pBase
  )
{
  boolean     succeeded = TRUE;
  char const  *pStr;  

  pStr = mre_xml_get_attribute(pElem, "base");

  if      (pStr == NULL)
  {
    succeeded = FALSE;
  }
  else if (strcasecmp(pStr, "none") == 0)
  {
    *pBase = POLICYMAN_BASE_NONE;
  }
  else if (strcasecmp(pStr, "config") == 0)
  {
    *pBase = POLICYMAN_BASE_CONFIG;
  }    
  else if (strcasecmp(pStr, "hardware") == 0)
  {
    *pBase = POLICYMAN_BASE_HARDWARE;
  }
  else
  {
    POLICYMAN_UT_MSG_1("invalid 'base' specified: %s", pStr);
    succeeded = FALSE;
  }

  return succeeded;
}

/*-------- policyman_util_get_subs --------*/
mre_status_t policyman_util_get_subs(
  policyman_xml_element_t const *pElem,
  sys_modem_as_id_e_type        policySubs,
  policyman_subs_t              *pSubs
  )
{
  mre_status_t  status = MRE_STATUS_SUCCESS;
  char const    *pStr;
  size_t        subsId;
  policyman_subs_t executeSubs;

  pStr = mre_xml_get_attribute(pElem, "subs");

  if (pStr == NULL)
  {
    *pSubs = SUBS_THIS;
  }
  else 
  {
    executeSubs = policyman_str_to_subs(pStr);

    /* New "this" or "other" subs value */
    if (executeSubs != SUBS_NONE)
    {
      *pSubs = executeSubs;
    }
    else
    {
      subsId = atoi(pStr);
      if (subsId <= POLICYMAN_MAX_SUB)
      {
        *pSubs = ((subsId - 1) == policySubs) ? SUBS_THIS : SUBS_OTHER;
      }
      else
      {
        POLICYMAN_MSG_ERROR_3( SUBS_PREFIX "element has a subs id of %d, but there are only %d subs allowed",
                               policySubs,
                               subsId,
                               POLICYMAN_MAX_SUB );

        status = MRE_STATUS_ERR_MALFORMED_XML;
      }
    }
  }

  return status;
}

/*-------- policyman_check_stack_is_valid --------*/
boolean 
policyman_check_stack_is_valid(
  sys_modem_stack_id_e_type stack
  )
{

  return    stack == SYS_MODEM_STACK_ID_1
         || stack == SYS_MODEM_STACK_ID_2;
}

/*-------- policyman_util_get_rats_from_bands --------*/
void
policyman_util_get_rats_from_bands(
  cm_band_pref_e_type        gwBands,
  sys_lte_band_mask_e_type   lteBands,
  cm_band_pref_e_type        tdsBands,
  uint32                    *pratMask
  )
{
  if (NULL != pratMask)
  {
    *pratMask = 0;

    if (gwBands & ALL_GSM_BANDS)
    {
      *pratMask |= SYS_SYS_MODE_MASK_GSM;
    }

    if (gwBands & ALL_WCDMA_BANDS)
    {
      *pratMask |= SYS_SYS_MODE_MASK_WCDMA;
    }

    if (!SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lteBands))
    {
      *pratMask |= SYS_SYS_MODE_MASK_LTE;
    }
  
    if (tdsBands & ALL_TDS_BANDS)
    {
      *pratMask |= SYS_SYS_MODE_MASK_TDS;
    }
  }
}



/*-------- policyman_util_rat_mask_to_sd_mode_pref --------*/
sd_ss_mode_pref_e_type
policyman_util_rat_mask_to_sd_mode_pref(
  uint32 mask
  )
{
  sd_ss_mode_pref_e_type sd_mode_pref = (sd_ss_mode_pref_e_type)mask;
  
  if((mask & (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_WCDMA)) == (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_WCDMA))
  {
    sd_mode_pref |= SD_SS_MODE_PREF_GW;
  }

  if((mask & (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_LTE)) == (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_LTE)) 
  {
    sd_mode_pref |= SD_SS_MODE_PREF_GSM_LTE;
  }

  if((mask & (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_TDS)) == (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_TDS)) 
  {
    sd_mode_pref |= SD_SS_MODE_PREF_TDS_GSM;
  }

  if((mask & (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_WCDMA|SYS_SYS_MODE_MASK_LTE)) == (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_WCDMA|SYS_SYS_MODE_MASK_LTE)) 
  {
    sd_mode_pref |= SD_SS_MODE_PREF_GWL;
  }

  if((mask & (SYS_SYS_MODE_MASK_LTE|SYS_SYS_MODE_MASK_WCDMA)) == (SYS_SYS_MODE_MASK_LTE|SYS_SYS_MODE_MASK_WCDMA)) 
  {
    sd_mode_pref |= SD_SS_MODE_PREF_WCDMA_LTE;
  }

  if((mask & (SYS_SYS_MODE_MASK_TDS|SYS_SYS_MODE_MASK_WCDMA)) == (SYS_SYS_MODE_MASK_TDS|SYS_SYS_MODE_MASK_WCDMA)) 
  {
    sd_mode_pref |= SD_SS_MODE_PREF_TDS_WCDMA;
  }

  if((mask & (SYS_SYS_MODE_MASK_TDS|SYS_SYS_MODE_MASK_LTE)) == (SYS_SYS_MODE_MASK_TDS|SYS_SYS_MODE_MASK_LTE)) 
  {
    sd_mode_pref |= SD_SS_MODE_PREF_TDS_LTE;
  }

  if((mask & (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_TDS|SYS_SYS_MODE_MASK_LTE)) == (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_TDS|SYS_SYS_MODE_MASK_LTE)) 
  {
    sd_mode_pref |= SD_SS_MODE_PREF_TDS_GSM_LTE;
  }

  if((mask & (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_TDS|SYS_SYS_MODE_MASK_WCDMA)) == (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_TDS|SYS_SYS_MODE_MASK_WCDMA)) 
  {
    sd_mode_pref |= SD_SS_MODE_PREF_TDS_GSM_WCDMA;
  }

  if((mask & (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_WCDMA|SYS_SYS_MODE_MASK_LTE|SYS_SYS_MODE_MASK_TDS)) == (SYS_SYS_MODE_MASK_GSM|SYS_SYS_MODE_MASK_WCDMA|SYS_SYS_MODE_MASK_LTE|SYS_SYS_MODE_MASK_TDS))
  {
    sd_mode_pref |= SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE;
  }

  if((mask & (SYS_SYS_MODE_MASK_LTE|SYS_SYS_MODE_MASK_TDS|SYS_SYS_MODE_MASK_WCDMA)) == (SYS_SYS_MODE_MASK_LTE|SYS_SYS_MODE_MASK_TDS|SYS_SYS_MODE_MASK_WCDMA)) 
  {
    sd_mode_pref |= SD_SS_MODE_PREF_TDS_WCDMA_LTE;
  }

  POLICYMAN_MSG_HIGH_1("rat_mask to sd_mode_pref returned %d", sd_mode_pref);
  return sd_mode_pref;
}


/*-------- policyman_util_check_valid_sys_mode --------*/
boolean policyman_util_check_valid_sys_mode(
  sys_sys_mode_e_type sys_mode
  )
{
  if (   sys_mode != SYS_SYS_MODE_CDMA
      && sys_mode != SYS_SYS_MODE_HDR
      && sys_mode != SYS_SYS_MODE_GSM
      && sys_mode != SYS_SYS_MODE_WCDMA
      && sys_mode != SYS_SYS_MODE_LTE
      && sys_mode != SYS_SYS_MODE_TDS
     )
  {
    return FALSE;
  }

  return TRUE;
}

/*-------- policyman_util_str_to_rat --------*/
rat_map * policyman_util_str_to_rat(
  const char              *pStr
  )
{
  static rat_map map[] =
  {
    {"CDMA",    SYS_SYS_MODE_MASK_CDMA,  SYS_SYS_MODE_CDMA},
    {"HDR",     SYS_SYS_MODE_MASK_HDR,   SYS_SYS_MODE_HDR},
    {"GSM",     SYS_SYS_MODE_MASK_GSM,   SYS_SYS_MODE_GSM},
    {"WCDMA",   SYS_SYS_MODE_MASK_WCDMA, SYS_SYS_MODE_WCDMA},
    {"LTE",     SYS_SYS_MODE_MASK_LTE,   SYS_SYS_MODE_LTE},
    {"TDSCDMA", SYS_SYS_MODE_MASK_TDS,   SYS_SYS_MODE_TDS},
    {"C",       SYS_SYS_MODE_MASK_CDMA,  SYS_SYS_MODE_CDMA},
    {"H",       SYS_SYS_MODE_MASK_HDR,   SYS_SYS_MODE_HDR},
    {"G",       SYS_SYS_MODE_MASK_GSM,   SYS_SYS_MODE_GSM},
    {"W",       SYS_SYS_MODE_MASK_WCDMA, SYS_SYS_MODE_WCDMA},
    {"L",       SYS_SYS_MODE_MASK_LTE,   SYS_SYS_MODE_LTE},
    {"T",       SYS_SYS_MODE_MASK_TDS,   SYS_SYS_MODE_TDS}
  };
  
  size_t  i;
  rat_map * element = NULL;

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(pStr, map[i].pStr) == 0)
    {
      element = &map[i];
      break;
    }
  }
  
  return element;
}



/*-------- policyman_create_mcc_set --------*/
policyman_set_t*  policyman_create_mcc_set(
  sys_mcc_type  *pMccArray,
  size_t         mccArrayLen
)
{
  policyman_set_t   *pMccSet;
  sys_mcc_type       mcc;
  size_t             idx;
  
  pMccSet = policyman_set_new(sizeof(mcc),
                              10,
                              POLICYMAN_SET_NO_MAX_SIZE,
                              NULL,
                              NULL);

  for (idx = 0 ; idx < mccArrayLen ; ++idx)
  {
    mre_set_add(pMccSet, &pMccArray[idx]);
  }

  return pMccSet;
}
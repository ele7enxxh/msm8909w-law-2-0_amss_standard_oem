/*===========================================================================
  FILE: ds_Net_FirewallManager.cpp

  OVERVIEW: This file provides implementation of the FirewallManager class.

  DEPENDENCIES: None

  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_FirewallManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2012-05-09 rk  Memory cleanup for fi_ptr_arr in AddFirewallRule.
  2008-05-20 dm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ps_system_heap.h"
#include "ds_Errors_Def.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Net_Utils.h"
#include "ds_Net_FirewallManager.h"
#include "ds_Net_FirewallRule.h"
#include "ds_Net_Platform.h"
#include "ds_Net_Conversion.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_IPFilterSpec.h"
#include "ds_Net_FirewallRule.h"

using namespace ds::Error;
using namespace ds::Net;
using namespace ds::Net::Conversion;
using namespace NetPlatform;


/*---------------------------------------------------------------------------
  CONSTRUCTOR/DESTRUCTOR
---------------------------------------------------------------------------*/
FirewallManager::FirewallManager
(
  int32 ifaceHandle
)
: mIfaceHandle (ifaceHandle),
  refCnt(1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* FirewallManager::FirewallManager() */

FirewallManager::~FirewallManager
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mIfaceHandle = 0;

} /* FirewallManager::~FirewallManager() */

ds::ErrorType FirewallManager::EnableFirewall
(
  boolean allowPkts
)
{
  ds::ErrorType result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  result = IfaceIoctlNonNullArg (mIfaceHandle,
                                 IFACE_IOCTL_ENABLE_FIREWALL,
                                 (void *)&allowPkts);

  LOG_MSG_FUNCTION_EXIT_4 ("FirewallManager::EnableFirewall(): "
                           "Obj 0x%p, handle 0x%x, allow pkts %d result: 0x%x",
                           this, mIfaceHandle, allowPkts, result);
  return result;

} /* FirewallManager::EnableFirewall() */


ds::ErrorType FirewallManager::DisableFirewall
(
  void
)
{
  ds::ErrorType result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  result = IfaceIoctl (mIfaceHandle,
                       IFACE_IOCTL_DISABLE_FIREWALL,
                       NULL);

  LOG_MSG_FUNCTION_EXIT_3 ("FirewallManager::DisableFirewall(): "
                           "Obj 0x%p, handle 0x%x result: 0x%x",
                           this, mIfaceHandle, result);
  return result;

} /* FirewallManager::DisableFirewall() */

ds::ErrorType FirewallManager::AddFirewallRule
(
  IIPFilterPriv*                pIFilterSpec,
  IFirewallRule**               ppIFirewallRule
)
{
  ds::ErrorType                 result = AEE_SUCCESS;
  PSIfaceIPFilterAddParamType   filterAddParam;
  int32                         fltrHandle = 0 ;
  FirewallRule*                 pFirewallRule = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_4 ("FirewallManager::AddFirewallRule(): "
                            "Obj 0x%p, handle 0x%x pIFilterSpec 0x%x ppIFirewallRule 0x%x",
                            this, mIfaceHandle, pIFilterSpec, ppIFirewallRule);

  memset (&filterAddParam, 0, sizeof(filterAddParam));

  /*-------------------------------------------------------------------------
    Validation.
  -------------------------------------------------------------------------*/
  if (NULL == pIFilterSpec || NULL == ppIFirewallRule)
  {
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Allocate memory to hold array to ip_filter_type
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(
    filterAddParam.fi_ptr_arr,
    sizeof (ip_filter_type),
    void *);

  if  (NULL == filterAddParam.fi_ptr_arr)
  {
    result = AEE_ENOMEMORY;
    goto bail;
  }

  /*-----------------------------------------------------------------------
    Allocate memory for the firewall rule object. This is done prior to
    installing filters on PS iface layer.
  -----------------------------------------------------------------------*/
  pFirewallRule = new FirewallRule();
  if (NULL == pFirewallRule)
  {
    result = AEE_ENOMEMORY;
    goto bail;
  }

  filterAddParam.enable             = TRUE;
  filterAddParam.fi_result          = mIfaceHandle;
  filterAddParam.num_filters        = 1;
  filterAddParam.filter_type        = IPFLTR_DEFAULT_TYPE;
  filterAddParam.is_validated       = FALSE;
  filterAddParam.fltr_priority      = PS_IFACE_IPFLTR_PRIORITY_DEFAULT;
  filterAddParam.fltr_compare_f_ptr = NULL;
  filterAddParam.subset_id          = PS_IFACE_IPFLTR_SUBSET_ID_DEFAULT;

  (void) DS2PSIPFilterSpec
  (
    pIFilterSpec,
    (((ip_filter_type *)filterAddParam.fi_ptr_arr))
  );

  result = PSIfaceIPFilterAdd (mIfaceHandle,
                               ANY_DEFAULT_GROUP,
                               IP_FLTR_CLIENT_FIREWALL_INPUT,
                               &filterAddParam,
                               &fltrHandle);
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  if (PS_IFACE_IPFLTR_INVALID_HANDLE == fltrHandle)
  {
    result = QDS_EINVAL;
    goto bail;
  }

  /*-----------------------------------------------------------------------
    Set the handles in Firewall rule object and return.
  -----------------------------------------------------------------------*/
  pFirewallRule->SetIfaceHandle(mIfaceHandle);
  pFirewallRule->SetFirewallHandle(fltrHandle);
  *ppIFirewallRule = static_cast <IFirewallRule *> (pFirewallRule);

  /* Free fi_ptr_arr allocated in AddFirewallRule. */
  if (NULL != filterAddParam.fi_ptr_arr)
  {
    PS_SYSTEM_HEAP_MEM_FREE (filterAddParam.fi_ptr_arr);
  }						 
  return AEE_SUCCESS;
  /* Fall through into bail for cleanup */

bail:
  LOG_MSG_ERROR_1 ("FirewallManager::AddFirewallRule(): "
                   "Err 0x%x adding firewall rule", result);
  if (NULL != filterAddParam.fi_ptr_arr)
  {
    PS_SYSTEM_HEAP_MEM_FREE (filterAddParam.fi_ptr_arr);
  }

  DSNET_RELEASEIF (pFirewallRule);
  *ppIFirewallRule = NULL;

  return result;

} /* FirewallManager::AddFirewallRule() */


ds::ErrorType FirewallManager::GetFirewallTable
(
  IFirewallRule**               ppIFirewallRules,
  int                           rulesLen,
  int*                          pRulesLenReq
)
{
  int32                         result;
  int                           index;
  GetFirewallTableType          psFirewallTable;
  FirewallRule**                ppFirewallRuleObjs = NULL;
  uint8                         numFirewallRules = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("FirewallManager::GetFirewallTable(): "
                            "Obj 0x%p, handle 0x%x", this, mIfaceHandle);

  memset (&psFirewallTable, 0, sizeof (GetFirewallTableType));

  /*-------------------------------------------------------------------------
    Call GET_FIREWALL_TABLE_IOCTL first to figure out number of rules
    currently available.
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (mIfaceHandle,
                       IFACE_IOCTL_GET_FIREWALL_TABLE,
                       static_cast <void *> (&psFirewallTable));
  if (AEE_SUCCESS != result)
  {
    memset (&psFirewallTable, 0, sizeof(GetFirewallTableType));
    goto bail;
  }

  /*-------------------------------------------------------------------------
    pRulesLenReq should be populated to available number of filters.
  -------------------------------------------------------------------------*/
  if(NULL != pRulesLenReq)
  {
    *pRulesLenReq = psFirewallTable.avail_num_fltrs;
  }

  numFirewallRules = MIN(rulesLen, psFirewallTable.avail_num_fltrs);
  LOG_MSG_INFO2_2 ("FirewallManager::GetFirewallTable(): "
                   "Available number of fltrs %d, app requesting %d",
                   psFirewallTable.avail_num_fltrs, rulesLen);

  if (0 == numFirewallRules)
  {
    memset (&psFirewallTable, 0, sizeof(GetFirewallTableType));
    result = AEE_SUCCESS;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Allocate all dynamic memories required.
  -------------------------------------------------------------------------*/
  memset (&psFirewallTable, 0, sizeof(GetFirewallTableType));
  psFirewallTable.num_fltrs       = numFirewallRules;
  PS_SYSTEM_HEAP_MEM_ALLOC(
    psFirewallTable.fltr_spec_arr,
    sizeof(ip_filter_type) * numFirewallRules,
    ip_filter_type*);

  PS_SYSTEM_HEAP_MEM_ALLOC(
    psFirewallTable.handle_arr,
    sizeof(uint32) * numFirewallRules,
    uint32*);

  /*-------------------------------------------------------------------------
    Allocate memory for Firewall rule objects array. Here we are allocating
    memory for only the array of pointers and not the Firewall rule objects
    themselves.
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(
    ppFirewallRuleObjs,
    sizeof (FirewallRule *) * numFirewallRules,
    FirewallRule**);

  /*-------------------------------------------------------------------------
    If any memory allocation failed, goto cleanup.
  -------------------------------------------------------------------------*/
  if (NULL == ppFirewallRuleObjs ||
      NULL == psFirewallTable.handle_arr ||
      NULL == psFirewallTable.fltr_spec_arr)
  {
    result = AEE_ENOMEMORY;
    goto bail;
  }

  memset (ppFirewallRuleObjs, 0, numFirewallRules * sizeof(FirewallRule *));

  /*-------------------------------------------------------------------------
    Create corresponding firewall objects as well.  If any memory allocation
    fails, goto cleanup.
  -------------------------------------------------------------------------*/
  for (index = 0; index < numFirewallRules; index++)
  {
    ppFirewallRuleObjs[index] = new FirewallRule();
    if (NULL == ppFirewallRuleObjs[index])
    {
      result = AEE_ENOMEMORY;
      goto bail;
    }
  } /* for */

  /*-----------------------------------------------------------------------
    Call the iface IOCTL to get all firewall rules.
  -----------------------------------------------------------------------*/
  result = IfaceIoctl (mIfaceHandle,
                       IFACE_IOCTL_GET_FIREWALL_TABLE,
                       static_cast <void *> (&psFirewallTable));
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }


  /*-------------------------------------------------------------------------
    Add the rule object to firewall manager list, populate output params.
  -------------------------------------------------------------------------*/
  for (index = 0; index < numFirewallRules; index++)
  {

    ppFirewallRuleObjs[index]->SetFirewallHandle
      (
        psFirewallTable.handle_arr[index]
      );

    ppFirewallRuleObjs[index]->SetIfaceHandle
      (
        mIfaceHandle
      );

    ppIFirewallRules[index] = static_cast <IFirewallRule *>
                              (ppFirewallRuleObjs[index]);

  }

  /* Fall-through into bail */

bail:
  /*-------------------------------------------------------------------------
    Cleanup
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1 ("FirewallManager::GetFirewallTable(): "
                     "Err 0x%x getting firewall rule table", result);

    if(NULL != ppFirewallRuleObjs)
    {
      for (index = 0; index < numFirewallRules; index++)
      {
        DSNET_RELEASEIF (ppFirewallRuleObjs[index]);
      }
    }
  }

  if (NULL != psFirewallTable.fltr_spec_arr)
  {
    PS_SYSTEM_HEAP_MEM_FREE(psFirewallTable.fltr_spec_arr);
  }

  if (NULL != psFirewallTable.handle_arr)
  {
    PS_SYSTEM_HEAP_MEM_FREE(psFirewallTable.handle_arr);
  }

  if (NULL != ppFirewallRuleObjs)
  {
    PS_SYSTEM_HEAP_MEM_FREE(ppFirewallRuleObjs);
  }

  return result;

} /* FirewallManager::GetFirewallTable() */





/*==========================================================================*/
/*!
  @file
  ds_Net_FirewallRule.cpp

  @brief
  This file provides implementation for the DS::Net::FirewallRule class.

  @see  ds_Net_Firewall.h

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_FirewallRule.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-20 dm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Errors_Def.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Net_FirewallRule.h"
#include "ds_Net_Platform.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_IPFilterSpec.h"
#include "ds_Net_Conversion.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace ds::Net::Conversion;
using namespace NetPlatform;


FirewallRule::FirewallRule
(
  void
)
: mIfaceHandle (0),
  mFirewallHandle (0),
  refCnt(1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* FirewallRule() */

FirewallRule::~FirewallRule
(
  void
)
throw()
{
  NetPlatform::DeleteFirewallRuleType    delInfo;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Call the iface IOCTL to delete firewall rule.
  -----------------------------------------------------------------------*/
  memset (&delInfo, 0, sizeof (NetPlatform::DeleteFirewallRuleType));
  delInfo.handle = mFirewallHandle;

  (void) NetPlatform::IfaceIoctl
         (
           mIfaceHandle,
           NetPlatform::IFACE_IOCTL_DELETE_FIREWALL_RULE,
           static_cast <void *> (&delInfo)
         );

} /* FirewallRule::~FirewallRule() */

/*---------------------------------------------------------------------------
  Inherited functions from IFirewallRule.
---------------------------------------------------------------------------*/
ds::ErrorType FirewallRule::GetFirewallRule
(
  IIPFilterPriv** ppIFilterSpec
)
{
  int32                                     result;
  NetPlatform::GetFirewallRuleType          firewallRule;
  IPFilterSpec                             *pFilterSpec;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3  ("FirewallRule::GetFirewallRule(): "
                             "Get firewall rule, Obj 0x%p, handle 0x%x ppIFilterSpec 0x%x",
                             this, mFirewallHandle, ppIFilterSpec);

  /*-------------------------------------------------------------------------
    Validation
  -------------------------------------------------------------------------*/
  if (NULL == ppIFilterSpec)
  {
    return QDS_EFAULT;
  }

  *ppIFilterSpec = NULL;

  /*-------------------------------------------------------------------------
    Perform iface IOCTL to get firewall rule.
  -------------------------------------------------------------------------*/
  memset (&firewallRule, 0, sizeof(firewallRule));
  firewallRule.handle = mFirewallHandle;

  result = IfaceIoctl (mIfaceHandle,
                      IFACE_IOCTL_GET_FIREWALL_RULE,
                      (void *)&firewallRule);
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Convert to out args
  -------------------------------------------------------------------------*/
  pFilterSpec = new IPFilterSpec();
  if (NULL == pFilterSpec)
  {
    result = AEE_ENOMEMORY;
    goto bail;
  }

  PS2DSIPFilterSpec(&firewallRule.fltr_spec, pFilterSpec);

  *ppIFilterSpec = static_cast <IIPFilterPriv*> (pFilterSpec);

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_2 ("FirewallRule::GetFirewallRule(): "
                   "Err 0x%x getting firewall rule for handle 0x%x",
                   result, mFirewallHandle);
  return result;
}

boolean FirewallRule::Process
(
  void *pUserData
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) pUserData;
  DATA_ERR_FATAL("FirewallRule::Process(): Process is unsupported");
  return FALSE;

} /* FirewallRule::Process() */

void FirewallRule::SetIfaceHandle
(
  int32 ifaceHandle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mIfaceHandle = ifaceHandle;

} /* FirewallRule::SetIfaceHandle() */


void FirewallRule::SetFirewallHandle
(
  uint32 firewallHandle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mFirewallHandle = firewallHandle;

}


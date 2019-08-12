#ifndef DS_NET_FIREWALLRULE_H
#define DS_NET_FIREWALLRULE_H

/*=========================================================================*/
/*!
  @file
  ds_Net_FirewallRule.h

  @brief
  This file defines the class that implements the IFirewallRule interface.

  @details
  The Firewall class (DS::Net::FirewallRule) implements the following
  interfaces:
  1. IQI

  IFirewall instance can be obtained by calling createInstance() method.

  IFirewall object represents the firewall rules set up by the admin.
  The admin can perform firewall related IOCTLs
  like:
  1. GetFirewall(): getting a firewall rule

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_FirewallRule.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-20 dm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_mem.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_INode.h"
#include "ds_Net_Utils.h"
#include "ds_Net_IFirewallRule.h"
#include "ds_Errors_Def.h"


/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
namespace Net
{

/*lint -esym(1510, IFirewallRule) */
/*lint -esym(1510, IQI) */
class FirewallRule: public IFirewallRule
{
private:
  int32       mIfaceHandle;
  uint32      mFirewallHandle;

  /*!
  @brief
  Dummy destructor. Only used to free memory.

  @details
  For objects that support weak references, destruction of the object and
  freeing up of memory can happen at different times. Hence Destructor()
  is the one which performs actual destruction of the object, where as
  ~Object() would perform memory cleanup.
  @params     None.
  @return     None.
  */
  virtual ~FirewallRule
  (
    void
  )
  throw();

public:
  /*!
  @brief
  Constructor.

  @param      None.
  @return     None.
  @see        IQI::Release()
  */
  FirewallRule
  (
    void
  );

  /*-------------------------------------------------------------------------
    Methods from DS::Net::IFirewallRule interface.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  Retrieve the firewall spec for the rule.

  @details
  @param[out] pFilterSpec - Filter Spec.

  @return SUCCESS    - if request is received successfully
  @return DSS_EFAULT - Parameters are invalid.
  */
  virtual ds::ErrorType CDECL GetFirewallRule
  (
    IIPFilterPriv** pFilterSpec
  );


  /*-------------------------------------------------------------------------
    Method from INode interface - NO-OP
  -------------------------------------------------------------------------*/
  virtual boolean Process
  (
    void *pUserData
  );

  /*-------------------------------------------------------------------------
    Methods to set the firewall handle and iface handle.
  -------------------------------------------------------------------------*/
  virtual void SetIfaceHandle
  (
    int32 ifaceHandle
  );

  virtual void SetFirewallHandle
  (
    uint32 firewallHandle
  );

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_FIREWALL_RULE)

  /*-------------------------------------------------------------------------
    Defintions of IQI Methods
  -------------------------------------------------------------------------*/
  DSIQI_IMPL_DEFAULTS(IFirewallRule)


}; /* class FirewallRule */

} /* namespace Net */
} /* namespace DS */

#endif /* DS_NET_QOS_FIREWALLRULE_H */





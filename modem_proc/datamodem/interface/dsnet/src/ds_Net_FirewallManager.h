#ifndef DS_NET_FIREWALL_MANAGER_H
#define DS_NET_FIREWALL_MANAGER_H
/*==========================================================================*/
/*!
  @file
  ds_Net_FirewallManager.h

  @brief
  This file defines the class that implements the IFirewallManager
  interface.

  @details
  The FirewallManager class (DS::Net::FirewallManager) implements the following
  interfaces:
  IQI
  IFirewalltManager


  @todo
  Write details

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_FirewallManager.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-20 dm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Errors_Def.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_Factory.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_IFirewallManager.h"

/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Net
{
/*lint -esym(1510, IFirewallManager) */
/*lint -esym(1510, IQI) */
class FirewallManager : public IFirewallManager
{

private:
  int32                 mIfaceHandle;

  /*!
  @brief
  Private destructor.

  @details
  The destructor of this object is private. Use Release() method of the
  IQI interface to free the object.

  @param      None.
  @return     None.
  @see        IQI::Release()
  */
  virtual ~FirewallManager
  (
    void
  )
  throw();


public:
  FirewallManager(int32 ifaceHandle);

  /*-------------------------------------------------------------------------
    Inherited functions from IFirewallManager.
  -------------------------------------------------------------------------*/
  /*!
  @function
  EnableFirewall()

  @brief
  This function enables the firewall on the interface.

  @details
  TODO

  @param[in] pIsAllowed - In parameter to allow/disallow firewall rules.

  @return     SUCCESS - on success
  @return     DSS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL EnableFirewall
  (
    boolean    pIsAllowed
  );

  /*!
  @function
  DisableFirewall()

  @brief
  This function disables the firewall on the interface.

  @details
  TODO

  @return     SUCCESS - on success
  @return     DSS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL DisableFirewall
  (
    void
  );

  /*!
  @function
  AddFirewallRule()

  @brief
  This function adds the firewall rule on the interface.

  @details
  TODO

  @param[in] pFltrSpec - In parameter to add firewall rules.
  @param[out] pFirewallSpec - Firewall Rule interface

  @return     SUCCESS - on success
  @return     DSS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL AddFirewallRule
  (
    IIPFilterPriv* pFltrSpec,
    IFirewallRule** pFirewallSpec
  );

  /*!
  @function
  GetFirewallTable()

  @brief
  This function gets the firewall table on the interface.

  @details
  TODO

  @param[out] pFltrSpec - Firewall rule.
  @param[out] rulesLen - No of Firewall rules
  @param[out] pRulesLenReq - Required no of Firewall rules

  @return     SUCCESS - on success
  @return     DSS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL GetFirewallTable
  (
    IFirewallRule** pFilterSpec,
    int             rulesLen,
    int*            pRulesLenReq
  );

  /*-------------------------------------------------------------------------
    IQI interface Methods
  -------------------------------------------------------------------------*/
  DSIQI_IMPL_DEFAULTS(IFirewallManager)

  /*-------------------------------------------------------------------------
    Overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_FIREWALL_MANAGER)

};/* class firewallManager */
} /* namespace Net */
} /* namespace DS */

#endif /* DS_NET_FIREWALL_MANAGER_H */



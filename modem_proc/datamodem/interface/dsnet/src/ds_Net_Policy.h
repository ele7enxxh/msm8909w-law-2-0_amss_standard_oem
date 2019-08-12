#ifndef DS_NET_POLICY_H
#define DS_NET_POLICY_H

/*===========================================================================
  @file Policy.h

  This file defines the class that implements the IPolicy interface.

  TODO: Detailed explaination about the class here.
  The Policy class implements the following interfaces:
  IPolicy
  IPolicyPriv

  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_Policy.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-03-31 ss  Added appType field needed for arbitration among apps.
  2008-03-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_IPolicy.h"
#include "ds_Net_IPolicyPriv.h"
#include "ds_Net_Platform.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_Def.h"

/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/
/*Suppress lint error because of namespace collisions */
/*lint -save -e578 */
namespace ds
{
namespace Net
{
/*lint -esym(1510, IPolicyPriv) */
/*lint -esym(1510, IPolicy) */
/*lint -esym(1510, IQI) */
class Policy : public IPolicyPriv
{

private:
  ICritSect*                 mpICritSect;
  IfaceNameType              ifaceName;
  IfaceGroupType             ifaceGroup;
  unsigned short int         ipFamily;
  boolean                    isRouteable;
  PolicyFlagType             policyFlag;
  unsigned int               ifaceId;
  int                        umtsProfileNum;
  int                        cdmaProfileNum;
  char *                     umtsAPNName;
  int                        umtsAPNNameLen;
  int                        appType;
  unsigned int               subsId;
  unsigned int               apnType;

public:

  Policy(); /* Similar to init_netpolicy() */
  virtual ~Policy() throw();

  /*---------------------------------------------------------------------------
    Inherited functions.
  ---------------------------------------------------------------------------*/
  /*!
  @brief
  Get the policy flag from network policy.

  @details
  The application can use this function to get the policy flag which has
  been set in the network policy. The policy flag could be SPECIFIED,
  UP_ONLY and UP_SPEC.

  @param[out] argPolicyFlag - Policy flag is returned through this.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL GetPolicyFlag
  (
    PolicyFlagType* PolicyFlag
  );

  /*!
  @brief
  Set the policy flag from network policy.

  @details
  The application can use this function to set the policy flag in the network
  policy. The policy flag could be set to SPECIFIED, UP_ONLY and UP_SPEC.

  @param[in]  argPolicyFlag - Policy flag value.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL SetPolicyFlag
  (
    PolicyFlagType PolicyFlag
  );

  /*!
  @brief
  Get the UMTS APN name.

  @details
  Get the UMTS APN Name.

  @param[out]  pAPNName  - APN name to be returned.
  @param[in]   APNNameLen - APN length allocated by user.
  @param[out]  pAPNNameLenReq - lenReq for APN name.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL GetUMTSAPNName
  (
    char *  pAPNName,
    int     APNNameLen,
    int*    pAPNNameLenReq
  );

  /*!
  @brief
  Set the UMTS APN name.

  @details
  Set the UMTS APN Name.

  @param[in]   pAPNName  - APN name to be set.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL SetUMTSAPNName
  (
    const char *    pAPNName
  );

  virtual ds::ErrorType CDECL GetIfaceName
  (
    ::ds::Net::IfaceNameType* NetType
  );

  virtual ds::ErrorType CDECL SetIfaceName
  (
    ::ds::Net::IfaceNameType NetType
  );

  virtual ds::ErrorType CDECL GetIfaceGroup
  (
    ds::Net::IfaceGroupType *GroupType
  );

  virtual ds::ErrorType CDECL SetIfaceGroup
  (
    ds::Net::IfaceGroupType GroupType
  );

  virtual ds::ErrorType CDECL GetAddressFamily
  (
    ::ds::AddrFamilyType* AddressFamily
  );

  virtual ds::ErrorType CDECL SetAddressFamily
  (
    ::ds::AddrFamilyType AddressFamily
  );

  virtual ds::ErrorType CDECL GetRouteable
  (
    boolean* Routeable
  );

  virtual ds::ErrorType CDECL SetRouteable
  (
    boolean Routeable
  );

  virtual ds::ErrorType CDECL GetAppType
  (
    int* AppType
  );

  virtual ds::ErrorType CDECL SetAppType
  (
    int AppType
  );

  virtual ds::ErrorType CDECL GetRoutable (boolean* Routable)
  {
    return GetRouteable(Routable);
  }

  virtual ds::ErrorType CDECL SetRoutable (boolean Routable)
  {
    return SetRouteable(Routable);
  }

  virtual ds::ErrorType CDECL GetCDMAProfileNumber (/*rout*/ int* CDMAProfileNumber);
  virtual ds::ErrorType CDECL SetCDMAProfileNumber (/*in*/ int CDMAProfileNumber);
  virtual ds::ErrorType CDECL GetUMTSProfileNumber (/*rout*/ int* UMTSProfileNumber);
  virtual ds::ErrorType CDECL SetUMTSProfileNumber (/*in*/ int UMTSProfileNumber);

  virtual ds::ErrorType CDECL GetProfileList (/*rout*/ ::ds::Net::PolicyProfileType* ProfileList, /*in*/ int ProfileListLen, /*rout*/ int* ProfileListLenReq){return 0;}
  virtual ds::ErrorType CDECL SetProfileList (/*in*/ const ::ds::Net::PolicyProfileType* ProfileList, /*in*/ int ProfileListLen) {return 0;}

  /*-------------------------------------------------------------------------
    Defintions of IQI Methods
    TODO: IMPL_DEFAULTS should only implement ADDREF and RELEASE.
    QueryInterface should be implemented by each object.
  -------------------------------------------------------------------------*/
  DSIQI_DECL_LOCALS()
  DSIQI_ADDREF()
  DSIQI_RELEASE()

  virtual ds::ErrorType CDECL QueryInterface (AEEIID iid, void **ppo);

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_POLICY)

  /*-------------------------------------------------------------------------
    Methods from IPolicyPriv interface.
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL GetIfaceId (/*rout*/ ::ds::Net::IfaceIdType* IfaceId);
  virtual ds::ErrorType CDECL SetIfaceId (/*in*/ ::ds::Net::IfaceIdType IfaceId);

  virtual ds::ErrorType CDECL GetSubsId (/*rout*/ ::ds::Net::SubsInfoType* subs_id);
  virtual ds::ErrorType CDECL SetSubsId (/*in*/ ::ds::Net::SubsInfoType subs_id);

  virtual ds::ErrorType CDECL GetApnType (/*rout*/ ::ds::Net::ApnInfoType* apn_type);
  virtual ds::ErrorType CDECL SetApnType (/*in*/ ::ds::Net::ApnInfoType apn_type);

};/* class Policy */
} /* namespace Net */
} /* namespace ds */

/*lint -restore */

#endif /* DS_NET_POLICY_H */


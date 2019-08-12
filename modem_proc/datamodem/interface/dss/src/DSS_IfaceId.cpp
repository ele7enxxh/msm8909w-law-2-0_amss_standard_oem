/*======================================================

FILE:  DSS_IfaceId.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of DSSIfaceId class

=====================================================

Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_IfaceId.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-12-17 yl  Class Created.

===========================================================================*/

//===================================================================
//   Includes and Public Data Declarations
//===================================================================

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------
#include "datamodem_variation.h"
#include "DSS_IfaceId.h"
#include "ds_Net_INetworkPriv.h"

//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Type Declarations (typedef, struct, enum, etc.)
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Constant Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------

//===================================================================
//              Macro Definitions
//===================================================================

//===================================================================
//            DSSIfaceId Functions Definitions
//===================================================================

//===================================================================
//  FUNCTION:   BuildIfaceId
//
//  DESCRIPTION:
//  Get IfaceId from Network object (dsnet) and build IfaceId
//  to be used in DSS or by the application.
//===================================================================
AEEResult DSSIfaceId::BuildIfaceId(
  ds::Net::INetwork* pIDSNetwork,
  IFaceIdType ifaceIdType,
  sint15 appId,
  dss_iface_id_type* pIfaceId)
{
  ds::Net::IfaceIdType ifaceIdTemp = 0;
  uint8 ifaceIndex = 0;
  ds::Net::INetworkPriv* pIDSNetworkPriv = NULL;
  AEEResult res = AEE_SUCCESS;

  if (NULL == pIfaceId)
  {
    LOG_MSG_ERROR_0("DSSIfaceId::BuildIfaceId: "
                    "pIfaceId is NULL");
    return QDS_EINVAL;
  }

  if (NULL == pIDSNetwork)
  {
    LOG_MSG_ERROR_0("DSSIfaceId::BuildIfaceId: "
                    "pIDSNetwork is NULL");
    return QDS_EINVAL;
  }

  switch(ifaceIdType)
  {
  case IFACE_ID_TYPE_CURRENT:
    IDS_ERR_RET(pIDSNetwork->GetIfaceIdNoLookup(&ifaceIdTemp));
    break;

  case IFACE_ID_TYPE_STALE:
    IDS_ERR_RET(pIDSNetwork->QueryInterface(ds::Net::AEEIID_INetworkPriv, (void**)&pIDSNetworkPriv));
    res = pIDSNetworkPriv->GetStaleIfaceId(&ifaceIdTemp);
    DSSCommon::ReleaseIf((IQI**)&pIDSNetworkPriv);
    IDS_ERR_RET(res);
    break;
  
  default:
    return QDS_EINVAL;
  }

  // we need special treatment in case of STA iface , in this case the original ifaceIdTemp from lower layers is returned
  // rather than modified by DSS . This is needed because some STA iface tests work directly with ps layers passing them
  // the iface id and ps layer expect it to be in the original unmodified format
#ifdef FEATURE_STA_PS_IFACE
  ds::Net:IfaceNameType ifaceName;
  IDS_ERR_RET(pIDSNetwork->GetIfaceName(&ifaceName));
  if (IfaceName::IFACE_STA == ifaceName)
  {
    *pIfaceId = static_cast<dss_iface_id_type>(ifaceIdTemp);
    return AEE_SUCCESS;
  }
#endif

  // if iface id is invalid return invalid.
  // Do not include app id within the iface id when the iface id is invalid
  if (DSS_IFACE_INVALID_ID == ifaceIdTemp)
  {
    *pIfaceId = DSS_IFACE_INVALID_ID;
  }
  else
  {
    ifaceIndex = static_cast<uint8>(GetIfaceIndex(ifaceIdTemp));

    if (-1 != appId)
    {
      // build iface with app id bits set
      *pIfaceId = BuildIfaceIdWithAppId(ifaceIndex, appId);
    }
    else
    {
      // build iface with policy bit set
      *pIfaceId = BuildIfaceIdWithPolicyBit(ifaceIndex);
    }
  }

  return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   BuildIfaceId
//
//  DESCRIPTION:
//  Get IfaceId from Network Policy object (dsnet) and build IfaceId
//  to be used in DSS or by the application.
//===================================================================
AEEResult DSSIfaceId::BuildIfaceId(
  ds::Net::IPolicyPriv* pIDSNetworkPolicyPriv,
  sint15 appId,
  dss_iface_id_type* pIfaceId)
{
  ds::Net::IfaceIdType ifaceIdTemp = 0;
  uint8 ifaceIndex = 0;

  if (NULL == pIfaceId)
  {
    LOG_MSG_ERROR_0("DSSIfaceId::BuildIfaceId: "
                    "pIfaceId is NULL");
    return QDS_EBADF;
  }

  if (NULL == pIDSNetworkPolicyPriv)
  {
    LOG_MSG_ERROR_0("DSSIfaceId::BuildIfaceId: "
                    "pIDSNetworkPolicyPriv is NULL");
    return QDS_EINVAL;
  }

  IDS_ERR_RET(pIDSNetworkPolicyPriv->GetIfaceId(&ifaceIdTemp));

  // we need special treatment in case of STA iface , in this case the original ifaceIdTemp from lower layers is returned
  // rather than modified by DSS . This is needed because some STA iface tests work directly with ps layers passing them
  // the iface id and ps layer expect it to be in the original unmodified format
#ifdef FEATURE_STA_PS_IFACE
  ds::Net::IfaceNameType ifaceName;
  IDS_ERR_RET(pIDSNetworkPolicyPriv->GetIfaceName(&ifaceName));
  if (IfaceName::IFACE_STA == ifaceName)
  {
    *pIfaceId = static_cast<dss_iface_id_type>(ifaceIdTemp);
    return AEE_SUCCESS;
  }
#endif

  ifaceIndex = static_cast<uint8>(GetIfaceIndex(ifaceIdTemp));

  // if iface id is invalid return invalid.
  // Do not include app id within the iface id when the iface id is invalid
  if (DSS_IFACE_INVALID_ID == ifaceIdTemp)
  {
    *pIfaceId = DSS_IFACE_INVALID_ID;
  }
  else
  {
    if (-1 != appId)
    {
      // build iface with app id bits set
      *pIfaceId = BuildIfaceIdWithAppId(ifaceIndex, appId);
    }
    else
    {
      // build iface with policy bit set
      *pIfaceId = BuildIfaceIdWithPolicyBit(ifaceIndex);
    }
  }

  return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   BuildIfaceIdWithAppId
//
//  DESCRIPTION:
//  Build IfaceId with application ID embedded in it. 
//===================================================================
uint32 DSSIfaceId::BuildIfaceIdWithAppId(uint8 ifaceIndex, sint15 appId)
{
  uint32 extendedAppId = 0;
  uint32 ifaceId = 0;

  // Update the iface id with the iface index, the policy flag will turned off in the returned ifaceId.
  ifaceId = static_cast<uint32>(ifaceIndex); // Put the iface index into the ifaceId
  ifaceId = (ifaceId << 24); // perform shift left on the ifaceId so the iface index will be stored in the MSB byte.

  extendedAppId = appId; // put the appId into uint32 buffer.
  extendedAppId = extendedAppId << 8; // perform shift left on the appId so it will be stored in the second and the third bytes.

  ifaceId |= extendedAppId; // perform bitwise or in order to store the nethandle into the ifaceId buffer.

  return ifaceId;
}

//===================================================================
//  FUNCTION:   BuildIfaceIdWithPolicyBit
//
//  DESCRIPTION:
//  Build IfaceId without application ID embedded in it. 
//===================================================================
uint32 DSSIfaceId::BuildIfaceIdWithPolicyBit(uint8 ifaceIndex)
{
  uint32 ifaceId = 0;

  ifaceId = static_cast<uint32>(ifaceIndex); // Put the iface index into the ifaceId
  ifaceId = (ifaceId << 24); // perform shift left on the ifaceId so the iface index will be stored in the MSB byte.
  ifaceId = TurnPolicyFlagOn(ifaceId); // turn on the policy flag that indecated that this iface id was returned calling get_iface_id_by_policy().

  return ifaceId;
}

//===================================================================
//  FUNCTION:   CompareIfaceIds
//
//  DESCRIPTION:
//  This function is called from different places in code with different
//  ifaceId formats.
//  If one of ifaceId has its policy bit on,
//  (meaning that it was created either with dss_get_iface_id_by_policy
//  or with IDSNetworkPriv), just compare the iface index,
//  otherwise compare everything
//===================================================================
bool DSSIfaceId::CompareIfaceIds(
  dss_iface_id_type ifaceId1,
  dss_iface_id_type ifaceId2)
{
  if ((DSS_IFACE_INVALID_ID == ifaceId1) ||
    (DSS_IFACE_INVALID_ID == ifaceId2))
  {
    return false;
  }
  if (PolicyFlagIsOn(ifaceId1) ||
    PolicyFlagIsOn(ifaceId2))
  {
    return (GetIfaceIndex(ifaceId1) == GetIfaceIndex(ifaceId2));
  }
  else
  {
    return (ifaceId1 == ifaceId2);
  }
}

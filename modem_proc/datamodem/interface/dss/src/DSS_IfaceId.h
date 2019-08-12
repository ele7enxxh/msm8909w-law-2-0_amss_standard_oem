#ifndef __DSS_IFACEID_H__
#define __DSS_IFACEID_H__

/*======================================================

FILE:  DSS_IfaceId.h

SERVICES:
IfaceId methods.

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_IfaceId.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-12-07 yl  Class Created.

===========================================================================*/

//===================================================================
//   Includes and Public Data Declarations
//===================================================================

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------
#include "comdef.h"
#include "customer.h"

#include "ds_Net_INetwork.h"

#include "DSS_Common.h"

using namespace ds::Error;

//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Type Declarations (typedef, struct, enum, etc.)
//-------------------------------------------------------------------
typedef uint32 FlowID;
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
//              Class Definitions
//===================================================================
#define IFACE_ID_POLICY_FLAG (0x00800000)
#define IFACE_ID_ERROR (0xFFFFFFFF)

//===================================================================
//  CLASS:      DSSIfaceId
//
//  DESCRIPTION:
//  This is a static class, i.e. all of its methods and data members
//  are static.
//  This class handles all ifaceId associated issues.
//
//  HOW TO USE:
//  N/A
//
//===================================================================

class DSSIfaceId
{
//-------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//  Public methods
//-------------------------------------------------------------------
public:
  enum IFaceIdType
  {
    IFACE_ID_TYPE_CURRENT,
    IFACE_ID_TYPE_STALE
  };

  static AEEResult BuildIfaceId(
    ds::Net::INetwork* pIDSNetwork,
    IFaceIdType ifaceIdType,
    sint15 appId,
    dss_iface_id_type* pIfaceId);

  static AEEResult BuildIfaceId(
    ds::Net::IPolicyPriv* pIDSNetworkPolicyPriv,
    sint15 appId,
    dss_iface_id_type* ifaceId);

  static uint32 BuildIfaceIdWithAppId(uint8 ifaceIndex, sint15 appId);
  static uint32 BuildIfaceIdWithPolicyBit(uint8 ifaceIndex);

  // This function is called from different places in code with different ifaceIdTemp formats
  // If one of ifaceIdTemp has it policy bit on (meaning that it was created either with dss_get_iface_id_by_policy
  // or with IDSNetworkPriv) , then just compare the iface index , otherwise compare everything
  static bool CompareIfaceIds(
    dss_iface_id_type ifaceId1,
    dss_iface_id_type ifaceId2);

  inline static FlowID StripDownToFlowID(uint32 handle)
  {
    return handle & 0xFF;
  }

  inline static dss_iface_id_type StripAppIdFromIfaceId(dss_iface_id_type ifaceId)
  {
    return ifaceId | 0x00FFFF00;
  }

  inline static dss_iface_id_type DSSIfaceId2DSNetIfaceId(dss_iface_id_type ifaceId)
  {
    return ifaceId = StripAppIdFromIfaceId(ifaceId & 0xFF000000);
  }

  inline static bool PolicyFlagIsOn(dss_iface_id_type ifaceId)
  {
    return ((ifaceId & IFACE_ID_POLICY_FLAG) != 0);
  }

  inline static dss_iface_id_type GetIfaceIndex(dss_iface_id_type ifaceId)
  {
    return ifaceId >> 24;
  }

  // turn on the policy flag that indecated that this iface id was returned calling get_iface_id_by_policy().
  inline static dss_iface_id_type TurnPolicyFlagOn(dss_iface_id_type ifaceId)
  {
    return ifaceId | IFACE_ID_POLICY_FLAG;
  }

  // get app Id
  inline static sint15 GetAppId(dss_iface_id_type ifaceId)
  {
    // first leave only app Id, then move to the right
    return static_cast<sint15>((ifaceId & 0x00FFFF00) >> 8);
  }

  // get app Id or -1 for policy flag
  inline static sint15 GetAppIdOrPolicy(dss_iface_id_type ifaceId)
  {
    sint15 appId = -1;
    if (!PolicyFlagIsOn(ifaceId))
    {
      appId = DSSIfaceId::GetAppId(ifaceId);
    }

    return appId;
  }

//-------------------------------------------------------------------
//  Get/Set functions for protected members access
//
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//  Protected members
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//  Data members
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//  Protected members
//-------------------------------------------------------------------

private:

  // degenerated C-Tor
  DSSIfaceId();

  // degenerated D-Tor
  ~DSSIfaceId();

};

//===================================================================

#endif // __DSS_IFACEID_H__


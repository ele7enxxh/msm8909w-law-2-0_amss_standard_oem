#ifndef DS_NET_IPOLICY_H
#define DS_NET_IPOLICY_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
namespace ds
{
   namespace Net
   {
      
      /**
        * PolicyFlag: Specifies network interface compatibility to the policy in
        * respect to the interface's Data Connection.
        */
      typedef int PolicyFlagType;
      namespace PolicyFlag
      {
         
         /**
           * ANY: The state of the Data Connection is irrelevant for selecting a
           * Network interface compatible to the policy.
           */
         const ::ds::Net::PolicyFlagType QDS_ANY = 0;
         
         /**
           * UP_ONLY: A Network interface shall be compatible to the policy only
           * if all its characteristics are compatible to the policy AND data
           * connection is already established on the interface.
           * INetwork creation per that policy shall fail if there are interfaces
           * compatible to the policy but none of them in UP state.
           */
         const ::ds::Net::PolicyFlagType QDS_UP_ONLY = 1;
         
         /**
           * UP_PREFERRED: If there are several interfaces compatible with the
           * policy specifications, but some of them in UP state and some not, an
           * interface which is in UP state shall be selected. If Data Connection
           * is not established for all of the compatible interfaces, one of them
           * shall still be selected for the operation where the policy was
           * specified (the operation will not fail).
           */
         const ::ds::Net::PolicyFlagType QDS_UP_PREFERRED = 2;
      };
      
      /**
        * PolicyProfileType: This type is used in the ProfileList member of the
        * Policy interface.
        */
      struct PolicyProfileType {
         int ProfileID;
         int ProfileValue;
      };
      struct _SeqPolicyProfileType__seq_PolicyProfileType_Net_ds {
         PolicyProfileType* data;
         int dataLen;
         int dataLenReq;
      };
      typedef _SeqPolicyProfileType__seq_PolicyProfileType_Net_ds SeqPolicyProfileType;
      const ::AEEIID AEEIID_IPolicy = 0x106c547;
      
      /** @interface IPolicy
        * 
        * ds Network Policy interface.
        * Instantiation of this interface shall create a default policy.
        * That policy can be used as is to produce a default INetwork object via
        * INetworkFactory::CreateNetwork. Alternatively, applications can change
        * one or more members of the Policy object and use it in
        * INetworkFactory::CreateNetwork to request instantiation of a
        * non-default INetwork object.
        */
      struct IPolicy : public ::IQI
      {
         
         /**          
           * This attribute represents the interface name (Network Type) of the
           * policy.
           * Either Network Group *or* Network Type is taken into account when
           * the policy takes effect.
           * If both are set, the last one being set takes effect.          
           * @param value Attribute value
           * @see SetIfaceGroup
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetIfaceName(::ds::Net::IfaceNameType* value) = 0;
         
         /**          
           * This attribute represents the interface name (Network Type) of the
           * policy.
           * Either Network Group *or* Network Type is taken into account when
           * the policy takes effect.
           * If both are set, the last one being set takes effect.          
           * @param value Attribute value
           * @see SetIfaceGroup
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetIfaceName(::ds::Net::IfaceNameType value) = 0;
         
         /**          
           * This attribute represents the interface group (Network Group) of
           * the policy.
           * Either Network Group *or* Network Type is taken into account when
           * the policy takes effect.
           * If both are set, the last one being set takes effect.          
           * @param value Attribute value
           * @see SetIfaceName
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetIfaceGroup(::ds::Net::IfaceGroupType* value) = 0;
         
         /**          
           * This attribute represents the interface group (Network Group) of
           * the policy.
           * Either Network Group *or* Network Type is taken into account when
           * the policy takes effect.
           * If both are set, the last one being set takes effect.          
           * @param value Attribute value
           * @see SetIfaceName
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetIfaceGroup(::ds::Net::IfaceGroupType value) = 0;
         
         /**
           * This attribute indicates the policy flags as part of the network
           * policy.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetPolicyFlag(::ds::Net::PolicyFlagType* value) = 0;
         
         /**
           * This attribute indicates the policy flags as part of the network
           * policy.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetPolicyFlag(::ds::Net::PolicyFlagType value) = 0;
         
         /**
           * This attribute indicates the address family as part of the network
           * policy.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetAddressFamily(::ds::AddrFamilyType* value) = 0;
         
         /**
           * This attribute indicates the address family as part of the network
           * policy.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetAddressFamily(::ds::AddrFamilyType value) = 0;
         
         /**
           * This attribute indicates the CDMA profile number as part of the
           * network policy. In general, profile numbers are carrier specific.
           *
           * In CDMA, when the service processes the policy, it always uses a
           * profile number. If the application does not specify any, the
           * service uses the default profile number. The application should
           * take care that the attributes it specifies in the policy are
           * compatible to the specifications in the profile specified in the 
           * policy (or to the default profile if no profile is specified).
           *          
           * In CDMA the profile number may be related to the AppType (OMH).
           * There are cases where both this attribute and
           * UMTSProfileNumber are set in a single policy, for example, when app
           * specifies IfaceGroup that includes both CDMA and UMTS interfaces, 
           * or when the policy is set to support some inter technology hand off
           * scenarios.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetCDMAProfileNumber(int* value) = 0;
         
         /**
           * This attribute indicates the CDMA profile number as part of the
           * network policy. In general, profile numbers are carrier specific.
           *
           * In CDMA, when the service processes the policy, it always uses a
           * profile number. If the application does not specify any, the
           * service uses the default profile number. The application should
           * take care that the attributes it specifies in the policy are
           * compatible to the specifications in the profile specified in the 
           * policy (or to the default profile if no profile is specified).
           *          
           * In CDMA the profile number may be related to the AppType (OMH).
           * There are cases where both this attribute and
           * UMTSProfileNumber are set in a single policy, for example, when app
           * specifies IfaceGroup that includes both CDMA and UMTS interfaces, 
           * or when the policy is set to support some inter technology hand off
           * scenarios.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetCDMAProfileNumber(int value) = 0;
         
         /**
           * This attribute indicates the UMTS profile number as part of the
           * network policy. In general, profile numbers are carrier specific.
           *
           * In UMTS, when the service processes the policy, it always uses a
           * profile number. If the application does not specify any, the
           * service uses the default profile number. The application should
           * take care that the attributes it specifies in the policy are
           * compatible to the specifications in the profile specified in the 
           * policy (or to the default profile if no profile is specified).
           *          
           * There are cases where both this attribute and CDMAProfileNumber are
           * set in a single policy, for example, when app specifies IfaceGroup
           * that includes both CDMA and UMTS interfaces, or when the policy is
           * set to support some inter technology hand off scenarios.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetUMTSProfileNumber(int* value) = 0;
         
         /**
           * This attribute indicates the UMTS profile number as part of the
           * network policy. In general, profile numbers are carrier specific.
           *
           * In UMTS, when the service processes the policy, it always uses a
           * profile number. If the application does not specify any, the
           * service uses the default profile number. The application should
           * take care that the attributes it specifies in the policy are
           * compatible to the specifications in the profile specified in the 
           * policy (or to the default profile if no profile is specified).
           *          
           * There are cases where both this attribute and CDMAProfileNumber are
           * set in a single policy, for example, when app specifies IfaceGroup
           * that includes both CDMA and UMTS interfaces, or when the policy is
           * set to support some inter technology hand off scenarios.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetUMTSProfileNumber(int value) = 0;
         
         /**
           * This attribute indicates a list of profile numbers that define the
           * policy.
           * In general, profile numbers are carrier specific.
           * In CDMA the profile number may be related to the AppType (OMH)
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetProfileList(::ds::Net::PolicyProfileType* value, int valueLen, int* valueLenReq) = 0;
         
         /**
           * This attribute indicates a list of profile numbers that define the
           * policy.
           * In general, profile numbers are carrier specific.
           * In CDMA the profile number may be related to the AppType (OMH)
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetProfileList(const ::ds::Net::PolicyProfileType* value, int valueLen) = 0;
         
         /**
           * Set to TRUE to select a routable interface. An interface is routable
           * if it is brought up by a tethered device. In default policy this
           * field is set to FALSE.
           * Only applications that need to modify the behavior of a tethered
           * call should set this flag.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRoutable(boolean* value) = 0;
         
         /**
           * Set to TRUE to select a routable interface. An interface is routable
           * if it is brought up by a tethered device. In default policy this
           * field is set to FALSE.
           * Only applications that need to modify the behavior of a tethered
           * call should set this flag.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetRoutable(boolean value) = 0;
         
         /**
           * This attribute indicates the application type which is used for
           * network usage arbitration.
           * Application Types used on a carrier network shall be based on the
           * standard applicable to that network. One such standard, for example,
           * is C.S0023-D v1.0 R-UIM (See "APPLICATIONS" under EFSIPUPPExt - 
           * SimpleIP User Profile Parameters 1 Extension, in chapter "Multi-Mode
           * R-UIM Dedicated File (DF) and Elementary File (EF) Structure").
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetAppType(int* value) = 0;
         
         /**
           * This attribute indicates the application type which is used for
           * network usage arbitration.
           * Application Types used on a carrier network shall be based on the
           * standard applicable to that network. One such standard, for example,
           * is C.S0023-D v1.0 R-UIM (See "APPLICATIONS" under EFSIPUPPExt - 
           * SimpleIP User Profile Parameters 1 Extension, in chapter "Multi-Mode
           * R-UIM Dedicated File (DF) and Elementary File (EF) Structure").
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetAppType(int value) = 0;
      };
   };
};
#endif //DS_NET_IPOLICY_H

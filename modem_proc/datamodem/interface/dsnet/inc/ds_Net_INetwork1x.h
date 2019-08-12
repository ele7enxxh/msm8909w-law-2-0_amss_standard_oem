#ifndef DS_NET_INETWORK1X_H
#define DS_NET_INETWORK1X_H

/*============================================================================
  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Net_DownReasons_Def.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_INetwork.h"
namespace ds
{
   namespace Net
   {
      typedef int Network1xMDRModeType;
      namespace Network1xMDRMode
      {
         const ::ds::Net::Network1xMDRModeType MDR_ONLY = 0;
         const ::ds::Net::Network1xMDRModeType IF_AVAIL = 1;
         const ::ds::Net::Network1xMDRModeType NO_MDR = 2;
         const ::ds::Net::Network1xMDRModeType SO33_PREF = 3;
      };
      typedef int Network1xPrivHysteresisTimerType;
      namespace Network1xRLP
      {
         const int MAX_NAK_ROUNDS = 3;
      };
      struct Network1xRLPOptionType {
         unsigned char rscbIndex;
         unsigned char nakRoundsFwd;
         unsigned char naksPerRoundFwd[3];
         unsigned char nakRoundsRev;
         unsigned char naksPerRoundRev[3];
      };
      typedef int Network1xSIPMIPCallType;
      namespace Network1xSIPMIPCall
      {
        const ::ds::Net::Network1xSIPMIPCallType NONE      = 0;
        const ::ds::Net::Network1xSIPMIPCallType SIP       = 1;
        const ::ds::Net::Network1xSIPMIPCallType MIP       = 2;
      };

      typedef int Network1xDoSFlagsType;
      namespace Network1xDoSFlags
      {
         const ::ds::Net::Network1xDoSFlagsType QDS_MSG_EXPEDITE = 0x1;
         const ::ds::Net::Network1xDoSFlagsType QDS_MSG_FAST_EXPEDITE = 0x2;
      };
      const ::AEEIID AEEIID_INetwork1x = 0x106ce1c;
      
      /** @interface INetwork1x
        * 
        * ds Net Network 1x interface.
        */
      struct INetwork1x : public ::IQI
      {
         
         /**
           * This function is used to query if DoS (Data Over Signaling) is
           * supported. The flags parameter is used to query support for
           * specific DoS features (same features used in Socket SendTo
           * operations).
           * DoS and SDB (Short Data Burst) are parallel terms.
           * DoS is the term used in 1xEVDO
           * SDB is the term used in CDMA 1x.
           * @param flags DoS flags to be used.
           * @param dosSupported TRUE if DoS is supported.
           *                     FALSE if DoS is not supported.
           * @retval AEE_SUCCESS Registration succeeded.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL QueryDoSSupport(::ds::Net::Network1xDoSFlagsType flags, boolean* dosSupported) = 0;
         
         /**
           * This attribute indicates the medium data rate value.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetMDR(int* value) = 0;
         
         /**
           * This attribute indicates the medium data rate value.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetMDR(int value) = 0;
         
         /**
           * This attribute indicates the QoS NA priority.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetQoSNAPriority(int* value) = 0;
         
         /**
           * This attribute indicates the QoS NA priority.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetQoSNAPriority(int value) = 0;
         
         /**
           * This attribute indicates the 1x RLP current NAK policy information.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRLPAllCurrentNAK(::ds::Net::Network1xRLPOptionType* value) = 0;
         
         /**
           * This attribute indicates the 1x RLP current NAK policy information.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetRLPAllCurrentNAK(const ::ds::Net::Network1xRLPOptionType* value) = 0;
         
         /**
           * This attribute indicates the 1x RLP default NAK policy information.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRLPDefCurrentNAK(::ds::Net::Network1xRLPOptionType* value) = 0;
         
         /**
           * This attribute indicates the 1x RLP default NAK policy information.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetRLPDefCurrentNAK(const ::ds::Net::Network1xRLPOptionType* value) = 0;
         
         /**
           * This attribute indicates the 1x RLP negotiated NAK policy information.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRLPNegCurrentNAK(::ds::Net::Network1xRLPOptionType* value) = 0;
         
         /**
           * This attribute indicates the hysteresis act timer.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetHysteresisTimer(int* value) = 0;
         
         /**
           * This attribute indicates the hysteresis act timer.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetHysteresisTimer(int value) = 0;

         /**
           * This attribute indicates the data call type mip, sim & none.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSIPMIPCallType(Network1xDoSFlagsType* callType) = 0;

      };
   };
};
#endif //DS_NET_INETWORK1X_H

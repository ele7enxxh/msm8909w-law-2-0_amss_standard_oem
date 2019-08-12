#ifndef DS_NET_INETWORK1XPRIV_H
#define DS_NET_INETWORK1XPRIV_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_Def.h"
#include "ds_Net_IEventManager.h"
#include "ds_Addr_Def.h"
#include "ds_Net_DownReasons_Def.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_INetwork.h"
#include "ds_Net_INetwork1x.h"
namespace ds
{
   namespace Net
   {
      namespace Network1xPrivEvent
      {
         const int SLOTTED_MODE_RESULT = 0x106e11a;
         const int SLOTTED_MODE_CHANGED = 0x106e11b;
         const int HDR_REV0_RATE_INERTIA_RESULT = 0x106e11c;
      };
      struct Network1xPrivHDRSlottedModeArg {
         boolean enable;
         AEEINTERFACE_PADMEMBERS(slottedModeOption, 3)
         int slottedModeOption;
         int getSlottedMode;
      };
      typedef int Network1xPrivResultCodeType;
      namespace Network1xPrivSlottedMode
      {
         const ::ds::Net::Network1xPrivResultCodeType REQUEST_SUCCEES = 0;
         const ::ds::Net::Network1xPrivResultCodeType REQUEST_REJECTED = 1;
         const ::ds::Net::Network1xPrivResultCodeType REQUEST_FAILED_TX = 2;
         const ::ds::Net::Network1xPrivResultCodeType REQUEST_NO_NET = 3;
         const ::ds::Net::Network1xPrivResultCodeType REQUEST_UNSUPPORTED = 4;
      };
      typedef int Network1xPrivSessionTimerSelectType;
      namespace Network1xPrivSessionTimerSelect
      {
         const ::ds::Net::Network1xPrivSessionTimerSelectType S_INVALID = 0;
         const ::ds::Net::Network1xPrivSessionTimerSelectType S_DO = 1;
         const ::ds::Net::Network1xPrivSessionTimerSelectType S_1X = 2;
         const ::ds::Net::Network1xPrivSessionTimerSelectType S_1X_AND_DO = 3;
      };
      struct Network1xPrivSessionTimerType {
         Network1xPrivSessionTimerSelectType select;
         short value;
         AEEINTERFACE_PADMEMBERS(__pad, 2)
      };
      typedef int Network1xPrivHDRRev0RateInertiaFailureCodeType;
      namespace Network1xPrivHDRRev0RateInertiaFailureCode
      {
         const ::ds::Net::Network1xPrivHDRRev0RateInertiaFailureCodeType REQUEST_REJECTED = 0;
         const ::ds::Net::Network1xPrivHDRRev0RateInertiaFailureCodeType REQUEST_FAILED_TX = 1;
         const ::ds::Net::Network1xPrivHDRRev0RateInertiaFailureCodeType NOT_SUPPORTED = 2;
         const ::ds::Net::Network1xPrivHDRRev0RateInertiaFailureCodeType NO_NET = 3;
      };
      const ::AEEIID AEEIID_INetwork1xPriv = 0x10741e0;
      
      /** @interface INetwork1xPriv
        * 
        * ds Net Network 1x Priv interface.
        * Events that can be registered on this interface via OnStateChange (as part of IEventManager interface): 
        * - ds::Net::Network1xPrivEvent::SLOTTED_MODE_RESULT. Use GetHDRSlottedModeResult to fetch the slotted mode result information.
        * - ds::Net::Network1xPrivEvent::SLOTTED_MODE_CHANGED. Use GetHDRSlottedModeCycleIndex to fetch the new slotted mode cycle index. TODO: Verify that this API provides the information relevant to this event.
        * - ds::Net::Network1xPrivEvent::HDR_REV0_RATE_INERTIA_RESULT. Use GetHDRRev0RateInteriaResult to fetch the rate information.
        */
      struct INetwork1xPriv : public ::ds::Net::IEventManager
      {
         
         /**
           * This function is used to set the 1x holddown timer.
           * @param enable Used to enable/disable the functionality.
           * @retval AEE_SUCCESS Registration succeeded.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL EnableHoldDown(boolean enable) = 0;
         
         /**
           * This function is used to set the mode for operating VT on 
           * a Rev 0 HDR system.
           * @param enable Used to enable/disable the functionality.
           * @retval AEE_SUCCESS Registration succeeded.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL EnableHDRRev0RateInertia(boolean enable) = 0;
         
         /**
           * This function is used to get the result of attempting to 
           * set the mode for operating VT on a Rev 0 HDR system.
           * @param result Holds the result.
           * @retval AEE_SUCCESS Registration succeeded.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetHDRRev0RateInertiaResult(boolean* result) = 0;
         
         /**
           * This function is used to set the HDR slotted mode.
           * @param arg Slotted mode argument.
           * @see ds::Net::Network1xPriv::HDRSlottedModeArg.
           * @retval AEE_SUCCESS Registration succeeded.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL EnableHDRSlottedMode(const ::ds::Net::Network1xPrivHDRSlottedModeArg* arg) = 0;
         
         /**
           * This function is used to get the result of attempting to 
           * set HDR slotted mode.
           * @param resultCode Holds the result.
           * @see ds::Net::Network1x::ResultCodeType.
           * @retval AEE_SUCCESS Registration succeeded.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetHDRSlottedModeResult(::ds::Net::Network1xPrivResultCodeType* resultCode) = 0;
         
         /**
           * This function is used to get HDR slotted mode cycle index.
           * @param sci Holds the HDR Slotted mode cycle index.
           * @retval AEE_SUCCESS Registration succeeded.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetHDRSlottedModeCycleIndex(::uint32* sci) = 0;
         
         /**
           * This function is used to set the HDR HPT mode.
           * @param enable Used to enable/disable the functionality.
           * @retval AEE_SUCCESS Registration succeeded.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL EnableHDRHPTMode(boolean enable) = 0;
         
         /**
           * This attribute indicates the dormacy timer.
           * @param value Attribute value
           */
         virtual int AEEINTERFACE_CDECL GetDormancyTimer(int* value) = 0;
         
         /**
           * This attribute indicates the dormacy timer.
           * @param value Attribute value
           */
         virtual int AEEINTERFACE_CDECL SetDormancyTimer(int value) = 0;
         
         /**
           * This attribute indicates the session timer.
           * @param value Attribute value
           */
         virtual int AEEINTERFACE_CDECL GetSessionTimer(::ds::Net::Network1xPrivSessionTimerType* value) = 0;
         
         /**
           * This attribute indicates the session timer.
           * @param value Attribute value
           */
         virtual int AEEINTERFACE_CDECL SetSessionTimer(const ::ds::Net::Network1xPrivSessionTimerType* value) = 0;
         
         /**
           * This attribute indicates the 1x HandDown Option information.
           * @param value Attribute value
           */
         virtual int AEEINTERFACE_CDECL GetHDR1xHandDownOption(boolean* value) = 0;
         
         /**
           * This attribute indicates the 1x HandDown Option information.
           * @param value Attribute value
           */
         virtual int AEEINTERFACE_CDECL SetHDR1xHandDownOption(boolean value) = 0;
         
         /**
           * This function gets the result info code from HDR Rev0 
           * Rate inertia event.
           * @param infoCode Info code returned
           * @retval AEE_SUCCESS Registration succeeded.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetHDRRev0RateInertiaResultInfoCode(::ds::Net::Network1xPrivHDRRev0RateInertiaFailureCodeType* failureCode) = 0;
      };
   };
};
#endif //DS_NET_INETWORK1XPRIV_H

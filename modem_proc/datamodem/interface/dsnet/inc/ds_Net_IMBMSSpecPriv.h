#ifndef DS_NET_IMBMSSPECPRIV_H
#define DS_NET_IMBMSSPECPRIV_H

#ifdef FEATUTE_DATA_PS_MCAST
/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
namespace ds
{
   namespace Net
   {
      typedef int MBMSServiceType;
      namespace MBMSService
      {
         const ::ds::Net::MBMSServiceType QDS_STREAMING = 1;
         const ::ds::Net::MBMSServiceType QDS_DOWNLOAD = 2;
      };
      typedef int MBMSServiceMethodType;
      namespace MBMSServiceMethod
      {
         const ::ds::Net::MBMSServiceMethodType QDS_BROADCAST = 1;
         const ::ds::Net::MBMSServiceMethodType QDS_MULTICAST = 2;
      };
      const ::AEEIID AEEIID_IMBMSSpecPriv = 0x106cf97;
      
      /** @interface IMBMSSpecPriv
        * 
        * MBMS Spec Info interface.
        */
      struct IMBMSSpecPriv : public ::IQI
      {
         virtual ::AEEResult AEEINTERFACE_CDECL GetTMGI(::uint64* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetTMGI(::uint64 value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetSessionStartTime(::uint64* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetSessionStartTime(::uint64 value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetSessionEndTime(::uint64* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetSessionEndTime(::uint64 value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetPriority(unsigned short* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetPriority(unsigned short value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetService(::ds::Net::MBMSServiceType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetService(::ds::Net::MBMSServiceType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetServiceMethod(::ds::Net::MBMSServiceMethodType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetServiceMethod(::ds::Net::MBMSServiceMethodType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetSelectedService(boolean* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetSelectedService(boolean value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetServiceSecurity(boolean* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetServiceSecurity(boolean value) = 0;
      };
   };
};
#endif // FEATUTE_DATA_PS_MCAST
#endif //DS_NET_IMBMSSPECPRIV_H

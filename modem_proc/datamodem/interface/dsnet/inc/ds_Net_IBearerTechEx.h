#ifndef DS_NET_IBEARERTECHEX_H
#define DS_NET_IBEARERTECHEX_H

/*============================================================================
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Net_Def.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IBearerTechEx = 0x106c6a6;
      struct IBearerTechEx : public ::IQI
      {
         /**
           * This attribute indicates the technology type of the data connection (3GPP/3GPP2)
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTechnology(unsigned int* value) = 0;
         
         /**
           * This attribute indicates the RAT of the bearer.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRatValue(unsigned int* value) = 0;
         
         /**
           * This attribute indicates the SO mask for 3GPP2.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSoMask(unsigned int* value) = 0;
      };
   };
};
#endif //DS_NET_IBEARERTECHEX_H

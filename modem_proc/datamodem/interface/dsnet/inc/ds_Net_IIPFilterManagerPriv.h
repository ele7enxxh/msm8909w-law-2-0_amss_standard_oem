#ifndef DS_NET_IIPFILTERMANAGERPRIV_H
#define DS_NET_IIPFILTERMANAGERPRIV_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Net_Def.h"
#include "ds_Utils_IQI.h"
#include "ds_Addr_Def.h"
#include "ds_Net_IIPFilterPriv.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IIPFilterRegPriv.h"
#include "dssocket_defs.h"

namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IIPFilterManagerPriv = 0x107dd61;
      
      /** @interface IIPFilterManagerPriv
        * 
        * ds network IP Filter manager interface.
        * This interface manages the IP filters defined on INetwork object.
        * Application creates INetwork object and call IQI with IIPFilterMgr - TODO - move this documentation to a bid file.
        * This is privileged - internal interface
        */
      struct IIPFilterManagerPriv : public ::IQI
      {
         
         /**
           * This function registers filters on specific ds Network object.
           * IMPORTANT: This interface is non-public. If and when it becomes
           *            public there may be need to change the usage of sequence
           *            to IGroup-like interface if there is a requirement to
           *            support more than 14 filters in a single API call
           *            (currently not supported in CS).
           *            In Addition, IIPFilterRegPriv shall be modified if this
           *            functionality is exposed to external apps. See comment
           *            in IIPFilterRegPriv.
           * 
           * @param fi_result TODO: document this parameter.
           * @param filters List of IPFilters to be defined on the ds Network object.
           *                define IP filters on the ds Network object.
           *                           
           * @See IIPFilter.
           * @retval AEE_SUCCESS Registration succeeded.
           * @retval ds::EBADOPTS Bad options.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL RegisterFilters(int fi_result, ::ds::Net::IIPFilterPriv** filters, int filtersLen, ::ds::Net::IIPFilterRegPriv** filterReg, IfaceGroupType ifaceGroup) = 0;
      };
   };
};
#endif //DS_NET_IIPFILTERMANAGERPRIV_H

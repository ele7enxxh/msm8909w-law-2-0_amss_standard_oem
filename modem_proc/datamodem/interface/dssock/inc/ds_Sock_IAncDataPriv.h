#ifndef DS_SOCK_IANCDATAPRIV_H
#define DS_SOCK_IANCDATAPRIV_H

/*============================================================================
  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#if !defined(AEEINTERFACE_CPLUSPLUS)
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
typedef int ds_Sock_AncDataIDType;

/** @memberof ds_Sock_AncData
  * 
  * Ancillary Data ID for ICMP extended error information (used in RecvMsg)
  */
#define ds_Sock_AncData_ICMP_ERROR_INFO 1

/** @memberof ds_Sock_AncData
  * 
  * Ancillary Data ID for RECVIF information (used in RecvMsg)
  */
#define ds_Sock_AncData_RECV_IF_INFO 2
#define ds_Sock_AEEIID_IAncDataPriv 0x106c947

/** @interface ds_Sock_IAncDataPriv
  * 
  * ds Socket Ancillary Data interface.
  * This interface provides a common base for all the possible
  * Ancillary data that may be defined in the future for use via SendMsg/RecvMsg.
  */
#define INHERIT_ds_Sock_IAncDataPriv(iname) \
   INHERIT_IQI(iname); \
   AEEResult (*GetAncID)(iname* _pif, ds_Sock_AncDataIDType* value); \
   AEEResult (*SetAncID)(iname* _pif, ds_Sock_AncDataIDType value)
AEEINTERFACE_DEFINE(ds_Sock_IAncDataPriv);

/** @memberof ds_Sock_IAncDataPriv
  * 
  * Increment the object's reference count.
  *
  * @param _pif Pointer to interface
  **/
static __inline uint32 ds_Sock_IAncDataPriv_AddRef(ds_Sock_IAncDataPriv* _pif)
{
   return AEEGETPVTBL(_pif, ds_Sock_IAncDataPriv)->AddRef(_pif);
}

/** @memberof ds_Sock_IAncDataPriv
  * 
  * Decrement the object's reference count.  If zero, free the object.
  *
  * @param _pif Pointer to interface
  **/
static __inline uint32 ds_Sock_IAncDataPriv_Release(ds_Sock_IAncDataPriv* _pif)
{
   return AEEGETPVTBL(_pif, ds_Sock_IAncDataPriv)->Release(_pif);
}

/** @memberof ds_Sock_IAncDataPriv
  * 
  * Detect if an object implements the requested interface.
  *
  * @param _pif Pointer to interface
  * @param iqi_iid Interface ID of requested interface
  * @param iqi Requested interface.
  */
static __inline int ds_Sock_IAncDataPriv_QueryInterface(ds_Sock_IAncDataPriv* _pif, AEEIID iqi_iid, void** iqi)
{
   return AEEGETPVTBL(_pif, ds_Sock_IAncDataPriv)->QueryInterface(_pif, iqi_iid, iqi);
}
static __inline AEEResult ds_Sock_IAncDataPriv_GetAncID(ds_Sock_IAncDataPriv* _pif, ds_Sock_AncDataIDType* value)
{
   return AEEGETPVTBL(_pif, ds_Sock_IAncDataPriv)->GetAncID(_pif, value);
}
static __inline AEEResult ds_Sock_IAncDataPriv_SetAncID(ds_Sock_IAncDataPriv* _pif, ds_Sock_AncDataIDType value)
{
   return AEEGETPVTBL(_pif, ds_Sock_IAncDataPriv)->SetAncID(_pif, value);
}

/**
  * Ancillary data SendMsg and RecvMsg API
  */
struct ds_Sock__SeqIAncDataPrivType__seq_IAncDataPriv_Sock_ds {
   struct ds_Sock_IAncDataPriv** data;
   int dataLen;
   int dataLenReq;
};
typedef struct ds_Sock__SeqIAncDataPrivType__seq_IAncDataPriv_Sock_ds ds_Sock__SeqIAncDataPrivType__seq_IAncDataPriv_Sock_ds;
typedef ds_Sock__SeqIAncDataPrivType__seq_IAncDataPriv_Sock_ds ds_Sock_SeqIAncDataPrivType;
#else /* C++ */
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
namespace ds
{
   namespace Sock
   {
      typedef int AncDataIDType;
      namespace AncData
      {
         
         /**
           * Ancillary Data ID for ICMP extended error information (used in RecvMsg)
           */
         const ::ds::Sock::AncDataIDType ICMP_ERROR_INFO = 1;
         
         /**
           * Ancillary Data ID for RECVIF information (used in RecvMsg)
           */
         const ::ds::Sock::AncDataIDType RECV_IF_INFO = 2;
      };
      const ::AEEIID AEEIID_IAncDataPriv = 0x106c947;
      
      /** @interface IAncDataPriv
        * 
        * ds Socket Ancillary Data interface.
        * This interface provides a common base for all the possible
        * Ancillary data that may be defined in the future for use via SendMsg/RecvMsg.
        */
      struct IAncDataPriv : public ::IQI
      {
         virtual ::AEEResult AEEINTERFACE_CDECL GetAncID(::ds::Sock::AncDataIDType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetAncID(::ds::Sock::AncDataIDType value) = 0;
      };
      
      /**
        * Ancillary data SendMsg and RecvMsg API
        */
      struct _SeqIAncDataPrivType__seq_IAncDataPriv_Sock_ds {
         IAncDataPriv** data;
         int dataLen;
         int dataLenReq;
      };
      typedef _SeqIAncDataPrivType__seq_IAncDataPriv_Sock_ds SeqIAncDataPrivType;
   };
};
#endif /* !defined(AEEINTERFACE_CPLUSPLUS) */
#endif //DS_SOCK_IANCDATAPRIV_H

#ifndef MBMS_SPEC_H
#define MBMS_SPEC_H
#ifdef FEATUTE_DATA_PS_MCAST
/*==========================================================================*/ 
/*! 
  @file 
  ds_Net_MBMSSpec.h

  @brief
  This file defines the class that implements the IMBMSSpecPriv 
  interface.

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MBMSSpec.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-09-12 hm  Created module.

===========================================================================*/
/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_StdErr.h"
#include "ds_Errors_Def.h"
#include "ds_Net_IMBMSSpecPriv.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Utils_ICritSect.h"

namespace ds
{
namespace Net
{
/*lint -esym(1510, IMBMSSpecPriv) */
/*lint -esym(1510, IQI) */

/*!
  @class
  MBMSSpec

  @brief
  This class provides implementation for IMBMSSpecPriv interface.

  @todo
  Write details.
*/
class MBMSSpec : public IMBMSSpecPriv
{
private:
  /*!
    This is the critical section used by this object. DO NOT
    define multiple crit sects per object
  */
  ICritSect*                mpICritSect;
  /* Private variables */
  uint64                            mTmgi;
  uint64                            mSessionStartTime;
  uint64                            mSessionEndTime;
  unsigned short int                mPriority;
  MBMSServiceType          mServiceType;
  MBMSServiceMethodType    mServiceMethodType;
  boolean                           mSelectedService;
  boolean                           mServiceSecurity;

  /*
  @details
  The destructor of this object is private. Use Release() method of the
  IQI interface to free the object.

  @param      None.
  @return     None.
  @see        IQI::Release()
  */
  virtual ~MBMSSpec
  (
    void 
  ) 
  throw();

public:

  MBMSSpec();

  /* Functions derived from IMBMSSpecPriv interface */
  /* TODO: Functions are awefully named, use correct mixed capitalization in the IDLs */
  virtual ds::ErrorType CDECL GetTMGI (uint64* TMGI);
  virtual ds::ErrorType CDECL SetTMGI (uint64 TMGI);
  virtual ds::ErrorType CDECL GetSessionStartTime (uint64* SessionStartTime);
  virtual ds::ErrorType CDECL SetSessionStartTime (uint64 SessionStartTime);
  virtual ds::ErrorType CDECL GetSessionEndTime (uint64* SessionEndTime);
  virtual ds::ErrorType CDECL SetSessionEndTime (uint64 SessionEndTime);
  virtual ds::ErrorType CDECL GetPriority (unsigned short int* Priority);
  virtual ds::ErrorType CDECL SetPriority (unsigned short int Priority);
  virtual ds::ErrorType CDECL GetService (MBMSServiceType* Service);
  virtual ds::ErrorType CDECL SetService (MBMSServiceType Service);
  virtual ds::ErrorType CDECL GetServiceMethod (MBMSServiceMethodType* ServiceMethod);
  virtual ds::ErrorType CDECL SetServiceMethod (MBMSServiceMethodType ServiceMethod);
  virtual ds::ErrorType CDECL GetSelectedService (boolean* SelectedService);
  virtual ds::ErrorType CDECL SetSelectedService (boolean SelectedService);
  virtual ds::ErrorType CDECL GetServiceSecurity (boolean* ServiceSecurity);
  virtual ds::ErrorType CDECL SetServiceSecurity (boolean ServiceSecurity); 

  /* Functions inherited from IQI interface */
  DSIQI_DECL_LOCALS()
  DSIQI_ADDREF()
  DSIQI_RELEASE()

  virtual ds::ErrorType CDECL QueryInterface (AEEIID iid, void **ppo);

  /*-------------------------------------------------------------------------
  Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_MBMS_JOIN_INFO)

}; /* class MBMSSpec */

} /* namespace Net */
} /* namespace ds */
#endif //FEATUTE_DATA_PS_MCAST
#endif /* MBMS_JOIN_INFO_H */


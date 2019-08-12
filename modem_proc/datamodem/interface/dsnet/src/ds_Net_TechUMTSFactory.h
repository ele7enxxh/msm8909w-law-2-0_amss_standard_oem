#ifndef TECH_UMTS_FACTORY_H
#define TECH_UMTS_FACTORY_H

/*===========================================================================
  @file TechUMTSFactory.h

  This file defines the class that implements the ITechUMTS interface.

  TODO: Detailed explaination about the class here.

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_TechUMTSFactory.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-03-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Errors_Def.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_ITechUMTS.h"
#include "ds_Net_IPolicy.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Utils_Singleton.h"

namespace ds
{

namespace Net
{
   /*lint -esym(1510, ITechUMTS) */
   /*lint -esym(1510, IQI) */
// There is no need to AddRef/Release on this class, it's a singleton
class TechUMTSFactory : public ITechUMTS,
                        public ds::Utils::Singleton<TechUMTSFactory>
{
  friend class ds::Utils::Singleton<TechUMTSFactory>;

  TechUMTSFactory () throw();
  /*!
  @brief
  Private destructor.

  @details
  The destructor of this object is private. Use Release() method of the
  IQI interface to free the object.

  @param      None.
  @return     None.
  @see        IQI::Release()
  */
  virtual ~TechUMTSFactory
  (
    void
  )
  throw();

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_TECH_UMTS_FACTORY)

  /*!
    This is the critical section used by this object. DO NOT
    define multiple crit sects per object
  */
  ICritSect*                mpICritSect;
public:

  /*-------------------------------------------------------------------------
    Functions inherited from ITechUMTS interface
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL RegMTPD
  (
    IPolicy* Policy,
    ISignal* signalObj,
    IQI**    mtpdRegObj
  );

  /*-------------------------------------------------------------------------
    Functions inherited from IQI interface
  -------------------------------------------------------------------------*/
  DSIQI_IMPL_DEFAULTS(ITechUMTS);

  /*-------------------------------------------------------------------------
    Mobile Terminated Packet Data event
  -------------------------------------------------------------------------*/
  const static EventType QDS_EV_MTPD  = 0x0106e612;

}; /* class TechUMTSFactory */
} /* namespace Net */
} /* namespace ds */

/* Declaration CreateInstance method for TechUMTSFactory */
DSIQI_DECL_CREATE_INSTANCE2(DS,Net,TechUMTSFactory)

#endif /* TECH_UMTS_FACTORY_H */


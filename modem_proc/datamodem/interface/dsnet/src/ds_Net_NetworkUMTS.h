#ifndef DS_NET_NETWORK_UMTS_H
#define DS_NET_NETWORK_UMTS_H

/*===========================================================================
  @file NetworkUMTS.h

  This file defines the class that implements the INetworkUMTS interface.

  TODO: Detailed explaination about the class here.

  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_NetworkUMTS.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-04-06 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_INetworkUMTS.h"
#include "ds_Net_Network.h"
#include "ds_Net_Platform.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Utils_ICritSect.h"

/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Net
{
   /*lint -esym(1510, INetworkUMTS) */
   /*lint -esym(1510, IQI) */
class NetworkUMTS : public INetworkUMTS
{
private:
  Network*                 mpParent;
  ICritSect*               mpICritSect;

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
  virtual ~NetworkUMTS
  (
    void
  )
  throw();

public:
  NetworkUMTS(Network* pParent);

  /*---------------------------------------------------------------------
    Inherited functions from INetworkUMTS.
  ---------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL GetIMCNFlag 
  (
    UMTSIMCNFlagType* IMCNFlag
  );


  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_NETWORK_UMTS)

  /*-------------------------------------------------------------------------
    Definitions of IQI Methods
  -------------------------------------------------------------------------*/
  DSIQI_DECL_LOCALS()
  DSIQI_ADDREF()
  DSIQI_RELEASE()
  virtual int CDECL QueryInterface (AEEIID iid, void **ppo);

}; /* class NetworkUMTS */
} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_NETWORK_UMTS */

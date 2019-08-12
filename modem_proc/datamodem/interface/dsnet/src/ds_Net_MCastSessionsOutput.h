#ifndef DS_NET_MCAST_SESSIONS_OUTPUT_H
#define DS_NET_MCAST_SESSIONS_OUTPUT_H
#ifdef FEATUTE_DATA_PS_MCAST
/*===========================================================================
  @file ds_Net_MCastSessionsOutput.h

  This file defines the class that implements the IMCastSessionsOutput interface.

  The MCastSessionsOutput class (ds::Net::MCastSessionsOutput)
  implements the following interfaces:
  IQI
  IMCastSessionsOutput

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_Utils.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_IMCastSessionsOutput.h"
#include "ds_Net_MemManager.h"
#include "ds_Utils_List.h"
#include "ds_Errors_Def.h"

/*===========================================================================
                     FORWARD DECLERATION
===========================================================================*/


/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Net
{
/*lint -esym(1510, IMCastSessionsOutput) */
/*lint -esym(1510, IQI) */
class MCastSessionsOutput : public IMCastSessionsOutput
{
public:

  /*!
  @brief
  Constructor of MCastSessionsOutput object.
  */
  MCastSessionsOutput();

  /*!
  @brief
  Destructor of MCastSessionsOutput object.
  */
  virtual void Destructor
  (
    void
  )
  throw();

  /*!
  @brief
  Dummy destructor. Only used to free memory.

  @details
  For objects that support weak references, destruction of the object and
  freeing up of memory can happen at different times. Hence Destructor()
  is the one which performs actual destruction of the object, where as
  ~Object() would perform memory cleanup.
  @params     None.
  @return     None.
  */
  virtual ~MCastSessionsOutput() throw();


  /*-------------------------------------------------------------------------
    Inherited functions from IMCastSessionsOutput.
  -------------------------------------------------------------------------*/
  /**
  @brief
  This function provides the Nth IMCastSession object of the bundle.

  @details
  The order of the objects corresponds to the order of MCast addresses
  provided to IMCastManager::JoinBundle.

  @param[out] mcastSession - The Nth Mcast session object of the bundle.
  @param[in] index - number of Session in the bundle. Zero based.

  @retval ds::SUCCESS The Nth Mcast session is successfully provided.
  @retval Other ds designated error codes might be returned.

  @see ds_Errors_Def.idl.
  @See IMCastManager::JoinBundle.
  */
  virtual ds::ErrorType CDECL GetNth
  (
    int               index,
    IMCastSession **  mcastSession
  );

  /**
  @brief
  This function provides number of elements in the bundle.

  @param[out] numElements - number of elements in the bundle.

  @retval ds::SUCCESS The Nth Mcast session is successfully provided.
  @retval Other ds designated error codes might be returned.

  @see ds_Errors_Def.idl.
  @See IMCastManager::JoinBundle.
  */
  virtual ds::ErrorType CDECL GetnumElements
  (
    int* numElements
  );

  /*-------------------------------------------------------------------------
    Internal methods.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  Adds IMCastSession to the IMCastSessionsOutput.

  @params[in] pSession - Pointer to the IMCastSession to be added.

  @return     AEE_SUCCESS on SUCCESS
  @return     AEE_ENOMEMORY No memory to add the item.
  */
  ds::ErrorType AddMCastSession
  (
    IMCastSession * pSession
  );


  /*-------------------------------------------------------------------------
    Overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_MCAST_SESSIONS_OUTPUT)

  /*-------------------------------------------------------------------------
    Macro to implement IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS2(IMCastSessionsOutput)


private:
  ds::Utils::List  mMCastSessionsList;

};/* class MCastSessionsOutput */
} /* namespace Net */
} /* namespace ds */
#endif // FEATUTE_DATA_PS_MCAST
#endif /* DS_NET_MCAST_SESSIONS_OUTPUT_H */


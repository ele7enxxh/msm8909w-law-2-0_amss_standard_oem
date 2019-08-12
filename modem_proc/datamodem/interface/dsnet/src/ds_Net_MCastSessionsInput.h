#ifndef DS_NET_MCAST_SESSIONS_INPUT_H
#define DS_NET_MCAST_SESSIONS_INPUT_H
#ifdef FEATUTE_DATA_PS_MCAST
/*===========================================================================
  @file ds_Net_MCastSessionsInput.h

  This file defines the class that implements
  the IMCastSessionsInput interface.

  The MCastSessionsInput class (ds::Net::MCastSessionsInput)
  implements the following interfaces:
  IQI
  IMCastSessionsInput

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
#include "ds_Net_IMCastSessionsInput.h"
#include "ds_Net_MemManager.h"
#include "ds_Utils_List.h"
#include "ds_Errors_Def.h"

/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Net
{
/*lint -esym(1510, IMCastSessionsInput) */
/*lint -esym(1510, IQI) */
class MCastSessionsInput : public IMCastSessionsInput
{
public:

  /*!
  @brief
  Constructor of MCastSessionsInput object.
  */
  MCastSessionsInput();

  /*!
  @brief
  Destructor of QoSSecondariesInput object.
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
  virtual ~MCastSessionsInput() throw();


  /*-------------------------------------------------------------------------
    Inherited functions from IMCastSessionsInput.
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL Associate
  (
    IMCastSession* mcastSession
  );

  virtual ds::ErrorType CDECL Disassociate
  (
    IMCastSession* mcastSession
  );

  /*-------------------------------------------------------------------------
    Internal methods.
  -------------------------------------------------------------------------*/
  /**
  @brief
  This function provides the Nth IMCastSession object of the bundle.

  @param[out] mcastSession - The Nth Mcast session object of the bundle.
  @param[in] index - number of Session in the bundle. Zero based.

  @retval ds::SUCCESS The Nth Mcast session is successfully provided.
  @retval Other ds designated error codes might be returned.

  @see ds_Errors_Def.idl.
  @See IMCastManager::JoinBundle.
  */
  ds::ErrorType GetNth
  (
    int               index,
    IMCastSession **  mcastSession
  );


  /*!
  @brief
  Provides the number of sessions in IMCastSessionsInput.

  @params[out] sessionsNum - pointer to retrieved number
                             of sessions

  @return     AEE_SUCCESS on SUCCESS
  @return     QDS_EFAULT on null arguments.
  */
  ds::ErrorType GetNumOfSessions
  (
    int32 *  sessionsNum
  );

  /*-------------------------------------------------------------------------
    Overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_MCAST_SESSIONS_INPUT)

  /*-------------------------------------------------------------------------
    Macro to implement IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS2(IMCastSessionsInput)

private:
  ds::Utils::List  mMCastSessionsList;

};/* class MCastSessionsInput */
} /* namespace Net */
} /* namespace ds */
#endif // FEATUTE_DATA_PS_MCAST
#endif /* DS_NET_MCAST_SESSIONS_INPUT_H */


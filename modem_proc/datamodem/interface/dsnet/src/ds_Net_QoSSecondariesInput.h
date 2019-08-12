#ifndef DS_NET_QOS_SECONDARIES_INPUT_H
#define DS_NET_QOS_SECONDARIES_INPUT_H

/*===========================================================================
  @file QoSSecondariesInput.h

  This file defines the class that implements the IQoSSecondariesInput interface.

  The QoSSecondariesInput class (ds::Net::QoSSecondariesInput) implements the following interfaces:
  IQI
  IQoSSecondariesInput

  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
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
#include "ds_Net_IQoSSecondariesInput.h"
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
/*lint -esym(1510, IQoSSecondariesInput) */
/*lint -esym(1510, IQI) */
class QoSSecondariesInput : public IQoSSecondariesInput
{
private:

public:

  /*!
  @brief
  Constructor of QoSSecondariesInput object.
  */
  QoSSecondariesInput();
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
  virtual ~QoSSecondariesInput() throw();


  /*-------------------------------------------------------------------------
    Inherited functions from IQoSSecondariesInput.
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL Associate
  (
    ::ds::Net::IQoSSecondary* qosSecondary
  );

  virtual ds::ErrorType CDECL Disassociate
  (
    ::ds::Net::IQoSSecondary* qosSecondary
  );


  /*-------------------------------------------------------------------------
  Internal methods.
  -------------------------------------------------------------------------*/
  /**
  @brief
  This function provides the Nth IQoSSecondary object of the bundle.

  @param[out] qoSSecondary - The Nth QoS secondary object.
  @param[in] index - number of Sessions in the bundle. Zero based.

  @retval ds::SUCCESS The Nth QoS secondary session is successfully provided.
  @retval Other ds designated error codes might be returned.

  @see ds_Errors_Def.idl.
  */
  ds::ErrorType GetNth
  (
    int index,
    IQoSSecondary ** qoSSecondary
  );


  /*!
  @brief
  Provides the number of sessions in QoSSecondariesInput.

  @params[out] sessionsNum - pointer to retrieved number
  of sessions

  @return     AEE_SUCCESS on SUCCESS
  @return     QDS_EFAULT on null arguments.
  */
  ds::ErrorType GetNumElements
  (
    int * sessionsNum
  );

  /*-------------------------------------------------------------------------
    Overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_QOS_SECONDARIES_INPUT)

  /*-------------------------------------------------------------------------
  Macro to implement IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS2(IQoSSecondariesInput);

private:
  ds::Utils::List  mQoSSecondariesList;

};/* class QoSSecondariesInput */
} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_QOS_SECONDARIES_INPUT_H */


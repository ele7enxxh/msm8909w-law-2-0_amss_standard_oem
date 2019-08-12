#ifndef DS_NET_QOS_SECONDARIES_OUTPUT_H
#define DS_NET_QOS_SECONDARIES_OUTPUT_H

/*===========================================================================
  @file QoSSecondariesOutput.h

  This file defines the class that implements the IQoSSecondariesOutput interface.

  The QoSSecondariesOutput class (ds::Net::QoSSecondariesOutput) implements the following interfaces:
  IQI
  IQoSSecondariesOutput

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
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
#include "ds_Net_IQoSSecondariesOutput.h"
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
/*lint -esym(1510, IQoSSecondariesOutput) */
/*lint -esym(1510, IQI) */
class QoSSecondariesOutput : public IQoSSecondariesOutput
{
public:

  /*!
  @brief
  Constructor of QoSSecondariesOutput object.
  */
  QoSSecondariesOutput();

  /*!
  @brief
  Destructor of QoSSecondariesOutput object.
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
  virtual ~QoSSecondariesOutput() throw();


  /*-------------------------------------------------------------------------
    Inherited functions from IQoSSecondariesOutput.
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL GetNth
  (
    int index,  ::ds::Net::IQoSSecondary** qosSecondary
  );

  virtual ds::ErrorType CDECL GetnumElements
  (
    int* numElements
  );

  /*-------------------------------------------------------------------------
  Internal methods.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  Adds QoS Secondary to the QoSSecondariesOutput.

  @params[in] qosSecondary - Pointer to the IQoSSecondary to be added.

  @return     AEE_SUCCESS on SUCCESS
  @return     AEE_ENOMEMORY No memory to add the item.
  */
  ds::ErrorType AddQoSSecondary
  (
    IQoSSecondary* qosSecondary
  );

  /*-------------------------------------------------------------------------
    Overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_QOS_SECONDARIES_OUTPUT)

  /*-------------------------------------------------------------------------
  Macro to implement IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS2(IQoSSecondariesOutput);

private:
   ds::Utils::List  mQoSSecondariesList;

};/* class QoSSecondariesOutput */
} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_QOS_SECONDARIES_OUTPUT_H */


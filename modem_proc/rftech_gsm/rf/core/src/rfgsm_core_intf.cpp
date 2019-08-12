/*!
   @file
   rfgsm_core_intf.cpp

   @brief

   @details

*/

/*===========================================================================
Copyright (c) 2008 - 2014 by Qualcomm Technologies Incorporated. 
All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_intf.cpp#1 $
  $DateTime: 2016/12/13 07:59:51 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/31/14   sw/sc   Initial version

===========================================================================*/
#include "rfcom.h"
#include "msg.h"

#include "rfgsm_core_intf.h"
#include "rfgsm_core_state_machine.h"
#include "rfm_device_types.h"



/*===========================================================================*/
/*                                                                           */
/*                                MACROS                                     */
/*                                                                           */
/*===========================================================================*/



/*===========================================================================*/
/*                                                                           */
/*                              DEFINTIONS                                   */
/*                                                                           */
/*===========================================================================*/



/*===========================================================================*/
/*                                                                           */
/*                             DECLARATIONS                                  */
/*                                                                           */
/*===========================================================================*/



/*===========================================================================*/
/*                                                                           */
/*                          FUNCTION PROTOTYPES                              */
/*                                                                           */
/*===========================================================================*/



/*===========================================================================*/
/*                                                                           */
/*                         FUNCTION DEFINITIONS                              */
/*                                                                           */
/*===========================================================================*/
/*! @fn rfgsm_core_intf_state_machine_init                                   */
/*! @brief initialise the state machine for all subscriptions                */
/*! @details This API should be called once in rfgsm_mc_init                 */
/*! @param void                                                              */
/*! @retval boolean - success flag                                           */
boolean rfgsm_core_intf_state_machine_init()
{
  RfgsmCoreStateMachineInit();
  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_intf_state_machine_destroy                                */
/*! @brief destory the state machines for all subscriptions                  */
/*! @details This API should be called once in rfgsm_mc_deinit               */
/*! @param void                                                              */
/*! @retval boolean - success flag                                           */
boolean rfgsm_core_intf_state_machine_destroy()
{
  RfgsmCoreStateMachineDelete();
  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_intf_state_machine_drive                                  */
/*! @brief Drive the state machine to manage states and transitions          */
/*! @details The API gets the subscription-specific state machine object and */
/*!          tries to drive the state machine with an input.                 */
/*!          All checks and errors and checks are handled in the state       */
/*!          machine module. An invalid state transition will return FALSE   */
/*!          here.                                                           */
/*! @param sub_id - subscription of the state machine to drive               */
/*! @param rfm_dev - RFM device ID to drive the state machine based on       */
/*! @param input - the input to drive the state machine                      */
/*! @retval boolean - success/failure of state machine                       */
boolean rfgsm_core_intf_state_machine_drive( uint8 sub_id,
                                             rfm_device_enum_type rfm_dev,
                                             rfgsmCoreStateMachineInput input )
{
  return GetCRfgsmCoreStateMachine(sub_id)->RfgsmCoreStateMachineDrive(rfm_dev, input);
}



/*===========================================================================*/
/*! @fn rfgsm_core_intf_state_machine_commit                                 */
/*! @brief commit the state to the state machine and the core handle         */
/*! @details If the RF GSM API was successful we can say we are successfully */
/*!          in that new state. If the APIs failed then we have to revert the*/
/*!          state because we have not successfully transitioned even if the */
/*!          call-flow was valid.                                            */
/*! @param sub_id - subscription of the state machine to drive               */
/*! @param rfm_dev - RFM device ID to drive the state machine based on       */
/*! @param is_success - if the RF GSM APIs were successful                   */
/*! @retval boolean - success/failure of state machine                       */
boolean rfgsm_core_intf_state_machine_commit( uint8 sub_id,
                                              rfm_device_enum_type rfm_dev,
                                              boolean rf_api_success )
{
  return GetCRfgsmCoreStateMachine(sub_id)->RfgsmCoreStateMachineCommitState(rfm_dev, rf_api_success);
}

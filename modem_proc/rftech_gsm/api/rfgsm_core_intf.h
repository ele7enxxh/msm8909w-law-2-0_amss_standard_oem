/*!
  @file
  rfgsm_core_intf.h

  @brief
  RF GSM core state machine to manage and report on call-flow validity
*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================
                 EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_core_intf.h#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
03/11/14   sw/sc   Initial version

=============================================================================*/

#ifndef RFGSM_CORE_INTF_H
#define RFGSM_CORE_INTF_H

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "rfgsm_core_state_machine.h"


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================*/
/*                                                                           */
/*                              DEFINTIONS                                   */
/*                                                                           */
/*===========================================================================*/



/*=============================================================================

                                  FEATURES

=============================================================================*/



/*=============================================================================

                        EXTERNAL DEFINITIONS AND TYPES

=============================================================================*/



/*=============================================================================

                                  FUNCTIONS

=============================================================================*/
/*! @fn rfgsm_core_intf_state_machine_init                                   */
/*! @brief initialise the state machine for all subscriptions                */
/*! @details This API should be called once in rfgsm_mc_init                 */
/*! @param void                                                              */
/*! @retval boolean - success flag                                           */
boolean rfgsm_core_intf_state_machine_init();



/*===========================================================================*/
/*! @fn rfgsm_core_intf_state_machine_destroy                                */
/*! @brief destory the state machines for all subscriptions                  */
/*! @details This API should be called once in rfgsm_mc_deinit               */
/*! @param void                                                              */
/*! @retval boolean - success flag                                           */
boolean rfgsm_core_intf_state_machine_destroy();



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
                                             rfgsmCoreStateMachineInput input );



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
                                              boolean rf_api_success );



#ifdef __cplusplus
}
#endif

#endif /* RFGSM_CORE_INTF_H */


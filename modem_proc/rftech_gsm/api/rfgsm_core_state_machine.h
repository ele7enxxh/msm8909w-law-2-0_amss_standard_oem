/*!
  @file
  rfgsm_core_state_machine.h

  @brief
  RF GSM core state machine to manage and report on call-flow validity
*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_core_state_machine.h#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
11/26/14   sw/sc   Initial version

=============================================================================*/

#ifndef RFGSM_CORE_STATE_MACHINE_H
#define RFGSM_CORE_STATE_MACHINE_H

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"



/*===========================================================================*/
/*                                                                           */
/*                              DEFINTIONS                                   */
/*                                                                           */
/*===========================================================================*/
#define RFGSM_STATE_MACHINE_IP_HIST_BUFF_SIZE 50



/*=============================================================================

                                  FEATURES

=============================================================================*/



/*=============================================================================

                        EXTERNAL DEFINITIONS AND TYPES

=============================================================================*/
/*! @enum rfgsmCoreStateMachineState                                         */
/*! @brief definition of all the state machine states                        */
/*! @details the state machine transitions between these states or can use   */
/*!          the retain or return states to manage which state is            */
/*!          transitioned to.                                                */
typedef enum
{
  DEFAULT_STATE,
  POWER_UP_STATE,
  INITIALISED_STATE,
  IDLE_STATE,
  TX_ENABLED_STATE,
  TRAFFIC_STATE,
  IRAT_G2X_STATE,
  IRAT_X2G_STATE,
  QTA_X2G_STATE,
  DED_HOPPING_STATE,
  SLEEP_STATE,
  STATE_MAX,

  RETAIN_STATE,
  RETURN_PREV_STATE,

  ILLEGAL = 0xFF
}rfgsmCoreStateMachineState;



/*===========================================================================*/
/*! @enum rfgsmCoreStateMachineInput                                         */
/*! @brief definition of all the state machine driving inputs                */
/*! @details the state machine can transition between states based on these  */
/*!          inputs.                                                         */
typedef enum
{
  DEFAULT_IN,
  ENTER_MODE_IN,
  EXIT_MODE_IN,
  CM_ENTER_IN,
  CM_EXIT_IN,
  MEAS_ENTER_IRAT_IN,
  MEAS_ENTER_QTA_IN,
  MEAS_EXIT_IN,
  WAKEUP_IN,
  SLEEP_IN,
  RX_BURST_IN,
  IDLE_FRAME_PROC_IN,
  SET_ANTENNA_IN,
  TX_ENABLE_IN,
  TX_DISABLE_IN,
  TX_BAND_IN,
  TX_BURST_IN,
  MAX_IN
}rfgsmCoreStateMachineInput;



/*=============================================================================

                                  FUNCTIONS

=============================================================================*/
/*! @fn RfgsmCoreStateMachineInit                                            */
/*! @brief Initialises the state machines and transition table               */
/*! @details This API is called to create a state machine instance per sub   */
/*!          and initialise the transition table which is common to all subs */
/*! @param void                                                              */
/*! @retval void                                                             */
void RfgsmCoreStateMachineInit();



/*===========================================================================*/
/*! @fn RfgsmCoreStateMachineDelete                                          */
/*! @brief Destroys the state machines if they have been created             */
/*! @details If an instance of the state machine class exists then it will be*/
/*!          destroyed per subscription. The state machine transition table  */
/*!          is also set to default.                                         */
/*! @param void                                                              */
/*! @retval void                                                             */
void RfgsmCoreStateMachineDelete();



/*=============================================================================

                       STATE MACHINE CLASS DEFINITION

=============================================================================*/
#ifdef __cplusplus
/*===========================================================================*/
/*! @class CRfgsmCoreStateMachine                                            */
/*! @brief This class defines the state machine and APIs to manipulate the   */
/*!        state.                                                            */
/*! @details The state machine is subscription based and therefore an        */
/*!          instance of this class will need to be created per subscription */
class CRfgsmCoreStateMachine
{
  private:
    /* Store the device subscription is active on */
    rfm_device_enum_type mCurrPrxRfmDev;
    rfm_device_enum_type mCurrDrxRfmDev;

    /* Store the current committed state machine state  */
    /* This is used for logging if the RF is successful */
    rfgsmCoreStateMachineState mCommittedState;

    /* Store the current state machine state */
    rfgsmCoreStateMachineState mCurrState;

    /* Store the previous state to enable easy return transition */
    rfgsmCoreStateMachineState mPrevState;

    /* Store the current and previous for possible revert if RF fails */
    rfgsmCoreStateMachineState mCurrStateSaved;
    rfgsmCoreStateMachineState mPrevStateSaved;

    /* Store a history of state machine driving inputs for debug */
    uint8                      mInBuffIdx;
    rfgsmCoreStateMachineInput mInBuff[RFGSM_STATE_MACHINE_IP_HIST_BUFF_SIZE];
    uint32                     mInCount[MAX_IN];

  public:
    /* Constructor - initialises class members to required defaults */
    CRfgsmCoreStateMachine();

    /* Destructor */
    /* Note: if inherited as base-class make this virtual */
    ~CRfgsmCoreStateMachine();

    /* Member function to drive the state machine */
    boolean RfgsmCoreStateMachineDrive( rfm_device_enum_type rfmDev,
                                        rfgsmCoreStateMachineInput input );

    /* Member function to commit the state to cmd buffer */
    boolean RfgsmCoreStateMachineCommitState( rfm_device_enum_type rfmDev, 
                                              boolean rfApiSuccess );

  private:
    /* Member function to do checks on the RFM device */
    boolean RfgsmCoreStateMachineDevCheck( rfm_device_enum_type rfmDev,
                                           rfgsmCoreStateMachineInput input );

    /* Member function to store the new state in the core handle */
    boolean RfgsmCoreStateMachineSetState( rfm_device_enum_type rfmDev,
                                           rfgsmCoreStateMachineState state );
};

#else
/* Need this for compilation if included in non-c++ context */
struct rfgsm_compilaton_type;
typedef struct rfgsm_compilaton_type CRfgsmCoreStateMachine;
#endif /* #ifdef __cplusplus */



/*===========================================================================*/
/*! @class GetCRfgsmCoreStateMachine                                         */
/*! @brief This function gets the instance of the subscription-specific class*/
/*! @details This function checks the validity of the class instance and ERR */
/*!          FATALs on bad input or NULL instance showing that initialisation*/
/*!          failed which would be caught at developer regression.           */
/*!          Validity checks are within this function and therefore are not  */
/*!          required by the caller.                                         */
/*! @param subId - subscription of the instance required                     */
/*! @retval CRfgsmCoreStateMachine - instance of the class                   */
CRfgsmCoreStateMachine * GetCRfgsmCoreStateMachine( uint8 subId );



#endif /* RFGSM_CORE_STATE_MACHINE_H */


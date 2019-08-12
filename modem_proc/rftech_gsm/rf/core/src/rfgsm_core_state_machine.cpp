/*!
   @file
   rfgsm_core_state_machine.cpp

   @brief

   @details
*/

/*===========================================================================
Copyright (c) 2008 - 2014 by Qualcomm Technologies Incorporated. 
All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_state_machine.cpp#1 $
  $DateTime: 2016/12/13 07:59:51 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/26/14   sw/sc   Initial version

===========================================================================*/
#include "rfcom.h"
#include "msg.h"

#include "rfgsm_core_state_machine.h"
#include "rfgsm_core.h"
#include "rfgsm_core_handle.h"
#include "rfm_device_types.h"

#include "rflm_api_gsm.h"



/*===========================================================================*/
/*                                                                           */
/*                              DEFINTIONS                                   */
/*                                                                           */
/*===========================================================================*/
/* State machine debug messages feature */
/* Note: This must be commented out when state machine is switched on */
#define FEATURE_RFGSM_CORE_STATE_MACHINE_DEBUG



/* Ensure the max subscriptions reflects the system capabilities             */
/* To future-proof the max subscriptions is one larger than the SIM number.  */
/* The definitions are used as indexes and therefore the value is one lower  */
/* than the SIM number (hence +1 for the max value).                         */
/* Note: Potential to move out to common location for all RF GSM code to use */
#if defined ( FEATURE_TRIPLE_SIM )
  #define RFGSM_CORE_MAX_SUBSCRIPTIONS (RFLM_GSM_SUBSCRIPTION_3+1)
#else
#if defined ( FEATURE_DUAL_SIM )
  #define RFGSM_CORE_MAX_SUBSCRIPTIONS (RFLM_GSM_SUBSCRIPTION_2+1)
#else
  #define RFGSM_CORE_MAX_SUBSCRIPTIONS (RFLM_GSM_SUBSCRIPTION_1+1)
#endif
#endif



/*===========================================================================*/
/*                                                                           */
/*                                MACROS                                     */
/*                                                                           */
/*===========================================================================*/
/*! @def RFGSM_SM_DEBUG_                                                     */
/*! @brief macro used to allow clean featurisation of debug messages         */
/*! @details if FEATURE_RFGSM_CORE_STATE_MACHINE_DEBUG is defined then the   */
/*!          state machine debugging F3 messages will print, if not nothing  */
/*!          will happen                                                     */
#ifdef FEATURE_RFGSM_CORE_STATE_MACHINE_DEBUG
#define RFGSM_SM_DEBUG_MSG(msg_type, msg) \
                    MSG(MSG_SSID_RF, msg_type, msg);

#define RFGSM_SM_DEBUG_MSG_1(msg_type, msg, arg1) \
                    MSG_1(MSG_SSID_RF, msg_type, msg, arg1);

#define RFGSM_SM_DEBUG_MSG_2(msg_type, msg, arg1, arg2) \
                    MSG_2(MSG_SSID_RF, msg_type, msg, arg1, arg2);

#define RFGSM_SM_DEBUG_MSG_3(msg_type, msg, arg1, arg2, arg3) \
                    MSG_3(MSG_SSID_RF, msg_type, msg, arg1, arg2, arg3);
#else
#define RFGSM_SM_DEBUG_MSG(msg_type, msg) {/*DO NOTHING*/}
#define RFGSM_SM_DEBUG_MSG_1(msg_type, msg, arg1) {/*DO NOTHING*/}
#define RFGSM_SM_DEBUG_MSG_2(msg_type, msg, arg1, arg2) {/*DO NOTHING*/}
#define RFGSM_SM_DEBUG_MSG_3(msg_type, msg, arg1, arg2, arg3) {/*DO NOTHING*/}
#endif



/*===========================================================================*/
/*                                                                           */
/*                             DECLARATIONS                                  */
/*                                                                           */
/*===========================================================================*/
/*! @var stateMachine                                                        */
/*! @brief store global instances of the state machine class                 */
static CRfgsmCoreStateMachine * stateMachine[RFGSM_CORE_MAX_SUBSCRIPTIONS] = 
{
  NULL
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
  ,NULL
#if defined ( FEATURE_TRIPLE_SIM )
  ,NULL
#endif
#endif
};



/*===========================================================================*/
/*! @var rfgsmCoreStateMachineTransitions                                    */
/*! @brief store the transition table that is only read from after           */
/*!        initialisation                                                    */
/*! @details this table defines the valid inputs per state where ILLEGAL is  */
/*!          the value of an invalid state input                             */
static rfgsmCoreStateMachineState rfgsmCoreStateMachineTransitions[STATE_MAX][MAX_IN];



/*===========================================================================*/
/*                                                                           */
/*                          FUNCTION PROTOTYPES                              */
/*                                                                           */
/*===========================================================================*/
/*! @fn RfgsmCoreStateTransitionInit                                         */
/*! @brief prototype of the transition table initialisation function         */
static void RfgsmCoreStateTransitionInit( void );



/*===========================================================================*/
/*                                                                           */
/*                         FUNCTION DEFINITIONS                              */
/*                                                                           */
/*===========================================================================*/
/*! @fn RfgsmCoreStateTransitionInit                                         */
/*! @brief initialise the transistion table                                  */
/*! @details this table defines the valid inputs per state where ILLEGAL is  */
/*!          the value of an invalid state input                             */
static void RfgsmCoreStateTransitionInit( void )
{
  uint32 i, j;

  /* Initialise the transitions table */
  /* All are invalid unless explicitly set to a valid state transition */
  for(i=0; i<(uint32)STATE_MAX; i++)
  {
    for(j=0; j<(uint32)MAX_IN; j++)
    {
      rfgsmCoreStateMachineTransitions[i][j] = ILLEGAL;
    }
  }

  /* ------------------------- INITIALISED_STATE --------------------------- */
  /* Input                    Resultant State                                */
  /* ENTER_MODE_IN       ->   IDLE_STATE                                     */
  /* CM_ENTER_IN         ->   IRAT_X2G_STATE                                 */
  rfgsmCoreStateMachineTransitions[INITIALISED_STATE][ENTER_MODE_IN] = IDLE_STATE;
  rfgsmCoreStateMachineTransitions[INITIALISED_STATE][CM_ENTER_IN]   = IRAT_X2G_STATE;


  /* ----------------------------- IDLE_STATE ------------------------------ */
  /* Input                    Resultant State                                */
  /* RX_BURST_IN         ->   No transition/retain current state             */
  /* TX_ENABLE_IN        ->   TX_ENABLED_STATE                               */
  /* SLEEP_IN            ->   SLEEP_STATE                                    */
  /* MEAS_ENTER_IRAT_IN  ->   IRAT_G2X_STATE                                 */
  /* MEAS_ENTER_QTA_IN   ->   QTA_X2G_STATE                                  */
  /* EXIT_MODE_IN        ->   INITIALISED_STATE                              */
  /* SET_ANTENNA_IN      ->   No transition/retain current state             */
  rfgsmCoreStateMachineTransitions[IDLE_STATE][RX_BURST_IN]        = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[IDLE_STATE][TX_ENABLE_IN]       = TX_ENABLED_STATE;
  rfgsmCoreStateMachineTransitions[IDLE_STATE][SLEEP_IN]           = SLEEP_STATE;
  rfgsmCoreStateMachineTransitions[IDLE_STATE][MEAS_ENTER_IRAT_IN] = IRAT_G2X_STATE;
  rfgsmCoreStateMachineTransitions[IDLE_STATE][MEAS_ENTER_QTA_IN]  = QTA_X2G_STATE;
  rfgsmCoreStateMachineTransitions[IDLE_STATE][EXIT_MODE_IN]       = INITIALISED_STATE;
  rfgsmCoreStateMachineTransitions[IDLE_STATE][SET_ANTENNA_IN]     = RETAIN_STATE;


  /* ---------------------------- SLEEP_STATE ------------------------------ */
  /* Input                    Resultant State                                */
  /* WAKEUP_IN           ->   IDLE_STATE                                     */
  /* EXIT_MODE_IN        ->   INITIALISED_STATE                              */
  rfgsmCoreStateMachineTransitions[SLEEP_STATE][WAKEUP_IN]    = IDLE_STATE;
  rfgsmCoreStateMachineTransitions[SLEEP_STATE][EXIT_MODE_IN] = INITIALISED_STATE;


  /* ------------------------- TX_ENABLED_STATE ---------------------------- */
  /* Input                    Resultant State                                */
  /* RX_BURST_IN         ->   No transition/retain current state             */
  /* TX_BAND_IN          ->   TRAFFIC_STATE                                  */
  /* TX_DISABLE_IN       ->   IDLE_STATE                                     */
  /* SET_ANTENNA_IN      ->   No transition/retain current state @TODO IS THIS POSSIBLE? */
  rfgsmCoreStateMachineTransitions[TX_ENABLED_STATE][RX_BURST_IN]    = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[TX_ENABLED_STATE][TX_BAND_IN]     = TRAFFIC_STATE;
  rfgsmCoreStateMachineTransitions[TX_ENABLED_STATE][TX_DISABLE_IN]  = IDLE_STATE;
  rfgsmCoreStateMachineTransitions[TX_ENABLED_STATE][SET_ANTENNA_IN] = RETAIN_STATE;


  /* --------------------------- TRAFFIC_STATE ----------------------------- */
  /* Input                    Resultant State                                */
  /* RX_BURST_IN         ->   No transition/retain current state             */
  /* TX_BURST_IN         ->   No transition/retain current state             */
  /* TX_BAND_IN          ->   No transition/retain current state             */
  /* TX_DISABLE_IN       ->   IDLE_STATE                                     */
  /* MEAS_ENTER_IRAT_IN  ->   IRAT_G2X_STATE                                 */
  /* SET_ANTENNA_IN      ->   No transition/retain current state             */
  /* IDLE_FRAME_PROC_IN  ->   No transition/retain current state             */
  /* ENTER_MODE_IN       ->   DED_HOPPING_STATE (only if new device)         */
  rfgsmCoreStateMachineTransitions[TRAFFIC_STATE][RX_BURST_IN]        = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[TRAFFIC_STATE][TX_BURST_IN]        = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[TRAFFIC_STATE][TX_BAND_IN]         = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[TRAFFIC_STATE][TX_DISABLE_IN]      = IDLE_STATE;
  rfgsmCoreStateMachineTransitions[TRAFFIC_STATE][MEAS_ENTER_IRAT_IN] = IRAT_G2X_STATE;
  rfgsmCoreStateMachineTransitions[TRAFFIC_STATE][SET_ANTENNA_IN]     = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[TRAFFIC_STATE][IDLE_FRAME_PROC_IN] = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[TRAFFIC_STATE][ENTER_MODE_IN]      = DED_HOPPING_STATE;


  /* ------------------------- DED_HOPPING_STATE --------------------------- */
  /* Input                    Resultant State                                */
  /* TX_BURST_IN         ->   No transition/retain current state             */
  /* RX_BURST_IN         ->   No transition/retain current state             */
  /* TX_BAND_IN          ->   No transition/retain current state             */
  /* TX_ENABLE_IN        ->   No transition/retain current state (only if new device) */
  /* TX_DISABLE_IN       ->   No transition/retain current state (only if old device) */
  /* SET_ANTENNA_IN      ->   No transition/retain current state             */
  /* IDLE_FRAME_PROC_IN  ->   No transition/retain current state             */
  /* EXIT_MODE_IN        ->   TRAFFIC_STATE (only if old device)             */
  rfgsmCoreStateMachineTransitions[DED_HOPPING_STATE][TX_BURST_IN]        = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[DED_HOPPING_STATE][RX_BURST_IN]        = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[DED_HOPPING_STATE][TX_BAND_IN]         = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[DED_HOPPING_STATE][TX_ENABLE_IN]       = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[DED_HOPPING_STATE][TX_DISABLE_IN]      = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[DED_HOPPING_STATE][SET_ANTENNA_IN]     = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[DED_HOPPING_STATE][IDLE_FRAME_PROC_IN] = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[DED_HOPPING_STATE][EXIT_MODE_IN]       = TRAFFIC_STATE;


  /* --------------------------- IRAT_G2X_STATE ---------------------------- */
  /* Input                    Resultant State                                */
  /* MEAS_EXIT_IN        ->   Return to previously occupied state            */
  rfgsmCoreStateMachineTransitions[IRAT_G2X_STATE][MEAS_EXIT_IN] = RETURN_PREV_STATE;


  /* --------------------------- IRAT_X2G_STATE ---------------------------- */
  /* Input                    Resultant State                                */
  /* RX_BURST_IN         ->   No transition/retain current state             */
  /* MEAS_ENTER_IRAT_IN  ->   No transition/retain current state             */
  /* MEAS_EXIT_IN        ->   No transition/retain current state             */
  /* CM_EXIT_IN          ->   INITIALISED_STATE                              */
  rfgsmCoreStateMachineTransitions[IRAT_X2G_STATE][RX_BURST_IN]        = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[IRAT_X2G_STATE][MEAS_ENTER_IRAT_IN] = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[IRAT_X2G_STATE][MEAS_EXIT_IN]       = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[IRAT_X2G_STATE][CM_EXIT_IN]         = INITIALISED_STATE;


  /* ---------------------------- QTA_X2G_STATE ---------------------------- */
  /* Input                    Resultant State                                */
  /* RX_BURST_IN         ->   No transition/retain current state             */
  /* MEAS_EXIT_IN        ->   IDLE_STATE                                     */
  rfgsmCoreStateMachineTransitions[QTA_X2G_STATE][RX_BURST_IN]   = RETAIN_STATE;
  rfgsmCoreStateMachineTransitions[QTA_X2G_STATE][MEAS_EXIT_IN]  = IDLE_STATE;
}



/*===========================================================================*/
/*! @fn RfgsmCoreStateMachineInit                                            */
/*! @brief Initialises the state machines and transition table               */
/*! @details This API is called to create a state machine instance per sub   */
/*!          and initialise the transition table which is common to all subs */
/*! @param void                                                              */
/*! @retval void                                                             */
void RfgsmCoreStateMachineInit()
{
  /* Initialise local variables */
  uint32 sub_index = 0;

  /* Initialise the transition table */
  RfgsmCoreStateTransitionInit();

  /* Create new state machine instances per subscription */
  for( sub_index = 0; sub_index < RFGSM_CORE_MAX_SUBSCRIPTIONS; sub_index++ )
  {
    if(stateMachine[sub_index] == NULL) 
    {
      stateMachine[sub_index] = new CRfgsmCoreStateMachine(); 
    }
  }
}



/*===========================================================================*/
/*! @fn RfgsmCoreStateMachineDelete                                          */
/*! @brief Destroys the state machines if they have been created             */
/*! @details If an instance of the state machine class exists then it will be*/
/*!          destroyed per subscription. The state machine transition table  */
/*!          is also set to default.                                         */
/*! @param void                                                              */
/*! @retval void                                                             */
void RfgsmCoreStateMachineDelete()
{
  /* Initialise local variables */
  uint32 sub_index = 0;

  /* If the state machine for subscription x is NULL then nothing to delete  */
  /* else delete the state machine instance.                                 */
  for( sub_index = 0; sub_index < RFGSM_CORE_MAX_SUBSCRIPTIONS; sub_index++ )
  {
    if(stateMachine[sub_index] != NULL) 
    {
      delete stateMachine[sub_index];
    }
  }
}



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
CRfgsmCoreStateMachine * GetCRfgsmCoreStateMachine( uint8 subId )
{
  /* Check the subscription input argument and err fatal if not valid catching */
  /* critical errors and preventing exceptions.                                */
  if( (rflm_gsm_subscription_id)subId < RFGSM_CORE_MAX_SUBSCRIPTIONS )
  {
    /* If no instance exists err fatal because we cannot continue */
    if( stateMachine[subId] == NULL )
    {
      ERR_FATAL("RF GSM state machine instance doesn't exist for sub %d", subId, 0, 0);
    }
  }
  else
  {
    ERR_FATAL("RF GSM state machine invalid sub ID %d, max subs %d", subId, RFGSM_CORE_MAX_SUBSCRIPTIONS, 0);
  }

  /* Return the instance of the state machine class */
  return stateMachine[subId];
}



/*===========================================================================*/
/*! @fn CRfgsmCoreStateMachine                                               */
/*! @brief State machine class constructor                                   */
/*! @details Constructor initialises private variables to required defaults  */
/*! @param void                                                              */
/*! @retval void - no ret val from constructor                               */
CRfgsmCoreStateMachine::CRfgsmCoreStateMachine()
{
  /* Initialise state data*/
  mCurrPrxRfmDev = RFM_MAX_WAN_DEVICES;
  mCurrDrxRfmDev = RFM_MAX_WAN_DEVICES;

  mCommittedState = INITIALISED_STATE;
  mCurrState      = INITIALISED_STATE;
  mCurrStateSaved = INITIALISED_STATE;
  mPrevState      = POWER_UP_STATE;
  mPrevStateSaved = POWER_UP_STATE;

  /* Initialise state machine input logging */
  mInBuffIdx = 0;
  memset(&mInBuff,  0, sizeof(mInBuff));
  memset(&mInCount, 0, sizeof(mInCount));
}



/*===========================================================================*/
/*! @fn ~CRfgsmCoreStateMachine                                              */
/*! @brief State machine class destructor                                    */
/*! @details Destructor currently doesn't do anything                        */
/*! @param void                                                              */
/*! @retval void - no ret val from destructor                                */
CRfgsmCoreStateMachine::~CRfgsmCoreStateMachine()
{
}



/*===========================================================================*/
/*! @fn RfgsmCoreStateMachineDrive                                           */
/*! @brief This function validates the transitions based on current state and*/
/*!        input transition                                                  */
/*! @details This function should be called at the entry of any command proc */
/*!          function from GL1 in order to catch errors in call-flow before  */
/*!          further execution.                                              */
/*! @param subId - subscription of the state-machine to drive                */
/*! @param rfmDev - device the subscription is active on                     */
/*! @param input - Input to the state machine                                */
/*! @retval boolean - success status of the driving function                 */
boolean CRfgsmCoreStateMachine::RfgsmCoreStateMachineDrive( rfm_device_enum_type rfmDev,
                                                            rfgsmCoreStateMachineInput input )
{
  /* Initialise variables */
  boolean stateMachineSuccess = TRUE;
  rfgsmCoreStateMachineState tempState = DEFAULT_STATE;

  /* Validate the device ID */
  if ( rfmDev >= RFM_MAX_WAN_DEVICES )
  {
    RFGSM_SM_DEBUG_MSG_1( MSG_LEGACY_ERROR, 
                          "RF GSM state machine invalid device ID %d", 
                          rfmDev );
    return FALSE;
  }

  /* Validate the state machine drive input */
  if( input >= MAX_IN )
  {
    /* Error fatal to catch development errors or memory corruption */
    ERR_FATAL("RF GSM state machine driven with invalid input %d", 
              input, 0, 0);
  }

  /* Save the current and previous states for possible revert */
  mCurrStateSaved = mCurrState;
  mPrevStateSaved = mPrevState;

  /* Maintain the current device to enable scenario detection */
  if( !RfgsmCoreStateMachineDevCheck( rfmDev, input ) )
  {
    return FALSE;
  }

  /* Validate the state machine drive input */
  if( mCurrState >= STATE_MAX )
  {
    /* Error fatal to catch development errors or memory corruption */
    ERR_FATAL("RF GSM state machine transition with invalid state %d", 
              mCurrState, 0, 0);
  }

  /* Perform state transition */
  switch( rfgsmCoreStateMachineTransitions[mCurrState][input] )
  {
    /* Default all other valid state transitions */
    default:
      mPrevState = mCurrState;
      mCurrState = rfgsmCoreStateMachineTransitions[mCurrState][input];
      RFGSM_SM_DEBUG_MSG_3( MSG_LEGACY_HIGH, 
                            "RF GSM state machine valid transition to state %d "
                            "from state %d via input %d", 
                            mCurrState, mPrevState, input );
      break;

    /* Check if we retain the current state with no state transition */
    case RETAIN_STATE:
      /* No need to do anything because remaining in same state */
      RFGSM_SM_DEBUG_MSG_2( MSG_LEGACY_HIGH, 
                            "RF GSM state machine retained state %d via input %d", 
                            mCurrState, input );
      break;

    /* Check if input would result in returning to the previous state */
    case RETURN_PREV_STATE:
      /* Returning to previous state */
      RFGSM_SM_DEBUG_MSG_3( MSG_LEGACY_HIGH, 
                            "RF GSM state machine returned from state %d to "
                            "previous state %d via input %d", 
                            mCurrState, mPrevState, input );
      mCurrState = mPrevState;
      mPrevState = mCurrStateSaved;
      break;

    /* Check if transition is illegal */
    case ILLEGAL:
      RFGSM_SM_DEBUG_MSG_2( MSG_LEGACY_ERROR, 
                            "RF GSM state machine illegal transition from state"
                            " %d input %d",
                            mCurrState, input );
      stateMachineSuccess = FALSE;
      break;
  }

  /* Log to the input history buffer */
  mInBuff[mInBuffIdx] = input;

  /* Mod wrap input history buffer */
  mInBuffIdx++;
  mInBuffIdx %= RFGSM_STATE_MACHINE_IP_HIST_BUFF_SIZE;
  mInCount[input]++;

  return stateMachineSuccess;
}



/*===========================================================================*/
/*! @fn RfgsmCoreStateMachineCommitState                                     */
/*! @brief This function sets the new state in the state machine and core    */
/*!        handle                                                            */
/*! @details If the driving function was successful the state will be updated*/
/*!          in the state machine object and */
/*! @param */
/*! @retval */
boolean CRfgsmCoreStateMachine::RfgsmCoreStateMachineCommitState( rfm_device_enum_type rfmDev, 
                                                                  boolean rfApiSuccess )
{
  /* Validate the device ID */
  if ( rfmDev >= RFM_MAX_WAN_DEVICES )
  {
    RFGSM_SM_DEBUG_MSG_1( MSG_LEGACY_ERROR, 
                          "RF GSM state machine commit invalid device ID %d", 
                          rfmDev );
    return FALSE;
  }

  /* If RF API was successful we can transition to new state, else we cannot */
  /* and we must revert to the previous state.                               */
  if( rfApiSuccess == TRUE )
  {
    /* Set the committed state */
    mCommittedState = mCurrState;
    if( !RfgsmCoreStateMachineSetState( rfmDev, mCommittedState ) )
    {
      return FALSE;
    }
  }
  else
  {
    /* revert to previous states */
    mCurrState = mCurrStateSaved;
    mPrevState = mPrevStateSaved;
  }

  return TRUE;
}



/*===========================================================================*/
/*! @fn RfgsmCoreStateMachineSetState                                        */
/*! @brief This function sets the new state in the core handle               */
/*! @details If the core handle is present for the driving device the current*/
/*!          state is set for logging and debug within the command buffer    */
/*! @param rfmDev - RFM device ID                                            */
/*! @param state - new state machine state                                   */
/*! @retval boolean - success flag                                           */
boolean CRfgsmCoreStateMachine::RfgsmCoreStateMachineSetState( rfm_device_enum_type rfmDev,
                                                               rfgsmCoreStateMachineState state )
{
  /* Initialise variables */
  rfgsm_core_handle_type * rfgsmCoreHandle = rfgsm_core_handle_get(rfmDev);

  /* If core handle is not NULL then update the state of the current device */
  if( rfgsmCoreHandle != NULL )
  {
    rfgsmCoreHandle->curr_state = (uint8)state;
    return TRUE;
  }

  return FALSE;
}



/*===========================================================================*/
/*! @fn RfgsmCoreStateMachineDevCheck                                        */
/*! @brief This function checks the transition from a device perspective     */
/*! @details Device ID is managed to determine more scenarios such as WTR    */
/*!          hopping.                                                        */
/*! @param rfmDev - RFM device ID                                            */
/*! @param input - driving input                                             */
/*! @retval boolean - success flag                                           */
boolean CRfgsmCoreStateMachine::RfgsmCoreStateMachineDevCheck( rfm_device_enum_type rfmDev,
                                                               rfgsmCoreStateMachineInput input )
{
  /*-------------------------------------------------------------------------*/
  /* Store the device ID                                                     */
  if( ((mCurrState == INITIALISED_STATE) && (input == ENTER_MODE_IN)) ||
      ((mCurrState == INITIALISED_STATE) && (input == CM_ENTER_IN)) )
  {
    /* Only store the device if it has changed */
    mCurrPrxRfmDev = rfmDev;
  }
  else if( (mCurrState == SLEEP_STATE) && (input == WAKEUP_IN) )
  {
    /* Only store the device if it has changed */
    if( mCurrPrxRfmDev != rfmDev )
    {
      /*@TODO possibly print to debug*/
      mCurrPrxRfmDev = rfmDev;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* Could be going into dedicated hopping mode                              */
  else if ((mCurrState == TRAFFIC_STATE) && (input == ENTER_MODE_IN)) 
  {
    /* Enter mode from traffic is only valid if it is for a different device */
    /* in which case we are performing dedicated hopping.                    */
    if( mCurrPrxRfmDev == rfmDev )
    {
      RFGSM_SM_DEBUG_MSG_2( MSG_LEGACY_ERROR, 
                            "RF GSM state machine enter mode in traffic state "
                            "not valid when (curr dev %d == in-dev %d)", 
                            mCurrPrxRfmDev, rfmDev );
      /* Return because do not want to enter ded hopping state as this is an */
      /* error condition */
      return FALSE;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* Switching device in dedicated hopping mode                              */
  else if( (mCurrState == DED_HOPPING_STATE) && (input == TX_ENABLE_IN) )
  {
    /* Only store the device if it has changed */
    if( mCurrPrxRfmDev != rfmDev )
    {
      mCurrPrxRfmDev = rfmDev;
    }
    else
    {
      RFGSM_SM_DEBUG_MSG_2( MSG_LEGACY_ERROR,
                            "RF GSM state machine ded hopping Tx enable invalid "
                            "for curr dev %d, in-dev %d",
                            mCurrPrxRfmDev, rfmDev );
      return FALSE;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* Deactivating the device we have hopped from in dedicated hopping mode   */
  else if( ((mCurrState == DED_HOPPING_STATE) && (input == TX_DISABLE_IN)) ||
           ((mCurrState == DED_HOPPING_STATE) && (input == EXIT_MODE_IN)) )
  {
    if( mCurrPrxRfmDev == rfmDev )
    {
      RFGSM_SM_DEBUG_MSG_3( MSG_LEGACY_ERROR, 
                            "RF GSM state machine ded hopping sub %d input %d "
                            "invalid for curr dev %d, in-dev %d",
                            input, mCurrPrxRfmDev, rfmDev );
      return FALSE;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* All other cases we would expect the device to be the same               */
  else
  {
    if( mCurrPrxRfmDev != rfmDev )
    {
      RFGSM_SM_DEBUG_MSG_3( MSG_LEGACY_ERROR, 
                            "RF GSM state machine invalid dev %d for input %d "
                            "where curr dev is %d",
                            rfmDev, input, mCurrPrxRfmDev );
      return FALSE;
    }
  }

  return TRUE;
}


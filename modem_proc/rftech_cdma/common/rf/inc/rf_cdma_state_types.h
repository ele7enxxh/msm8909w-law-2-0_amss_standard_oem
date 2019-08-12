#ifndef RF_CDMA_STATE_TYPES_H
#define RF_CDMA_STATE_TYPES_H

/*!
  @file
  rf_cdma_state_types.h

  @details
  This file exports the 1xHDR State Types.
*/

/*==============================================================================

  Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_state_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/16/11   Saul    IRAT. D2L HDR Stop Rx sequence change to delay premature 
                   RF sleep/vreg shutdown which affects LTE measurements.
09/07/11   aro     Added CDMA state to indicate scheduled sleep
06/07/11   Saul    Updates for multi-carrier driver.
06/02/11   sty     Assigned numbers to enum for better readability
04/11/11   aro     Added Rx Prepare State
01/15/11   aro     Added Invalid RF State
12/17/10   aro     Updated CDMA RF State Enum
12/08/10   aro     Fixing preprocessor Macros
12/06/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#include "db.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF CDMA State Type Enumeration.
*/
typedef enum
{

  RF_CDMA_STATE_SLEEP = 0,
  /*!< This state is entered when CDMA is in Sleep State. During this state 
  CDMA software is in reset state and there should be no CDMA related 
  activitied on Hardware. This will be entered when Disable Rx function is 
  called or RF init is done*/
  
  RF_CDMA_STATE_RX_PREP = 1,
  /*!< This state is entered when Rx is prepared to be enabled and Tuned a 
  given band and channel. With this the RF Rx hardware is powered up and is 
  also setup to trigger an even to be able to receive signal from base
  station.*/

  RF_CDMA_STATE_RX = 2,
  /*!< This state is entered when Rx is completely enabled and Tuning is done on 
  a give band and channel. With this the RF Rx hardware is powered up and is 
  also setup to be able to receive signal from base station. */

  RF_CDMA_STATE_RXTX = 3,
  /*!< This state is entered after Tx is woken up. At the state, the phone 
  should be able to perform receive as well as transmit on given band and 
  channel. */

  RF_CDMA_STATE_RXDIV = 4,
  /*!< This state indicates that the device is in Receive Diversity state and is 
  acting as a slave from another primary device. */

  RF_CDMA_STATE_SLEEP_SCHEDULED = 5,
  /*!< This state is entered when CDMA scheduled to go to Sleep State after 
  some finite time in future. During this state CDMA software is in same state 
  state as Rx State. This state will be entered in the cases such as 
  timed sleep in 1x offline-QPCH */ 

  RF_CDMA_STATE_SHUTDOWN_FOR_IRAT = 6,
  /*!< This state is entered when CDMA is operating in IRAT mode which must 
    be followed by a modified Rx sleep. From this state, the only next state 
    that must follow is sleep. Not even re-tune or wake-up are allowed. */

  RF_CDMA_STATE_INVALID = 7
  /*!< This enum indicates that the state is invalid. This can only be used 
  as one of the return values in RF State Query function. */

} rf_cdma_state_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This enumeration is used to denote the carrier mode used.
*/
typedef enum
{
  RF_CDMA_CARRIER_MODE_SC, /*!< Single-Carrier Mode */
  RF_CDMA_CARRIER_MODE_MC  /*!< Multi-Carrier Mode */
} rf_cdma_carrier_mode_type;

#ifdef __cplusplus
}
#endif

#endif  /* RF_CDMA_STATE_TYPES_H */


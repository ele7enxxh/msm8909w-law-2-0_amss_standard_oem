#ifndef FTM_CDMA_STATE_TYPES_H
#define FTM_CDMA_STATE_TYPES_H

/*!
  @file
  ftm_cdma_state_types.h

  @details
  This file exports the 1xHDR State Types to be used by FTM.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/inc/ftm_cdma_state_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/06/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA State Type Enumeration.
*/
typedef enum
{

  FTM_CDMA_STATE_SLEEP,
  /*!< This state is entered when CDMA is in Sleep State. During this state 
  CDMA software is in reset state and there should be no CDMA related 
  activitied on Hardware. This will be entered when Disable Rx function is 
  called or RF init is done*/

  FTM_CDMA_STATE_RX,
  /*!< This state is entered when Rx is enabled and Tuning is done on a give 
  band and channel. With this the RF Rx hardware is powered up and is also setup 
  to be able to receive signal from base station. */

  FTM_CDMA_STATE_RXTX,
  /*!< This state is entered after Tx is woken up. At the state, the phone 
  should be able to perform receive as well as transmit on given band and 
  channel. */

  FTM_CDMA_STATE_RXDIV,
  /*!< This state indicates that the device is in Receive Diversity state and is 
  acting as a slave from another primary device. */

  FTM_CDMA_STATE_INVALID
  /*!< This enum indicates that the state is invalid. This can only be used 
  as one of the return values in RF State Query function. */

} ftm_cdma_state_type;

#ifdef __cplusplus
}
#endif

#endif  /* FTM_CDMA_STATE_TYPES_H */


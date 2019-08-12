#ifndef RFM_DEVICE_STATE_TYPES_H
#define RFM_DEVICE_STATE_TYPES_H

/*!
  @file
  Common types used in RFM APIs that deal with the state of the
  RF diver.

  @details
  This file exports the definitions of types that are used in the RFM
  interface relating to RF state parameters and returns.
*/

/*==============================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/rfm_device_state_types.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/06/10   aro     Initial Release.

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sleep/Rx/RxTx state of a path

  @details
  Used by the 1x and HDR RF driver to report the current sleep
  state of a path to upper layers.
*/
typedef enum
{
  RFM_1XDO_STATE_POWER_INIT,
  /*!< The path is powered up but has not been configured */

  RFM_1XDO_STATE_SLEEP,
  /*!< The path has been configured and placed in low Power mode */

  RFM_1XDO_STATE_RX,
  /*!< The path has been set to receive without Diversity*/

  RFM_1XDO_STATE_MC_RX,
  /*!< The path has been set to receive in multi-carrier mode without 
  Diversity*/

  RFM_1XDO_STATE_RXDIV,
  /*!< The path has been set to receive with Diversity */

  RFM_1XDO_STATE_MC_RXDIV,
  /*!< The path has been set to receive in multi-carrier mode with Diversity */

  RFM_1XDO_STATE_RXTX,
  /*!< The path has been set to receive and Transmit without Diversity*/

  RFM_1XDO_STATE_MC_RXTX,
  /*!< The path has been set to receive and Transmit in multi-carrier mode 
  without Diversity*/

  RFM_1XDO_STATE_RXDIVTX,
  /*!< The path has been set to receive and transmit with Diversity */

  RFM_1XDO_STATE_MC_RXDIVTX,
  /*!< The path has been set to receive and transmit in multi-carrier mode 
  with Diversity */

  RFM_1XDO_STATE_UNDEFINED
  /*!< The path is not initialized */

} rfm_1xdo_device_state_type;

#ifdef __cplusplus
}
#endif

#endif /* RFM_DEVICE_STATE_TYPES_H */

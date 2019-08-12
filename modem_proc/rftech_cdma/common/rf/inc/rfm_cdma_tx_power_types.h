#ifndef RFM_CDMA_TX_POWER_TYPES_H
#define RFM_CDMA_TX_POWER_TYPES_H

/*!
  @file
  Common CDMA Tx Power types to be used by RFM - FTM interfaces.

  @details
  This file exports Tx Power types requried by all FTM functions that call
  RFM APIs.
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rfm_cdma_tx_power_types.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/09/11   aro     Initial revision.

============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Lists the TxAGC Mode.
*/
typedef enum
{
  RFM_CDMA_TX_PWR_OPEN_LOOP    = 0,  /*!< Open Loop Transmit Power Mode */
  RFM_CDMA_TX_PWR_CLOSED_LOOP  = 1  /*!< Closed Loop Transmit Power Mode */ 
} rfm_cdma_tx_pwr_mode_type;

#ifdef __cplusplus
}
#endif

#endif /* RFM_CDMA_TX_POWER_TYPES_H */


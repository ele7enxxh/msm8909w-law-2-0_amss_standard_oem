/*!
  @file
  ftm_cdma_log.c

  @brief
  This module contains functions used for 1xHDR Common RF Logging

  @addtogroup RF_FTM_COMMON_LOG
  @{
*/

/*==============================================================================

  Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated. All Rights Reserved
  Copyright (c) 2013 -  Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/src/ftm_cdma_log.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
09/08/11   aro     Fixed RxAGC units
09/08/11   aro     Compiler warning fix
09/07/11   aro     Added intelliceiver logging API
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/13/10   aro     Renamed Intelliciever Logging function
12/07/10   aro     Initial Revision : Common Calibration

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_cdma_log.h"
#include "ftm_msg.h"
#include "ftm_log.h"
#include "rfm_1x_ftm.h"
#include "rfm_1x.h"
#include "rfm_cdma_ftm.h"
#include "rfm_hdr_ftm.h"
#include "rfm_hdr.h"
#include "rf_cdma_data.h"
#include "log.h"

/*============================================================================*/
/*!
  @name Intelliceiver Logging

  @brief
  This section contains the functions used for Intelliceiver Loggings
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Log Intelliceiver

  @details
  This function reports the IntelliCeiver state along with AGC and LNA
  state. Callback function, must be enabled with ftm_log_enable.

  @param interval
  Interval
*/
void
ftm_cdma_log_c0_intelliceiver_cb
(
  int32 interval
)
{
  ftm_cdma_log_intelliceiver_type *intelliceiver_log;
  rfm_device_enum_type device = RFM_DEVICE_0;
  rfm_cdma_ftm_rx_agc_data_type rx_agc_data;
  int16 rx_agc;
  const rf_cdma_data_status_type *dev_status = NULL; /* RF Dev Status */

  /* Get RF Device Status */
  dev_status = rf_cdma_get_device_status(device);

  /* Check if the device status is NULL or not */
  if ( dev_status == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_log_c0_intelliceiver_cb: Dev %d - "
               "Invalid CDMA Data", device );
    return;
  }

  /* Do not perform logging, if RF is not in either Rx or RxTx state */
  if (  !( ( dev_status->data_1x.rf_state == RF_CDMA_STATE_RX ) ||
           ( dev_status->data_1x.rf_state == RF_CDMA_STATE_RXTX ) ||
           ( dev_status->data_hdr.rf_state == RF_CDMA_STATE_RX ) ||
           ( dev_status->data_hdr.rf_state == RF_CDMA_STATE_RXTX ) ) )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_log_c0_intelliceiver_cb: Dev %d needs to "
               "to be in Rx or RxTx State", device );
    return;
  }

  /* Allocate memory for intelliceiver logging */
  intelliceiver_log = (ftm_cdma_log_intelliceiver_type *)ftm_log_malloc( 
             FTM_LOG_INTELLICEIVER, sizeof (ftm_cdma_log_intelliceiver_type) );

  /* If memory is allocated, then proceed sith logging */
  if (intelliceiver_log != NULL)
  {
    /* Based on 1x or HDR mode, call RFM APIs to get RxAGC and LNA state data */
    if ( dev_status->rf_mode == RFM_1XEVDO_MODE )
    {
      /* HDR */
      rfm_hdr_ftm_get_rx_agc_data( device, &rx_agc_data,
                                   RFM_CDMA_AGC_FORMAT__RAW );
      rx_agc = rx_agc_data.rx_agc[RFM_CDMA_CARRIER_CUMULATIVE]/64;
    }
    else if ( dev_status->rf_mode == RFM_1X_MODE )
    {
      /* 1x */
      rfm_1x_ftm_get_rx_agc_data( device, &rx_agc_data,
                                  RFM_CDMA_AGC_FORMAT__RAW );
      rx_agc = rx_agc_data.rx_agc[RFM_CDMA_CARRIER_0]/64;
    }
    else
    {
      rx_agc = 0;
      rx_agc_data.gain_state = RFM_LNA_INVALID_GAIN_STATE;
      FTM_MSG_2( FTM_ERROR, "ftm_cdma_log_c0_intelliceiver_cb: Dev %d - "
                 "Invalid RF Mode %d", device, dev_status->rf_mode );
    }

    /* Populate intelliceiver log */
    intelliceiver_log->data.rx_agc = rx_agc;
    intelliceiver_log->data.lna_state = rx_agc_data.gain_state;
    intelliceiver_log->data.power_mode = rfm_cdma_ftm_get_power_mode( device );
    intelliceiver_log->data.reserved = rfm_cdma_ftm_read_hdet( device );

    /* Commit the log */
    log_commit( intelliceiver_log );

  } /* if (intelliceiver_log) */
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_cdma_log_c0_intelliceiver_cb: Malloc Failed" );
  }
} /* ftm_cdma_log_c0_intelliceiver_cb */

/*! \} */

#endif /* FEATURE_CDMA1X */
#endif /* FEATURE_FACTORY_TESTMODE */

/*! @} */

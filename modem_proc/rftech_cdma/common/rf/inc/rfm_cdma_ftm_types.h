#ifndef RFM_CDMA_FTM_TYPES_H
#define RFM_CDMA_FTM_TYPES_H

/*!
  @file
  Common CDMA FTM types to be used by RFM - FTM interfaces.

  @details
  This file exports RFM FTM types requried by all FTM functions that call
  RFM APIs.
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rfm_cdma_ftm_types.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/14   cd      Add support for FTM AGC logging with FED and common tx AGC 
                   functionality
10/16/13   cc      Add new ET logging for FTM AGC logs
04/10/13   aro     Added AGC ACQ down counter in RxAGC data
03/28/13   zhw     Clean up obsoleted PA range in tx_agc_data
12/20/11   aro     Documentation update for APT value
12/02/11   sty     Added raw_rxagc element to rfm_cdma_ftm_rx_agc_data_type 
09/13/11   aro     Added carrier and cumulative Tx total power
08/24/11   aro     Updated Tx Data query function to have more Tx parameter
03/24/11   aro     Unified the RxAGC Read Interface
03/14/11   aro     Consolidated RxAGC Data to one data structure
03/03/11   sty     Added/renamed members in rfm_cdma_ftm_tx_agc_data_type
02/12/11   aro     Initial revision.

============================================================================*/

#include "comdef.h"
#include "rfm_gain_state_types.h"
#include "rfm_cdma_carrier_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold LNA Parameter
*/
typedef struct
{

  int16 rx_agc[ RFM_CDMA_CARRIER_NUM + 1 ];
  /*!< Current RxAGC value for all carriers. Index 0,1,2 are used for
  Carrier RxAGC and Index 3 is used for Cumulative RxAGC */
  
  int16 raw_rxagc;
  /*!< is the raw AGC equivalent of rx_agc[] (in FW units) */
    
  rfm_lna_gain_state_type gain_state;
  /*!< LNA Gain State */

  uint8 gain_word;
  /*!< RTR LNA Gain word */

  int32 acq_down_ctr; 
  /*!< AGC acquisition down counter in units of 64 chips
  The AGC moves to track mode (i.e, RxAGC has settled)  when counter = 0 */

} rfm_cdma_ftm_rx_agc_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold TxAGC Parameter
*/
typedef struct
{
  int16 open_loop_power;
  /*!< Tx Open Loop Power. The uint of value read from the interface -1/640dB
  in 102.4 dB Mode. This will be of range -32768 to +32767 */

  int16 closed_loop_power;
  /*!< Tx Closed Loop Power */

  int16 rate_adjust;
  /*!< Tx Rate Adjust */

  int16 carrier_total_power;
  /*!< Tx Total Power for a given carrier*/

  int16 total_power;
  /*!< Cumulative Tx Total Power */

  int16 pilot_power;
  /*!< Tx Pilot Power */

  uint8 pa_state;
  /*!< PA State in the STM */

  int16 tx_min;
  /*!< Tx Min Limit */

  int16 tx_max;
  /*!< Tx Max Limit */

  uint32 apt_val;
  /*!< APT value in 10-bit HW units */

  uint32 avga_val;
  /*!< Tx AVGA value */

  uint16 avga_index;
  /*!< Tx AVGA Index */

  int16 betap_gain;
  /*!< BetaPrime Value */

  uint8 xpt_mode_val;
  /*!< XPT mode 0: APT 1: EPT 2: ET */

  int16 env_gain_val;
  /*!< envelop scale gain read from HW */

  int16 iq_gain_val;
  /*!< IQ gain read from HW */

  int32 xpt_delay_val;
  /*!< envelop delay */

  uint16 pa_q_current;
  /*!< PA quiecent current setting */

  int16 pout_comp;
  /*!< Pout compensation value: temp and freq */
  
  uint16 dpd_idx;
  /*!< DPD table index */
  
  int16 pin_comp;
  /*!< Pin compensation value: temp and freq */

} rfm_cdma_ftm_tx_agc_data_type;


#ifdef __cplusplus
}
#endif

#endif /* RFM_CDMA_FTM_TYPES_H */


#ifndef FTM_CDMA_LOG_H
#define FTM_CDMA_LOG_H

/*!
  @file
  ftm_cdma_log.h

  @brief
  This module defines the function prototypes and defintions used by
  Common 1xHDR RF Logging.
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/inc/ftm_cdma_log.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/16/13  cc       Add new ET logging for FTM AGC logs
03/09/13   hdz     Added txagc_dbm10_from_hdet in log packet
11/28/12   nrk     Added beta scaling logging support
12/20/11   aro     Enabled logging for APT value
09/07/11   aro     Added intelliceiver logging API
09/07/11   aro     Added definitions for chain 1 AGC data
08/24/11   aro     Added definitions for chain 0 and chain 1 AGC data
12/13/10   aro     Renamed Intelliciever Logging function
12/07/10   aro     Initial Revision : Common Calibration

==============================================================================*/

#include "comdef.h"
#include "ftm_log.h"

/*----------------------------------------------------------------------------*/
/*! Structure to hold the device 0 Tx and Rx AGC logging data */
typedef PACK(struct)
{
  int16 rx_agc;
  /*!< Current Rx AGC */

  int16 tx_gain_ctl;
  /*!< Tx Gain control */

  int16 tx_gain_adj;
  /*!< Tx Gain Adjust */

  int16 tx_open_loop;
  /*!< Tx OpenLoop Power */

  int16 tx_agc_adj;
  /*!< Tx AGC adjust */

  uint8 pa_ratchet;
  /*!< PA Ratchet */

  uint8 lna_state;
  /*!< Current LNA state */

  uint8 pa_state;
  /*!< Current PA State */

  uint8 hdet_raw;
  /*!< Raw HDET */

  uint16 therm_raw;
  /*!< Raw Therm */

  uint8 therm_scaled;
  /*!< Scaled Therm */

  uint8 temp_index;
  /*!< Temperature index */

  uint8 temp_remainder;
  /*!< Temperature remainder */

  uint8 intelliceiver_state;
  /*!< Current intelliceiver state */

  uint16 hdet_raw_v2;
  /*!< Raw HDET */

  uint16 pa_smps_pdm;
  /*!< PA SMPS Bias Value in 10-bit HW units */

  int16 betap_gain;
  /*!< BetaPrime Value */

  int16 txagc_from_hdet;
  /*!< txagc in dbm10 converted from hdet read*/

  uint8 xpt_mode;
  /*!< xpt mode */

  int16 pin_comp;
  /*!< Pin compensation value */

  int16 pout_comp;
  /*!< Pout compensation value */

  int16 env_scale_backoff;
  /*!< envelop scale backoff in unit of 10th dB */
  
  int16 iq_backoff;
  /*!< iq gain backoff in unit of 10th dB */

  int16 xpt_delay;
  /*!< xpt delay ns*10 unit */

  uint16 dpd_index;
  /*!< dpd index */
  
  uint16 pa_icq;
  /*!< PA quiescent current setting */

} ftm_cdma_c0_agc_type;

/*----------------------------------------------------------------------------*/
/*!< Structure to hold the device 1 Tx and Rx AGC logging data */
typedef PACK(struct)
{
  int16 rx_agc;
  /*!< Current Rx AGC */

  int16 tx_gain_ctl;
  /*!< Tx Gain control */

  int16 tx_gain_adj;
  /*!< Tx Gain Adjust */

  int16 tx_open_loop;
  /*!< Tx OpenLoop Power */

  int16 tx_agc_adj;
  /*!< Tx AGC adjust */

  uint8 pa_ratchet;
  /*!< PA Ratchet */

  uint8 lna_state;
  /*!< Current LNA state */

  uint8 pa_state;
  /*!< Current PA State */

  uint8 hdet_raw;
  /*!< Raw HDET */

  uint16 therm_raw;
  /*!< Raw Therm */

  uint8 therm_scaled;
  /*!< Scaled Therm */

  uint8 temp_index;
  /*!< Temperature index */

  uint8 temp_remainder;
  /*!< Temperature remainder */

  int16 rx_agc_1;
  /*!< Current Rx AGC for Device 1 */

  byte lna_state_1;
  /*!< Current LNA State for Device 1 */

  uint8 intelliceiver_state;
  /*!< Current intelliceiver state */

  uint16 hdet_raw_v2;
  /*!< Raw HDET */

  uint16 pa_smps_pdm;
  /*!< PA SMPS Bias Value in 10-bit HW units */

  int16 betap_gain;
  /*!< BetaPrime Value */ 
  
  int16 txagc_from_hdet;
  /*!< txagc in dbm10 converted from hdet read*/ 

  uint8 xpt_mode;
  /*!< xpt mode */

  int16 pin_comp;
  /*!< Pin compensation value */

  int16 pout_comp;
  /*!< Pout compensation value */

  int16 env_scale_backoff;
  /*!< envelop scale backoff in unit of 10th dB */
  
  int16 iq_backoff;
  /*!< iq gain backoff in unit of 10th dB */

  int16 xpt_delay;
  /*!< xpt delay ns*10 unit */

  uint16 dpd_index;
  /*!< dpd index */
  
  uint16 pa_icq;
  /*!< PA quiescent current setting */

} ftm_cdma_c1_agc_type;

/*----------------------------------------------------------------------------*/
/*!< Structure to contain the Log header and Log paylaod for Device 0 */
typedef PACK(struct)
{
  ftm_log_gen_type header;
  /*!< Log header to be sent along with Log paylaod */

  ftm_cdma_c0_agc_type data;
  /*!< AGC Log data paylaod for device 0 */

} ftm_cdma_log_c0_agc_type;

/*----------------------------------------------------------------------------*/
/*!< Structure to contain the Log header and Log paylaod for Device 1 */
typedef PACK(struct)
{
  ftm_log_gen_type header;
  /*!< Log header to be sent along with Log paylaod */

  ftm_cdma_c1_agc_type data;
  /*!< AGC Log data paylaod for device 1 */

} ftm_cdma_log_c1_agc_type;

/*----------------------------------------------------------------------------*/
/*!< Structure to hold the Intelliceiver logging data */
typedef PACK(struct)
{
  int16  rx_agc;
  /*!< Current RxAGC */

  byte   lna_state;
  /*!< Current LNA State */

  byte   power_mode;
  /*!< Current PowerMode */

  uint32 reserved;
  /*!< Rerserved field */

} ftm_cdma_intelliceiver_data_type;

/*----------------------------------------------------------------------------*/
/*!< Structure to contain the Log header and Log paylaod for Intelliceiver */
typedef PACK(struct)
{
  ftm_log_gen_type            header;
  /*!< Log header to be sent along with Log paylaod */

  ftm_cdma_intelliceiver_data_type data;
  /*!< Intelliceiver Log data paylaod */

} ftm_cdma_log_intelliceiver_type;


/*----------------------------------------------------------------------------*/
void
ftm_cdma_log_c0_intelliceiver_cb
(
  int32 interval
);

#endif /* FTM_CDMA_LOG_H */

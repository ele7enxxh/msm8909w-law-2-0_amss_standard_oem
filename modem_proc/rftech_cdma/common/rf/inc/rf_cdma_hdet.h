#ifndef RF_CDMA_HDET_H
#define RF_CDMA_HDET_H

/*!
  @file
  rf_cdma_hdet.h

  @details
  This file exports the definitions and declartions needed for HDET operation
*/

/*==============================================================================

  Copyright (c) 2011-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_hdet.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/28/14   zhw     FTM HDET support
04/07/14   zhw     RFLM C2K HDET support
06/21/13   zhw     Tuner CL support. Remove redundant params
06/13/13   zhw     Tuner CL Return loss support using HDET mdsp polling
12/19/12   zhw     Online HDET initial support
03/11/13   hdz     Added rf_cdma_convert_hdet_read_to_txagc()
08/24/12   cd      CDMA EPT Temp and HDET feature support
02/02/12   cd      Added support for HDET Low Power mode 
12/22/11   shb     Added FW scripts in hdet_data so that we need to query it
                   one time per device
10/13/11   aro     Added interface to enable and disable various debug mode
08/25/11   shb     Added mode/band arg in rf_cdma_read_hdet()
08/25/11   shb     Added hdet vs agc offset variable in data as this is now 
                   extracted from NV.
08/25/11   shb     Updated code that converts hdet read to txagc to handle sign
                   in a more robust way
08/25/11   shb     Added API to read HDET. Added data members to track current 
                   band and last hdet read and tx agc
08/22/11   shb     Made exp_hdet_vs_agc a const pointer in HDET data type
08/22/11   shb     Added #define for max possible TX error
08/18/11   shb     Added device to hdet_data. Removed arg from start/stop APIs
08/16/11   shb     Added debugging_enabled flag to selectively print F3s
08/15/11   shb     Renamed HDET operating "mode" to "state" to avoid confusion 
                   with 1x/DO mode.
08/11/11   shb     Added argument to track HDET error convergence
08/09/11   shb     Deleted min/max pwr macros - use existing MDSP defns
08/08/11   shb     Extern rf_cdma_hdet_filter_err in offtarget file instead of
                   prototyping in header as API.
08/08/11   shb     Added rf_cdma_hdet_filter_err() prototype for offtgt testing
08/05/11   shb     Added members in hdet_data needed for timer cb, mdsp polling
                   MDSP interface, TX error calculation algorithm
08/03/11   shb     Added #defines for constants needed in HDET to TX AGC conv
08/03/11   shb     Added more state variables to HDET data type - needed for
                   basic start/stop/nv loading
07/26/11   aro     Updated HDET init to pass logical device
07/25/11   sty     Added critical section hook to rf_cdma_hdet_data_type
06/25/11   sty     Initial version

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfm_device_types.h"
#include "rfcommon_locks.h"
#include "rfc_cdma.h"
#include "rfdevice_cdma_interface.h" /* For rfdevice_cdma_hdet_param_type */
#include "timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Each bin in the Exp HDET vs AGC NV is 1.6 dB = 1.6 * 640 dB/640  */
#define RF_CDMA_EXP_HDET_VS_AGC_BIN_SIZE_DBX640          -1024

/*! Default Offset of Exp HDET vs AGC NV table = -256 x 64 TX AGC units (dB/640) */
#define RF_CDMA_EXP_HDET_VS_AGC_DEFAULT_OFFSET_DBX640   -16384

/*! MDSP polling period for CDMA HDET trigger status */
#define RF_CDMA_HDET_MDSP_POLLING_PERIOD_MS          1

/*! MDSP polling period for CDMA Tuner CL trigger status */
#define RF_CDMA_TUNER_CL_MDSP_POLLING_PERIOD_MS      1          

/*! HDET read validity window size - in number of HDET readings */
#define RF_CDMA_HDET_READ_VALIDITY_WINDOW            4

/*!
  @brief
  Max possible TX error on the transmit path in dB.

  @details
  This is effectively the delta from "tx power at min hdet" at which HDET 
  readings will be made since TX error cannot exceed this dB value. Any TX 
  power below this delta is considered too low to even attempt to read HDET. 
  At these low power levels, PDET will not be enabled to save current 
  consumption 
*/
#define RF_CDMA_HDET_MAX_TX_ERR_DB                   8
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration for modes in which may operate HDET
*/
typedef enum
{
  /*! HDET is not active */
  RF_CDMA_HDET_INACTIVE,

  /*! HDET is operating in "acquisition" state. HDET operates in this state
  when intialized, or when the difference between actual TX power and TxAGC 
  read from FW is more than a threshold specified by RF device
  */ 
  RF_CDMA_HDET_ACQUISITION,

  /*! HDET operates in "tracking" state when the difference between actual TX 
  power and TxAGC read from FW is less than a threshold configured by RF device
  */ 
  RF_CDMA_HDET_TRACKING,

} rf_cdma_hdet_state_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  State information that is needed by the CDMA HDET module.

  @details
  There is one instance of this structure per logical device in the CDMA
  data structure.  Each device may run its own HDET loop, and all the
  information needed in the HDET algorithm is stored in this data structure.
*/
typedef struct
{
  /*! Flag indicating if HDET initialization has been done by calling 
  rf_cdma_hdet_init() for a device */
  boolean init_done;

  /*! Current technology of operation (RFM Mode). Can be 1x or HDR */
  rfm_mode_enum_type mode;

  /*! Device which this data corresponds to */
  rfm_device_enum_type device;

  /*! Current band of operation - Only NVs are band dependent in HDET loop.
  Also needed for Turnaround constant which is band dependent. It is used
  to convert raw TXAGC to dbm for logging/debugging */
  rfm_cdma_band_class_type band;

  /*! Pointer to logical device HDET is currently operating ON. We store this
  in hdet_data to make it accessible in callback functions when needed */
  const rfc_cdma_logical_device_params_type* logical_device;

  /*! HDET related scripts obtained from TX device. Contains scripts to be 
  sent to FW to enable/trigger/disable HDET. Stored here so we dont have 
  to query it everytime before sending scripts to FW */ 
  rfdevice_cdma_hdet_scripts_type scripts;

  /*! Current operating state of HDET module */
  rf_cdma_hdet_state_type state;

  /*! Current power mode in which HDET is operating */
  rfdevice_cdma_hdet_power_mode_type power_mode;

  /*! counter that keeps track of number of times HDET reading has been 
  performed in the current operating state */ 
  uint8 state_count;

  /*! Status variable that is used to store whether HDET functionality is 
  enabled and valid for the two power modes */
  boolean enabled[RFDEVICE_CDMA_HDET_PM_NUM];

  /*! Number of HDET reads performed so far by MDSP image. MDSP udpates a 
  32bit SMEM counter everytime a successful read sequence is completed. We
  read this counter and compare it with the last count to figure if a fresh
  read is done. */
  uint32 mdsp_read_count;

  /*! Keeps track of number of times MDSP SMEM has been polled in current HDET 
  read loop for read done flag. Used as an escape criteria if MDSP fails to
  complete HDET read for an inexplicably long time */
  uint16 mdsp_polling_count;

  /*! difference between TxAGC read from FW and actual measured Tx power from
  the previous HDET measurement (in 1/640dB uints) */ 
  int16 filtered_tx_err;

  /*! Accumulated Tx Linearizer error in 1/640dB units */ 
  int16 accum_lin_tx_err;

  /*! Error in expected TX AGC based on current error estimate vs actual TX 
  power based on HDET reading. This variable tracks the convergence of 
  TX error estimate to actual TX error and hence is used to transition 
  between acquisition and tracking modes */
  int16 tx_estimate_err;

  /*! Interpolated HDET vs freq compensation to be applied at current channel */
  int8 hdet_vs_freq;

  /*! Interpolated HDET vs temp compensation to be applied based on the 
  last valid therm reading */
  int8 hdet_vs_temp;

  /*! Most recent HDET reading recorded for debugging and logging */
  uint16 last_hdet_read;

  /*! TX AGC during most recent HDET read in db640 (signed raw txagc) units for 
  debugging and logging */
  int16 last_txagc_at_hdet_read;

  /*! Min valid HDET reading for current band. Any HDET reading below this value 
  is considered invalid - cannot be trusted for accurate TX AGC estimation. 
  Comes from HDET OFFSET NV */
  uint16 min_hdet[RFDEVICE_CDMA_HDET_PM_NUM];

  /*! TX AGC read in raw MDSP TX AGC units (signed dB/640) for min HDET read */
  int16 txagc_at_min_hdet[RFDEVICE_CDMA_HDET_PM_NUM];

  /*! Max valid HDET reading for current band. Any HDET reading above this value 
  is considered invalid - cannot be trusted for accurate TX AGC estimation. 
  Sum of HDET OFFSET and SPAN NVs */
  uint16 max_hdet[RFDEVICE_CDMA_HDET_PM_NUM];

  /*! TX AGC read in raw MDSP TX AGC units (signed dB/640) for max HDET read */
  int16 txagc_at_max_hdet[RFDEVICE_CDMA_HDET_PM_NUM];

  /*! Pointer to first element of Expected HDET vs AGC cal data for current
  band */
  const uint16* exp_hdet_vs_agc[RFDEVICE_CDMA_HDET_PM_NUM];

  /*! Highest bin in Expected HDET vs AGC table with value that is railed low
  This captures flatness in lower end of hdet vs agc curve */
  uint8 exp_hdet_vs_agc_low_zone[RFDEVICE_CDMA_HDET_PM_NUM];

  /*! Lowest bin in Expected HDET vs AGC table with value that is railed high
  This captures flatness in higher end of hdet vs agc curve */
  uint8 exp_hdet_vs_agc_high_zone[RFDEVICE_CDMA_HDET_PM_NUM];

  /*! Start offset of Expected HDET vs AGC table in db640 (signed FW TX AGC) 
  units. Obtained from NV structure */
  int16 exp_hdet_vs_agc_start_offset[RFDEVICE_CDMA_HDET_PM_NUM];
  
  /*! Store the LPM HDET Threshold power value as obtained from NV. This is 
  stored in dBm10, and is used by the HDET algorithm to transition in and 
  out of LPM */
  int16 lpm_threshold_val;

  /*! All RF chipset dependent parameters that are queried from the device
  driver and used in HDET algorithm */
  rfdevice_cdma_hdet_param_type device_param;

  /*! Timer handle used to trigger HDET reads periodically thru MDSP in 
  acquisition as well as tracking mode */
  timer_type hdet_read_timer;

  /*! Timer handle used to poll MDSP SMEM for HDET trigger completion and 
  use HDET read for TXAGC error computation */
  timer_type hdet_mdsp_polling_timer;

  /*! Timer handle used to poll MDSP SMEM for Tuner CL trigger completion and 
  use HDET read for TXAGC error computation */
  timer_type tuner_cl_mdsp_polling_timer;
  
  /*! Bit representation of the valid HDET reads that are obtained 
  over the course of 4 HDET read triggers. For example, over the last 4 
  times we tried to do an HDET read, if the first and last were valid, 
  this variable would equal 1001(binary) = 9(dec) */ 
  uint8 hdet_read_validity_meter;

  /*! HDET Mod Range indicates the range of HDET accumulated error 
  that can be applied at different temperatures. It is bounded by 
  both Hi and Lo error values, in dB640 */ 
  int16 hdet_mod_range_hi;
  int16 hdet_mod_range_lo;

  /*! critical section protection for HDET operations */
  rf_lock_data_type crit_section;

  /*! Flag to enable debug mode. More F3s */
  boolean debugging_enabled;

} rf_cdma_hdet_data_type;

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_hdet_init
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_hdet_data_type *hdet_data
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_hdet_start
(
  rfm_device_enum_type device,
  rf_cdma_hdet_data_type *hdet_data,
  rfm_mode_enum_type mode,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_hdet_stop
(
  rfm_device_enum_type device,
  rf_cdma_hdet_data_type *hdet_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_read_hdet
(
  rf_cdma_hdet_data_type* hdet_data,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  uint16* hdet_reading,
  int16* txpwr_at_read_dbm10
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_hdet_configure_debug_mode
(
  rf_cdma_hdet_data_type* hdet_data,
  boolean debug_en
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_hdet_set_accum_err_from_temp_comp
(
  rf_cdma_hdet_data_type* hdet_data,
  int16 new_accum_err
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_hdet_update_read_validity
(
  rf_cdma_hdet_data_type* hdet_data,
  boolean curr_read_valid
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_hdet_get_recent_read_validity
(
  rf_cdma_hdet_data_type* hdet_data
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_hdet_update_temperature_params
(
  rf_cdma_hdet_data_type* hdet_data,
  uint16 last_temp_sensor_read
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_convert_hdet_read_to_txagc
(
  rf_cdma_hdet_data_type* hdet_data,
  uint16 hdet_read,
  rfdevice_cdma_hdet_power_mode_type power_mode
);

#endif /* FEATURE_CDMA1X */
#ifdef __cplusplus
}
#endif

#endif  /* RF_CDMA_HDET_H */


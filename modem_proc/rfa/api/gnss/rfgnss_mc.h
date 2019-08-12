#ifndef RFGNSS_MC_H
#define RFGNSS_MC_H
/*
   @file
   rfgnss_mc.h

   @brief
   This file contains RF GNSS main control definitions, enumerations and prototypes.

   @details
*/

/*===========================================================================
Copyright (c) 1999 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.


                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/gnss/rfgnss_mc.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/31/14   ndb     Added support for extended EARFCN for LTE
04/30/13   gy      Added API for GPS to vote rfc common to wakeup or sleep
04/09/13   dbz     Add new bootup FE and gain calibration API
03/28/13   gy      Add TDSCDMA band, channel status
02/11/13   dbz/gy  Added API to disable/enable gnss pga gain backoff
02/11/13   gy      Added gnss rfsw repartition
05/21/12   gy      Added gnss on/off state API
11/11/11   dbz     Changed 1x band type
10/03/11   dbz     Added timer based hysteresis loop for notch filter programming
08/02/11   dbz     Add new API for GNSS PGA tracking loop control
11/10/10   av      Check for the LTE Tx flag after gain adjust
10/05/10   av      Merge changes from Genesis
09/24/10   hp      Fixed compiler warnings 
09/10/10   hp      Update to LTE GPS B13 Gain Adjust Algorithm    
08/26/10   hp      Added API's for GPS Notch for LTE and EVDO 
06/30/10   hp      Added Support for LTE GPS B13 Gain Adj for SVLTE-Fusion
06/25/10   gy      Added GNSS Demod Quality API
29/01/10   sr      Changed the <tech>_mc_init() signature to fix warnings in 
                   <tech> specific builds
01/06/10   dbz     Added rfm_gnss_debug support
12/17/09   dbz     Changed rfgnss_notch_cfg_type 
11/30/09   bmg     Added common_init_successful parameter to initialization
                   Changed init function return to boolean
11/12/09   gy      Added rfgnss_mc_exit_mode
10/16/09   dbz     Added support for GNSS spur mitigation algorithms
10/15/09   hp      Added function prototype needed to support ELNA API
06/29/09   dbz     Merged Gen8 engineering codebase into source
06/18/09   gy      Added RFGNSS_ADC_MODE_ASTRA to GNSS ADC mode type
04/07/09   dbz     Adding rfgnss_mc_init to fix link error
03/10/09   gy      Code clean up
12/20/08   gy      Initial version to port gnss main control.
10/14/08   sr      Initial version of Gps Main Control.

============================================================================*/

#include "rfcom.h"
#include "rfm_cdma_band_types.h"
#include "rfm_device_types.h"
#include "rfm_gnss.h"

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This defines GNSS RF ZIF/LIF mode enums.

*/
typedef enum 
{
  RFGNSS_IF_MODE_ZIF,
  RFGNSS_IF_MODE_LIF,
} rfgnss_if_mode_enum_type;

typedef enum {
  RFGNSS_TX_OFF,
  RFGNSS_TX_ON
}rfgnss_tx_type;

typedef struct {
  rfgnss_tx_type rfgnss_tx;
  timer_group_type rfgnss_timer_group;
  timer_type rfgnss_tx_hysteresis_timer;
  boolean hysteresis_timer_on;
  rfcom_lte_band_type rf_tx_band_config;
  boolean rfgnss_gain_adjust_running;
  boolean rfgnss_tx_on_during_gain_adjust;
} rfgnss_gain_adjust_type;

typedef struct {
  rfgnss_notch_cfg_type rfgnss_notch_cfg;
  timer_group_type rfgnss_notch_timer_group;
  timer_type rfgnss_notch_timer;
  boolean notch_timer_on;
} rfgnss_notch_filter_timer_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This defines GNSS ADC enable/disable enums.

*/
typedef enum 
{
  RFGNSS_ADC_DISABLE,
  RFGNSS_ADC_ENABLE,
  RFGNSS_ADC_NUM_OPS
} rfgnss_adc_op_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This defines GNSS ADC modes.

*/
typedef enum
{
  RFGNSS_ADC_MODE_1=0,
  RFGNSS_ADC_MODE_2,
  RFGNSS_ADC_MODE_5,
  RFGNSS_ADC_MODE_ASTRA,
  RFGNSS_ADC_MODE_NUM,
  RFGNSS_ADC_MODE_MAX=RFGNSS_ADC_MODE_NUM
} rfgnss_adc_mode_enum_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This defines RF library enumeration numbers for RF GNSS.

*/
typedef enum{
  UMTS_RF_LIBRARY,
  CDMA1X_RF_LIBRARY,
  UNSUPPORTED_RF_LIBRARY
} rfgnss_rf_library_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This defines RF GNSS polority enumeration numbers.

*/
typedef enum
{
  CGPS_RX_SPECTRAL_INV,
  CGPS_RX_SPECTRAL_NORM
} cgps_rx_polarity_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This defines RF GNSS control enumeration numbers.

*/
typedef enum {
   RFGNSS_CMD1,
   RFGNSS_CMD2
  } rfgnss_mc_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

  @todo Dongbo to Implement this function, should do the rfdevice fp init 
  and prepare for bootup FE tuning

*/
extern boolean rfgnss_mc_init
(
  rfcom_device_enum_type device,
  boolean common_init_successful,
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Enter into gnss operation.

*/
extern uint32 rfgnss_mc_enter_mode(rfcom_device_enum_type device,
                             const void *user_data_ptr,
                             rfm_cb_handler_type cb_handler);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Configure GNSS mode operation after entering gnss mode.
*/
int32 rfgnss_mc_configure ( rfcom_device_enum_type device, rfgnss_mode_cfg_enum_type rfgnss_mode );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Exit gnss operation.
*/

int32 rfgnss_mc_exit( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Resume gnss operation after entering gnss pause mode.
*/

int32 rfgnss_mc_resume(rfcom_device_enum_type device);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   Pause gnss operation for fts or dpo.
*/
int32 rfgnss_mc_pause(rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Query for dc and amplitude values and update the RF as needed.

*/
int32 rfgnss_mc_trigger_bbc_update(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function handles rf mode change.

*/
int32 rfgnss_mc_exit_mode(rfcom_device_enum_type device,
                           rfcom_mode_enum_type to_mode);

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Resume gnss operation after entering gnss pause mode.

   @param device : RF Multimode device
*/
int32 rfgnss_mc_1x_band_chan(rfm_cdma_band_class_type band, word chan);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Provides information regarding RF Band and Chan for GNSS Notch Programming

   @details
   
   @param device : RF Multimode device
*/
int32 rfgnss_mc_evdo_band_chan(rfm_cdma_band_class_type band, uint32 chan);

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Resume gnss operation after entering gnss pause mode.

   @param device : RF Multimode device
*/
int32 rfgnss_mc_wg_chan(uint16 chan);

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Resume gnss operation after entering gnss pause mode.

   @param device : RF Multimode device
*/
int32 rfgnss_mc_program_notch_filter(rfgnss_notch_cfg_type notch_cfg);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   Dispatch gnss main control commands.
*/

int32 rfgnss_mc_cmd_dispatch(rfcom_device_enum_type device, int32 cmd,
                            void *data);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This API sets up the environment based of presence or absence or an external LNA. 
*/
int32 rfgnss_mc_has_ext_lna (boolean gnss_has_ext_lna);

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
  This function is used for debug purposes. The function queries
  the GPS state and returns TRUE if GPS is ON and FALSE if GPS is 
  turned OFF.

   @param device : RF Multimode device
 
          Boolean - TRUE if GPS State is ON and FALSE if turned OFF.
*/
boolean rfgnss_mc_debug(boolean verbose, rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Perform GNSS demod quality control

*/
int32 rfgnss_mc_demod_quality_control(rfgnss_demod_quality_enum_type rfgnss_demod_quality);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function updates the LTE Tx band
*/
void rfgnss_mc_tx_band(rfcom_lte_band_type rf_tx_band);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function updates the LTE Tx flag and lowers
   the PGA gain if needed  
*/
void rfgnss_mc_tx_indicator( boolean enable );

/*----------------------------------------------------------------------------*/
void rfgnss_mc_lte_band_chan(rfcom_lte_band_type band, rfcom_lte_earfcn_type chan);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Update notches with TDSCDMA band and channel information
*/
void rfgnss_mc_tdscdma_band_chan(rfcom_tdscdma_band_type band, uint16 chan);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS PGA tracking loop control

*/
void rfgnss_mc_pga_tracking_loop_control(boolean enable);

/*----------------------------------------------------------------------------*/
void rfgnss_mc_set_pga_backoff(void);

/*----------------------------------------------------------------------------*/
void rfgnss_mc_set_pga_restore(void);
/*----------------------------------------------------------------------------*/
int32 rfgnss_mc_wwan_tx_status(rfm_device_enum_type device, boolean enable );

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function provides GNSS on/off status

   @param device: none

*/
boolean rfgnss_mc_get_state(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS vote for dev on/off

*/
boolean rfgnss_mc_resource_vote(rfgnss_device_status_type dev_status, rfgnss_device_vote_type dev_vote);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS device sequence execute command dispatch

*/
boolean rfgnss_mc_device_seq_execute(rfm_gnss_device_seq_cmd_type cmd, void *data);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS vote for ccs enable

*/
boolean rfgnss_mc_hal_bus_enable (boolean enable);

/*
   @details
   This function is used to disable/enable GNSS PGA gain backoff

   @param device: none

*/
void rfgnss_mc_pga_gain_backoff_control(boolean enable);

/*----------------------------------------------------------------------------*/
/*!
   @brief
 
   @details
   This function is used to initiate GNSS boot-up calibration from
   GNSS SW once NPA resources are available and GNSS SW can turn clocks
   ON.

   @param device

*/

void rfgnss_mc_bootup_cal(rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   API for NavRF to vote for RFC wakeup or sleep

*/
boolean rfgnss_mc_rfc_common_enable(boolean enable);

#endif /* RFGNSS_MC_H */



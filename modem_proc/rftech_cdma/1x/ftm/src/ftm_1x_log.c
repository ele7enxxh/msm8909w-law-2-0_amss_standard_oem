/*!
  @file
  ftm_1x_log.c

  @brief
  This module contains functions used for 1x RF Logging

  @addtogroup RF_FTM_1X_LOG
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/ftm/src/ftm_1x_log.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/01/14   cd      Add support for FTM AGC logging with FED and common tx AGC 
                   functionality
10/16/13  cc       Add new ET logging for FTM AGC logs
06/10/13   sty     Reduced F3 message levels
03/19/13   APU     Replaced memcpy with memscpy.
03/14/13   aro     Deprecated Composite Calibration and TxSweep Commands
02/20/13   sty     Moved ftm_hdr_log_search_diversity_device and 
                   ftm_hdr_log_search_primary_device() to rfm_ftm layer
02/08/13   sty     F3 message cleanups
03/09/13   hdz     Added logging HDET reading in txagc unit
11/28/12   nrk     Added beta scaling logging support
08/07/12   hdz     Added logging temp comp's index and remainder
04/04/12   jr      Ensure 1x AGC logging is not colliding with log disable operation
03/15/12   spa     Added max device check for rf_cdma_get_device_status() 
02/10/12   sb      added log_dmss.h
01/23/12   hdz     Added support to log thermistor reading
12/20/11   aro     Enabled logging for APT value and TxLUT index
09/09/11   aro     Added feature to resolve the currently active primary and
                   diversity device
09/09/11   aro     Renamed CDMA logging functions to reflect primary and
                   secondary chains
09/08/11   aro     Fixed RxAGC units for logging
09/07/11   aro     Added functions to initialize, de-initialize and push the
                   RFM_DEVICE_1 AGC logging data
08/31/11   aro     Updated HDET Read to use new API
08/24/11   aro     Added functions to initialize, de-initialize and push the
                   RFM_DEVICE_0 AGC logging data
03/07/11   aro     Added Composite Calibration Log function
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/13/10   aro     Removed FTM_HAS_LOGGING Feature
12/07/10   aro     Initial Revision : Common Calibration

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "log_dmss.h"
#ifdef FEATURE_CDMA1X
/* memscpy */
#include "stringl.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_1x_log.h"
#include "ftm_cdma_data.h"
#include "ftm_log.h"
#include "ftm_msg.h"
#include "rfm_1x_ftm.h"
#include "rfm_1x.h"
#include "rfm_cdma_ftm.h"
#include "modem_mem.h"
#include "ftm_common_locks.h"

/*----------------------------------------------------------------------------*/
/*! Temperature Bin Size */
#define FTM_1X_LOG_TEMP_BIN_SIZ 36

/*============================================================================*/
/*!
  @name 1x AGC Logging

  @brief
  This section contains the functions used for 1x AGC Loggings
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize 1x AGC Log for Primary Path

  @details
  This function initializes 1x AGC Log by creating the ftm_log_array, and 
  populating it with the supported entries.
*/
void
ftm_1x_log_primary_agc_init_func
(
  void
)
{
  ftm_log_config_table_entry *log_config_ptr;
  ftm_cdma_log_data_type* log_data;
 
  /* Get Log config pointer */
  log_config_ptr = ftm_log_find_log_id(FTM_LOG_1X_AGC);
        
  /* Check that we have valid log configuration */
  if (log_config_ptr == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_primary_agc_init_func : Invalid Log "
                        "Config" );
    return;
  }
  ftm_common_enter_critical_section();
 
  /* Get CDMA Log Data pointer */
  log_data = ftm_cdma_get_log_ptr();

  /* Check that we have valid CDMA log data */
  if (log_data == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_primary_agc_init_func : Invalid Log Data" );
    ftm_common_leave_critical_section();
    return;
  }

  /* Allocate memory for chain 0 AGC */
  log_data->onex_log.c0_agc = (ftm_cdma_c0_agc_type*)modem_mem_alloc(
      log_config_ptr->log_config.num_buffer * sizeof (ftm_cdma_c0_agc_type),
      MODEM_MEM_CLIENT_RFA );
        
  /* check that we have memory to work with */
  if (log_data->onex_log.c0_agc == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_primary_agc_init_func : Log Buffer "
                        "allocation failed" );
  }
  MSG_LOW("ftm_1x_log_primary_agc_init_func: called",0,0,0);
  ftm_common_leave_critical_section();

} /* ftm_1x_log_primary_agc_init_func */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Free Internal Buffer for 1x AGC Log on Primary Path

  @details
  This function frees the memory used as internal bufer for 1x AGC Logs
*/
void
ftm_1x_log_primary_agc_exit_func
(
  void
)
{
  ftm_cdma_log_data_type* log_data;
  
  ftm_common_enter_critical_section();
  /* Get CDMA Log Data pointer */
  log_data = ftm_cdma_get_log_ptr();

  /* Check that we have valid CDMA log data */
  if (log_data == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_primary_agc_exit_func : Invalid Log Data" );
    ftm_common_leave_critical_section();
    return;
  }

  /* Free the memory */
  if( log_data->onex_log.c0_agc != NULL)
  {
    modem_mem_free(log_data->onex_log.c0_agc, MODEM_MEM_CLIENT_RFA);
    log_data->onex_log.c0_agc = NULL;
  }
  MSG_LOW("ftm_1x_log_primary_agc_exit_func: called",0,0,0);
  ftm_common_leave_critical_section();

} /* ftm_1x_log_primary_agc_exit_func */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Log 1x AGC on Primary Path

  @details
  This function reads the registers required for 1x AGC logging and
  returns them in an FTM log packet. This callback function, must be enabled 
  with ftm_log_enable.

  @param interval
  Interval
*/
void
ftm_1x_log_primary_agc_cb
(
  int32 interval
)
{
  static int buf = 0;
  ftm_cdma_log_c0_agc_type *agc_log = NULL;
  byte tx_min_status;
  byte tx_max_status;
  ftm_log_config_table_entry *log_config_ptr;
  ftm_cdma_log_data_type *log_data;
  rfm_cdma_ftm_rx_agc_data_type rx_agc_data;
  rfm_cdma_ftm_tx_agc_data_type tx_agc_data;
  rfm_device_enum_type device = RFM_INVALID_DEVICE;

  log_config_ptr = ftm_log_find_log_id(FTM_LOG_1X_AGC);

  /* check that we have valid log configuration */
  if (log_config_ptr == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_primary_agc_cb : Invalid Log Config" );
    return;
  }
  ftm_common_enter_critical_section();

  /* Get CDMA Log Data pointer */
  log_data = ftm_cdma_get_log_ptr();

  /* Check that we have valid CDMA log data */
  if (log_data == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_primary_agc_cb : Invalid Log Data" );
    ftm_common_leave_critical_section();
    return;
  }
  
  /* check if the Log buffer is allocated or not */
  if ( log_data->onex_log.c0_agc == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_primary_agc_cb : Log Buffer memeory not"
                        "allocated " );
    ftm_common_leave_critical_section();
    return;
  }

  /* Search for the primary Device */
  device = rfm_cdma_ftm_log_search_primary_device(RFM_1X_MODE);

  /* Do not continue, if the device is not Valid */
  if ( device == RFM_INVALID_DEVICE )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_primary_agc_cb : No Primary Device Found" );
    ftm_common_leave_critical_section();
    return;
  }

  rfm_1x_ftm_get_rx_agc_data( device, &rx_agc_data,
                              RFM_CDMA_AGC_FORMAT__RAW );

  rfm_1x_ftm_get_tx_agc_data( device, &tx_agc_data,
                              RFM_CDMA_AGC_FORMAT__RAW );

  log_data->onex_log.c0_agc[buf].rx_agc = (rx_agc_data.rx_agc[RFM_CDMA_CARRIER_0])/64;
  log_data->onex_log.c0_agc[buf].lna_state = rx_agc_data.gain_state;

  log_data->onex_log.c0_agc[buf].tx_gain_ctl =
                          (int)((tx_agc_data.total_power) << 16) >> 22;
  log_data->onex_log.c0_agc[buf].tx_gain_adj = 
                          (int)((tx_agc_data.closed_loop_power) << 16) >> 23;
  log_data->onex_log.c0_agc[buf].tx_open_loop = 
                         (int)((tx_agc_data.open_loop_power) << 16) >> 22 ;
  log_data->onex_log.c0_agc[buf].tx_agc_adj = tx_agc_data.avga_index;
  tx_min_status = ((~(byte)(tx_agc_data.tx_min)) & 0x1);
  tx_max_status = ((~(byte)(tx_agc_data.tx_max)) & 0x1);
  log_data->onex_log.c0_agc[buf].pa_ratchet = 
                                   (tx_min_status << 1) & tx_max_status;
  log_data->onex_log.c0_agc[buf].pa_state = tx_agc_data.pa_state; 
  log_data->onex_log.c0_agc[buf].hdet_raw_v2 = rfm_cdma_ftm_read_hdet( device );
  log_data->onex_log.c0_agc[buf].hdet_raw = 0;
  log_data->onex_log.c0_agc[buf].txagc_from_hdet =
    rfm_cdma_ftm_convert_hdet_to_txagc(log_data->onex_log.c0_agc[buf].hdet_raw_v2,device);
  log_data->onex_log.c0_agc[buf].therm_scaled = 0;
  log_data->onex_log.c0_agc[buf].temp_index = 
    rfm_cdma_ftm_get_therm_index_remainder( device, 
    &(log_data->onex_log.c0_agc[buf].temp_remainder),
    &(log_data->onex_log.c0_agc[buf].therm_raw) );
  log_data->onex_log.c0_agc[buf].intelliceiver_state = 
                                   rfm_cdma_ftm_get_power_mode( device );
  log_data->onex_log.c0_agc[buf].pa_smps_pdm = (uint16)tx_agc_data.apt_val;
  log_data->onex_log.c0_agc[buf].betap_gain = tx_agc_data.betap_gain;
  log_data->onex_log.c0_agc[buf].xpt_mode = tx_agc_data.xpt_mode_val;
  log_data->onex_log.c0_agc[buf].pin_comp = tx_agc_data.pin_comp;
  log_data->onex_log.c0_agc[buf].pout_comp = tx_agc_data.pout_comp;
  log_data->onex_log.c0_agc[buf].env_scale_backoff = tx_agc_data.env_gain_val;
  log_data->onex_log.c0_agc[buf].iq_backoff = tx_agc_data.iq_gain_val;
  log_data->onex_log.c0_agc[buf].xpt_delay = tx_agc_data.xpt_delay_val; 
  log_data->onex_log.c0_agc[buf].dpd_index = tx_agc_data.dpd_idx;
  log_data->onex_log.c0_agc[buf].pa_icq = tx_agc_data.pa_q_current;

  if (++buf >= log_config_ptr->log_config.num_buffer)
  {
    agc_log = (ftm_cdma_log_c0_agc_type *)ftm_log_malloc( FTM_LOG_1X_AGC,
                               ( sizeof (ftm_cdma_log_c0_agc_type) + 
                                 (log_config_ptr->log_config.num_buffer - 1)*
                                 sizeof( ftm_cdma_c0_agc_type) ) );
    if ( agc_log != NULL)
    {
      size_t data_length = 
        log_config_ptr->log_config.num_buffer * sizeof( ftm_cdma_c0_agc_type) ; 

      /* have to copy the internal buffer to the suite_log packet, we cannot 
      simply accumulate the data in the suite_log packet directly since any 
      memory allocated with ftm_log_malloc must be passed to log_commit soon 
      afterward (where "soon" is apparently less than tens of ms) */
      memscpy ( ( void*)&( agc_log->data), data_length ,
                ( void*)log_data->onex_log.c0_agc , data_length ) ; 

      /* send log */
      log_commit( (ftm_log_type*) agc_log );
    }

    buf = 0;
  }
  ftm_common_leave_critical_section();

  FTM_MSG_1( FTM_LOW, "ftm_1x_log_primary_agc_cb: Executed for Device %d",
             device );
  
} /* ftm_1x_log_primary_agc_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize 1x AGC Log for Diversity Path

  @details
  This function initializes 1x AGC Log by creating the ftm_log_array, and 
  populating it with the supported entries.
*/
void
ftm_1x_log_diversity_agc_init_func
(
  void
)
{

  ftm_log_config_table_entry *log_config_ptr;
  ftm_cdma_log_data_type* log_data;
 
  /* Get Log config pointer */
  log_config_ptr = ftm_log_find_log_id(FTM_LOG_1X_AGC_C1);
        
  /* Check that we have valid log configuration */
  if (log_config_ptr == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_diversity_agc_init_func : Invalid Log "
                        "Config" );
    return;
  }

  ftm_common_enter_critical_section();
  /* Get CDMA Log Data pointer */
  log_data = ftm_cdma_get_log_ptr();

  /* Check that we have valid CDMA log data */
  if (log_data == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_diversity_agc_init_func : Invalid Log "
                        "Data" );
    ftm_common_leave_critical_section();
    return;
  }

  /* Allocate memory for chain 0 AGC */
  log_data->onex_log.c1_agc = (ftm_cdma_c1_agc_type*)modem_mem_alloc(
      log_config_ptr->log_config.num_buffer * sizeof (ftm_cdma_c1_agc_type),
      MODEM_MEM_CLIENT_RFA );
        
  /* check that we have memory to work with */
  if (log_data->onex_log.c1_agc == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_diversity_agc_init_func : Log Buffer "
                        "allocation failed" );
  }

  ftm_common_leave_critical_section();

  return;

} /* ftm_1x_log_diversity_agc_init_func */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Free Internal Buffer for 1x AGC Log on Diversity Path

  @details
  This function frees the memory used as internal bufer for 1x AGC Logs
*/
void
ftm_1x_log_diversity_agc_exit_func
(
  void
)
{
  ftm_cdma_log_data_type* log_data;
 
  ftm_common_enter_critical_section();
  /* Get CDMA Log Data pointer */
  log_data = ftm_cdma_get_log_ptr();

  /* Check that we have valid CDMA log data */
  if (log_data == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_diversity_agc_exit_func : Invalid Log "
                        "Data" );

    ftm_common_leave_critical_section();
    return;
  }

  /* Free the memory */
  if( log_data->onex_log.c1_agc != NULL)
  {
    modem_mem_free(log_data->onex_log.c1_agc, MODEM_MEM_CLIENT_RFA);
    log_data->onex_log.c1_agc = NULL;
  }
  ftm_common_leave_critical_section();

} /* ftm_1x_log_diversity_agc_exit_func */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Log 1x AGC on Diversity Path

  @details
  This function reads the registers required for 1x AGC logging and
  returns them in an FTM log packet. This callback function, must be enabled 
  with ftm_log_enable.

  @param interval
  Interval
*/
void
ftm_1x_log_diversity_agc_cb
(
  int32 interval
)
{
  static int buf = 0;
  ftm_cdma_log_c1_agc_type *agc_log = NULL;
  byte tx_min_status;
  byte tx_max_status;
  ftm_log_config_table_entry *log_config_ptr;
  ftm_cdma_log_data_type *log_data;
  rfm_cdma_ftm_rx_agc_data_type rx0_agc_data;
  rfm_cdma_ftm_rx_agc_data_type rx1_agc_data;
  rfm_cdma_ftm_tx_agc_data_type tx_agc_data;
  rfm_device_enum_type p_device = RFM_INVALID_DEVICE;
  rfm_device_enum_type d_device = RFM_INVALID_DEVICE;

  log_config_ptr = ftm_log_find_log_id(FTM_LOG_1X_AGC_C1);

  /* check that we have valid log configuration */
  if (log_config_ptr == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_diversity_agc_cb : Invalid Log Config" );
    return;
  }
 
  ftm_common_enter_critical_section();
  /* Get CDMA Log Data pointer */
  log_data = ftm_cdma_get_log_ptr();

  /* Check that we have valid CDMA log data */
  if (log_data == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_diversity_agc_cb : Invalid Log Data" );
    ftm_common_leave_critical_section();
    return;
  }

  /* check if the Log buffer is allocated or not */
  if ( log_data->onex_log.c1_agc == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_diversity_agc_cb : Log Buffer memeory not"
                        "allocated " );
    ftm_common_leave_critical_section();
    return;
  }

  /* Search for the diversity Device */
  p_device = rfm_cdma_ftm_log_search_primary_device(RFM_1X_MODE);

  /* Do not continue, if the device is not Valid */
  if ( p_device == RFM_INVALID_DEVICE )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_diversity_agc_cb : No Primary Device "
                        "Found" );
    ftm_common_leave_critical_section();
    return;
  }

  /* Search for the diversity Device */
  d_device = rfm_cdma_ftm_log_search_diversity_device(RFM_1X_MODE);

  /* Do not continue, if the device is not Valid */
  if ( d_device == RFM_INVALID_DEVICE )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_log_diversity_agc_cb : No Diversity Device "
                        "Found" );
    ftm_common_leave_critical_section();
    return;
  }

  rfm_1x_ftm_get_rx_agc_data( p_device, &rx0_agc_data,
                              RFM_CDMA_AGC_FORMAT__RAW );
  rfm_1x_ftm_get_rx_agc_data( d_device, &rx1_agc_data,
                              RFM_CDMA_AGC_FORMAT__RAW );

  rfm_1x_ftm_get_tx_agc_data( p_device, &tx_agc_data,
                              RFM_CDMA_AGC_FORMAT__RAW );

  log_data->onex_log.c1_agc[buf].rx_agc = 
    (rx0_agc_data.rx_agc[RFM_CDMA_CARRIER_0])/64;
  log_data->onex_log.c1_agc[buf].rx_agc_1 = 
    (rx1_agc_data.rx_agc[RFM_CDMA_CARRIER_0])/64;
  log_data->onex_log.c1_agc[buf].lna_state = rx0_agc_data.gain_state;
  log_data->onex_log.c1_agc[buf].lna_state_1 = rx1_agc_data.gain_state;

  log_data->onex_log.c1_agc[buf].tx_gain_ctl =
                          (int)((tx_agc_data.total_power) << 16) >> 22;
  log_data->onex_log.c1_agc[buf].tx_gain_adj = 
                          (int)((tx_agc_data.closed_loop_power) << 16) >> 23;
  log_data->onex_log.c1_agc[buf].tx_open_loop = 
                         (int)((tx_agc_data.open_loop_power) << 16) >> 22 ;
  log_data->onex_log.c1_agc[buf].tx_agc_adj = tx_agc_data.avga_index;
  tx_min_status = ((~(byte)(tx_agc_data.tx_min)) & 0x1);
  tx_max_status = ((~(byte)(tx_agc_data.tx_max)) & 0x1);
  log_data->onex_log.c1_agc[buf].pa_ratchet = 
                                  (tx_min_status << 1) & tx_max_status;
  log_data->onex_log.c1_agc[buf].pa_state = tx_agc_data.pa_state; 
  log_data->onex_log.c1_agc[buf].hdet_raw_v2 = rfm_cdma_ftm_read_hdet(p_device);
  log_data->onex_log.c1_agc[buf].hdet_raw = 0;
  log_data->onex_log.c1_agc[buf].txagc_from_hdet = 0;
  log_data->onex_log.c1_agc[buf].therm_scaled = 0;
  log_data->onex_log.c1_agc[buf].therm_raw = 
    rfm_cdma_ftm_get_therm_reading( p_device );
  log_data->onex_log.c1_agc[buf].temp_index = 
          log_data->onex_log.c1_agc[buf].therm_scaled / FTM_1X_LOG_TEMP_BIN_SIZ;
  log_data->onex_log.c1_agc[buf].temp_remainder =
          log_data->onex_log.c1_agc[buf].therm_scaled % FTM_1X_LOG_TEMP_BIN_SIZ;
  log_data->onex_log.c1_agc[buf].intelliceiver_state = 
                                   rfm_cdma_ftm_get_power_mode( p_device );
  log_data->onex_log.c1_agc[buf].pa_smps_pdm = (uint16)tx_agc_data.apt_val;
  log_data->onex_log.c1_agc[buf].betap_gain = tx_agc_data.betap_gain;  
  log_data->onex_log.c1_agc[buf].xpt_mode = tx_agc_data.xpt_mode_val;
  log_data->onex_log.c1_agc[buf].pin_comp = tx_agc_data.pin_comp;
  log_data->onex_log.c1_agc[buf].pout_comp = tx_agc_data.pout_comp;
  log_data->onex_log.c1_agc[buf].env_scale_backoff = tx_agc_data.env_gain_val;
  log_data->onex_log.c1_agc[buf].iq_backoff = tx_agc_data.iq_gain_val;
  log_data->onex_log.c1_agc[buf].xpt_delay = tx_agc_data.xpt_delay_val; 
  log_data->onex_log.c1_agc[buf].dpd_index = tx_agc_data.dpd_idx;
  log_data->onex_log.c1_agc[buf].pa_icq = tx_agc_data.pa_q_current;

  if (++buf >= log_config_ptr->log_config.num_buffer)
  {
    agc_log = (ftm_cdma_log_c1_agc_type *)ftm_log_malloc( FTM_LOG_1X_AGC_C1,
                               ( sizeof (ftm_cdma_log_c1_agc_type) + 
                                 (log_config_ptr->log_config.num_buffer - 1)*
                                 sizeof( ftm_cdma_c1_agc_type) ) );
    if ( agc_log != NULL)
    {
       size_t data_length = 
         log_config_ptr->log_config.num_buffer * sizeof( ftm_cdma_c1_agc_type)  ; 
      /* have to copy the internal buffer to the suite_log packet, we cannot 
      simply accumulate the data in the suite_log packet directly since any 
      memory allocated with ftm_log_malloc must be passed to log_commit soon 
      afterward (where "soon" is apparently less than tens of ms) */
      memscpy ( ( void*)&( agc_log->data), data_length , 
                ( void*)log_data->onex_log.c1_agc, data_length ) ;

      /* send log */
      log_commit( (ftm_log_type*) agc_log );
    }

    buf = 0;
  }

  FTM_MSG_2( FTM_LOW, "ftm_1x_log_diversity_agc_cb: Executed for pDev %d, "
             "d_Dev %d", p_device, d_device );

  ftm_common_leave_critical_section();

} /* ftm_1x_log_diversity_agc_cb */

#endif /* FEATURE_CDMA1X */
/*! \} */


#endif /* FEATURE_FACTORY_TESTMODE */

/*! @} */


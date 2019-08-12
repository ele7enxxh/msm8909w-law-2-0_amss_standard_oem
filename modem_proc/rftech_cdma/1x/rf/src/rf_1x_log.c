/*!
   @file
   rf_1x_log.c

   @brief
   This file contains functions for RF log sub-packets specific to CDMA.
 
   @addtogroup RF_LOG_DEFN_CDMA
   @{
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/src/rf_1x_log.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/08/15   rs      KW error fix
11/12/13   JJ      Added a check in rf_1x_log_wakeup_rx
11/07/13   zhw     Split 1x/HDR wu_time logging structures
12/27/12   shb     Updates for different physical rx_device per band
08/30/12   bmg     Added sample logging
06/11/12   spa     Added warmup time histogram to log packet
05/10/12   spa     Moved RxAGC and LNA stage F3 outisde log_enabled check  
04/19/12   spa     Replaced writable device pointer with read only pointer 
04/18/12   spa     Initial version

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_1x_log.h"
#include "log.h"
#include "rfm_1x.h"
#include "rf_cdma_data.h"
#include "rfcommon_msg.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_msm.h"
#include "rf_1x_mdsp.h"
#include "rf_cdma_constants.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Number of samples to include in a 1x sample log packet.

  @details
  This size is based roughly on double the expected AGC acquisition time,
  so the log can be used to verify the gain stepping during acquisition.
*/
#define RF_1X_SAMPLE_LOG_SAMPLE_COUNT 2048

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Log structure for a simple 1x sample capture log packet.
*/
typedef struct cdma_sample_log_s
{
  /*! RF Logging common header. */
  rfcommon_log_rf_header_type     rf_header;

  /*! RF Logging subpacket common header. */
  rf_cdma_log_sub_pkt_header_type rf_subpacket_header;

  /*! @ref RF_1X_SAMPLE_LOG_SAMPLE_COUNT samples of sample capture data. */
  rf_cdma_msm_sample_type         samples[RF_1X_SAMPLE_LOG_SAMPLE_COUNT];
} cdma_sample_log_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate RF 1x warmup log packet and submit it 

  @details
  This function takes the current device as a parameter and populates the RF 1x 
  warmup log packet. Queries for the enhanced warmup time, the RxAGC, LNA state
  and all stats related to warmup

  @param device
  The RF device on which the warmup is performed.
*/

void
rf_1x_log_wakeup_rx
(
  rfm_device_enum_type device
)
{
  /* RxAGC Read */
  rf_1x_mdsp_rx_agc_read_type rx_agc_read;
   
  /* LNA state as read from RF device */
  uint8 rtr_lna_state; 

  /* Initialize Header for log */
  rf_1x_log_rf_warmup_log_pkt *rf_wu_log_pkt = NULL;

  /* To hold size of Log Packet*/
  unsigned int length;

  /* Counter for histogram loop*/
  unsigned int ctr;

  /* Pointer to expected warmup time*/
  rfm_1x_rx_warmup_time_type expected_wu_time = {0,0};

  /* get a valid device pointer */
  const rf_cdma_data_status_type* dev_status_r = 
                                  rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  const rfc_cdma_logical_device_params_type *logical_dev = 
                                rfc_cdma_get_logical_device_params( device );

  /*get enhanced warmup time*/
  boolean ret_val = rfc_cdma_get_enhanced_warmup_time( device, RFM_1X_MODE ,
                                                            &expected_wu_time );

  /*Check if enhanced warmup time was successfully retrieved*/
  if( ret_val == FALSE )
  { 
    /* Error F3 */  
    RF_MSG_1( RF_ERROR, "rf_1x_log_wakeup_rx: Dev %d - RFC query "
                        "failed", device );
    return;
  }

  if ( dev_status_r == NULL || ( logical_dev == NULL ) )
  {
    /* Error F3 */
    RF_MSG_1(RF_ERROR,"rf_1x_log_wakeup_rx:NULL data for device %d",device);
    return;
  }

  memset( &rx_agc_read, 0, sizeof(rf_1x_mdsp_rx_agc_read_type) );
  
  /* check if rx state is rx */
  if (rf_cdma_data_get_1x_rf_state(device) == RF_CDMA_STATE_RX) 
  {
  /* Get Final RxAGC/ LNA State */
  rf_1x_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_read );

  rx_agc_read.rx_agc = rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                             rx_agc_read.rx_agc);

  /* Get LNA state from RF device */
  rfdevice_cdma_rx_cmd_dispatch( logical_dev->rx_device[dev_status_r->curr_band], 
                                 RFDEVICE_GET_LNA_STATE,
                                 &rtr_lna_state);
  }
  else
  {
    /* if it's not in rx state, set rx_agc and rtr_lna_state to invalid */
    rx_agc_read.rx_agc = RF_1X_MDSP_RXAGC_MIN_VAL;   
    rtr_lna_state = 0xFF;
    RF_MSG( RF_ERROR, "rf_1x_log_wakeup_rx: rf_state is invalid! ");
  }
  

  /* Report band-chan information, final Rx AGC and LNA state */
  RF_MSG_8( RF_HIGH, "rf_1x_log_wakeup_rx: Dev %d, RxLM %d, Band %d, "
            "Chan %d, SSMA_Chan %d, RxAGC %d (dBm10), LNA (FW) %d, LNA "
            "(RTR) 0x%x", device, dev_status_r->rxlm_handle,  
            dev_status_r->curr_band, 
            dev_status_r->curr_chans[RFM_CDMA_CARRIER_0],
            dev_status_r->lo_tuned_to_chan, rx_agc_read.rx_agc, 
            rx_agc_read.lna_state, rtr_lna_state);

  /* Check if RF 1x Logging is enabled*/
  if( log_status(LOG_1X_RF_WARMUP_C) )
  {

    /* Calculate size of Log packet */
    length =  sizeof( rf_1x_log_rf_warmup_log_pkt );

    /* Allocate memory */
    rf_wu_log_pkt = (rf_1x_log_rf_warmup_log_pkt*) log_alloc(LOG_1X_RF_WARMUP_C, 
                                                              length);
    /*Populate fields and Commit Log*/
    if( rf_wu_log_pkt != NULL )
    {

      /*Populate fields in header of the sub-packet*/

      /*Sub packet ID*/
      rf_wu_log_pkt->wu_time_sub_pkt.header.pkt_id = RF_1X_WU_LOG_SUB_PKT_ID;

      /*Sub packet version*/
      rf_wu_log_pkt->wu_time_sub_pkt.header.pkt_ver = RF_1X_WU_LOG_SUB_PKT_VER;

      /*Sub packet size*/
      rf_wu_log_pkt->wu_time_sub_pkt.header.pkt_size = 
                                         sizeof(rf_wu_log_pkt->wu_time_sub_pkt);
      
      /*Get the warmup count (number of wake ups)*/
      rf_wu_log_pkt->wu_time_sub_pkt.wu_count = 
                                        dev_status_r->rf_wu_time_1x.sample_size;
       
      /* check for a valid sample size -- more of a precaution, since it is
         used in the denominator later in the API (to calculate average wu 
         time) */ 
      if (rf_wu_log_pkt->wu_time_sub_pkt.wu_count <= 0)
      {
        RF_MSG_1 ( RF_ERROR, "rf_1x_log_wakeup_rx: sample size = %d, should" 
                   " be > 0 !!", rf_wu_log_pkt->wu_time_sub_pkt.wu_count );
      }
      else
      {
        /* Populate current warmup time field*/
        rf_wu_log_pkt->wu_time_sub_pkt.curr_wu_time = 
                                      dev_status_r->rf_wu_time_1x.curr_wu_time;

        /*Just one warmup so far, min,max and current times are equal*/
        if (rf_wu_log_pkt->wu_time_sub_pkt.wu_count == 1)
        {
          rf_wu_log_pkt->wu_time_sub_pkt.curr_wu_time = 
            rf_wu_log_pkt->wu_time_sub_pkt.min_wu_time = 
              rf_wu_log_pkt->wu_time_sub_pkt.max_wu_time = 
                                       dev_status_r->rf_wu_time_1x.curr_wu_time;
        }
        else
        {
          /*Maximum warmup time*/
          rf_wu_log_pkt->wu_time_sub_pkt.max_wu_time = 
                                        dev_status_r->rf_wu_time_1x.max_wu_time;
          /*Minimum warmup time*/
          rf_wu_log_pkt->wu_time_sub_pkt.min_wu_time = 
                                        dev_status_r->rf_wu_time_1x.min_wu_time;
        }

        /* Average warmup time */
        rf_wu_log_pkt->wu_time_sub_pkt.avg_wu_time = 
            (dev_status_r->rf_wu_time_1x.cumulative_rf_time) / 
                                    (rf_wu_log_pkt->wu_time_sub_pkt.wu_count);

        /* Populate warmup time histogram*/
        for( ctr = 0; ctr < RF_1X_WU_LOG_NUM_HIST_BINS ; ctr++ )
        {
          rf_wu_log_pkt->wu_time_sub_pkt.histogram_wu_time[ctr] = 
                             dev_status_r->rf_wu_time_1x.histogram_wu_time[ctr];
        }/*for( ctr = 0; ctr < RF_1X_WU_LOG_NUM_HIST_BINS ; ctr++ )*/

        /* Packet version */
        rf_wu_log_pkt->rf_header.pkt_version = RF_1X_WU_LOG_PKT_VER; 
         
        /* Number of subpackets */
        rf_wu_log_pkt->rf_header.num_sub_pkt = RF_1X_WU_LOG_NUM_SUB_PKTS;
  
        /* Device associated with warmup log */
        rf_wu_log_pkt->wu_time_sub_pkt.device = device; 
        
        /*Populate RxAGC field*/
        rf_wu_log_pkt->wu_time_sub_pkt.rx_agc = rx_agc_read.rx_agc;
        
        /*Populate LNA state field*/
        rf_wu_log_pkt->wu_time_sub_pkt.lna_state = rx_agc_read.lna_state;

        /*Populate LNA gain word field*/
        rf_wu_log_pkt->wu_time_sub_pkt.lna_gain = rtr_lna_state;

        /*Populate expected warmup time field*/
        rf_wu_log_pkt->wu_time_sub_pkt.expected_wu_time = 
                                               expected_wu_time.prep_time_us +
                                               expected_wu_time.exec_time_us ;

        /*Commit Log packet*/
        log_commit( ( rf_1x_log_rf_warmup_log_pkt* ) rf_wu_log_pkt );

      }/*if !(rf_wu_log_pkt->wu_time_sub_pkt.wu_count <= 0)*/
    }/* if( rf_wu_log_pkt != NULL ) */
    else
    {
      RF_MSG_1( RF_ERROR, "rf_1x_log_wakeup_rx: Unable to allocate Log memory" 
                "for device : %d", device );
    }/* if( rf_wu_log_pkt == NULL  ) */
  }/*if!( rf_cdma_log_is_enabled(LOG_1X_RF_WARMUP_C) )*/

  return;
} /* rf_1x_log_wakeup_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Log (through Diag) a small amount of 1x samples.

  @details
  If the appropriate RF log packet is enabled, this function will copy recent
  samples from the specified device into a log buffer and send it to Diag.

  This log is intended to be generated during sleep wakeup.  The sample length
  is @ref RF_1X_SAMPLE_LOG_SAMPLE_COUNT, and is based roughly on double the
  expected AGC acquisition time, so the log can be used to verify the gain
  stepping during acquisition.

  @param device
  The RF device for which the samples should be logged.
*/
void
rf_1x_log_samples
(
  rfm_device_enum_type device
)
{
  if ( log_status( LOG_1X_RF_RX_QUALITY_C ) )
  {
    cdma_sample_log_type* log_buffer;

    RF_MSG( RF_LOW, "Logging 1x samples" );

    log_buffer = log_alloc( LOG_1X_RF_RX_QUALITY_C,
                            sizeof(struct cdma_sample_log_s) );
    if ( log_buffer != NULL )
    {
      rf_cdma_msm_sample_type* sample_buffer = log_buffer->samples;

      log_buffer->rf_header.pkt_version = 1;
      log_buffer->rf_header.num_sub_pkt = 1;
      log_buffer->rf_header.hdr_reserved = 0;
      log_buffer->rf_subpacket_header.pkt_id = 18;
      log_buffer->rf_subpacket_header.pkt_ver = 0;
      log_buffer->rf_subpacket_header.pkt_size =
        RF_1X_SAMPLE_LOG_SAMPLE_COUNT + 4;

      rf_1x_msm_get_last_n_samples( device, RF_1X_SAMPLE_LOG_SAMPLE_COUNT,
                                    sample_buffer );

      log_commit( log_buffer );
    } /* if ( log_buffer != NULL ) */
  } /* if ( log_status( LOG_RF_DEV_C ) ) */
} /* rf_1x_log_samples() */
#endif /* FEATURE_CDMA1X */
/*! @} */


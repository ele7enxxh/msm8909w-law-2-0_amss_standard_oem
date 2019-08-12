/*!
   @file
   rf_hdr_log.c

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/src/rf_hdr_log.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/07/13   zhw     Split 1x/HDR wu_time logging structures
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
12/27/12   shb     Updates for different physical rx_device per band
08/30/12   bmg     Added sample logging
07/17/12   spa     Populating the expected warmup time to 15ms using the #define 
06/11/12   spa     Added warmup time histogram to log packet 
05/10/12   spa     Moved RxAGC and LNA stage F3 outisde log_enabled check  
04/19/12   spa     Replaced writable device pointer with read only pointer 
04/18/12   spa     Initial version

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_hdr_log.h"
#include "rfm_hdr.h"
#include "rf_cdma_data.h"
#include "rfcommon_msg.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_msm.h"
#include "rf_hdr_mdsp.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Number of samples to include in an HDR sample log packet.

  @details
  This size is based roughly on the maximum log packet length, and will
  capture 1 and a half half-slots of data.
*/
#define RF_HDR_SAMPLE_LOG_SAMPLE_COUNT 3072

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Log structure for a simple HDR sample capture log packet.
*/
typedef struct hdr_sample_log_s
{
  /*! RF Logging common header. */
  rfcommon_log_rf_header_type     rf_header;

  /*! RF Logging subpacket common header. */
  rf_cdma_log_sub_pkt_header_type rf_subpacket_header;

  /*! @ref RF_HDR_SAMPLE_LOG_SAMPLE_COUNT samples of sample capture data. */
  rf_cdma_msm_sample_type         samples[RF_HDR_SAMPLE_LOG_SAMPLE_COUNT];
} hdr_sample_log_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate RF HDR warmup log packet and submit it 

  @details
  This function takes the current device as a parameter and populates the RF HDR 
  warmup log packet. Queries for the enhanced warmup time, the RxAGC, LNA state
  and all stats related to warmup

  @param device
  The RF device on which the warmup is performed.
*/

void
rf_hdr_log_wakeup_rx
(
  rfm_device_enum_type device
)
{
  /* RxAGC Read */
  rf_hdr_mdsp_rx_agc_read_type rx_agc_read;
   
  /* LNA state as read from RF device */
  uint8 rtr_lna_state; 

  /* Initialize Header for log */
  rf_hdr_log_rf_warmup_log_pkt *rf_wu_log_pkt = NULL;

  /* To hold size of Log Packet*/
  unsigned int length;

  /* Counter for histogram loop*/
  unsigned int ctr;

  /* get a valid device pointer */
  const rf_cdma_data_status_type* dev_status_r = 
                                            rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  const rfc_cdma_logical_device_params_type *logical_dev = 
                                rfc_cdma_get_logical_device_params( device );

  if ( dev_status_r == NULL || ( logical_dev == NULL ) )
  {
    /* Error F3 */
    RF_MSG_1(RF_ERROR,"rf_hdr_log_wakeup_rx:NULL data for device %d",device);
    return;
  }

  /* Get Final RxAGC/ LNA State */
  rf_hdr_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_read );

  rx_agc_read.rx_agc[0] = 
                         rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                   rx_agc_read.rx_agc[0]);
  rx_agc_read.rx_agc[1] = 
                         rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                   rx_agc_read.rx_agc[1]);
  rx_agc_read.rx_agc[2] = 
                         rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                   rx_agc_read.rx_agc[2]);
  rx_agc_read.rx_agc[3] = 
                         rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                   rx_agc_read.rx_agc[3]);

  /* Get LNA state from RF device */
  rfdevice_cdma_rx_cmd_dispatch( logical_dev->rx_device[dev_status_r->curr_band], 
                                 RFDEVICE_GET_LNA_STATE,
                                 &rtr_lna_state);

  /* Report Band Chan information and LNA state */
  RF_MSG_7( RF_MED, "rf_hdr_log_wakeup_rx: Dev %d, RxLM %d, Band %d, "
            "Chan %d, SSMA_Chan %d, LNA (FW) %d, LNA "
            "(RTR) 0x%x", device, dev_status_r->rxlm_handle,  
            dev_status_r->curr_band, 
            dev_status_r->curr_chans[RFM_CDMA_CARRIER_0],
            dev_status_r->lo_tuned_to_chan, rx_agc_read.lna_state, 
            rtr_lna_state);

  /* Report final Rx AGC */
  RF_MSG_5( RF_MED, "rf_hdr_log_wakeup_rx : Dev %d, "
            "RxAGC0 %d (dBm10), RxAGC1 %d (dBm10), "
            "RxAGC2 %d (dBm10), RxAGCAll %d (dBm10)",
            device, rx_agc_read.rx_agc[0], rx_agc_read.rx_agc[1], 
            rx_agc_read.rx_agc[2], rx_agc_read.rx_agc[3]);

    /* Check if RF hdr Logging is enabled*/
  if( log_status(LOG_HDR_RF_WARMUP_C) )
  {

    /* Calculate size of Log packet */
    length =  sizeof( rf_hdr_log_rf_warmup_log_pkt );

    /* Allocate memory */
    rf_wu_log_pkt = (rf_hdr_log_rf_warmup_log_pkt*) 
                                         log_alloc(LOG_HDR_RF_WARMUP_C, length);
    /*Populate fields and Commit Log*/
    if( rf_wu_log_pkt != NULL )
    {

      /*Populate fields in header of the sub-packet*/

      /*Sub packet ID*/
      rf_wu_log_pkt->wu_time_sub_pkt.header.pkt_id = RF_HDR_WU_LOG_SUB_PKT_ID;

      /*Sub packet version*/
      rf_wu_log_pkt->wu_time_sub_pkt.header.pkt_ver = RF_HDR_WU_LOG_SUB_PKT_VER;

      /*Sub packet size*/
      rf_wu_log_pkt->wu_time_sub_pkt.header.pkt_size = 
                                         sizeof(rf_wu_log_pkt->wu_time_sub_pkt);
      
      /*Get the warmup count (number of wake ups)*/
      rf_wu_log_pkt->wu_time_sub_pkt.wu_count = 
                                       dev_status_r->rf_wu_time_hdr.sample_size;
       
      /* check for a valid sample size -- more of a precaution, since it is
         used in the denominator later in the API (to calculate average wu 
         time) */ 
      if (rf_wu_log_pkt->wu_time_sub_pkt.wu_count <= 0)
      {
        RF_MSG_1 ( RF_ERROR, "rf_hdr_log_wakeup_rx: sample size = %d, should" 
                   " be > 0 !!", rf_wu_log_pkt->wu_time_sub_pkt.wu_count );
      }
      else
      {
        /* Populate current warmup time field*/
        rf_wu_log_pkt->wu_time_sub_pkt.curr_wu_time = 
                                      dev_status_r->rf_wu_time_hdr.curr_wu_time;

        /*Just one warmup so far, min,max and current times are equal*/
        if (rf_wu_log_pkt->wu_time_sub_pkt.wu_count == 1)
        {
          rf_wu_log_pkt->wu_time_sub_pkt.curr_wu_time = 
            rf_wu_log_pkt->wu_time_sub_pkt.min_wu_time = 
              rf_wu_log_pkt->wu_time_sub_pkt.max_wu_time = 
                                      dev_status_r->rf_wu_time_hdr.curr_wu_time;
        }
        else
        {
          /*Maximum warmup time*/
          rf_wu_log_pkt->wu_time_sub_pkt.max_wu_time = 
                                       dev_status_r->rf_wu_time_hdr.max_wu_time;
          /*Minimum warmup time*/
          rf_wu_log_pkt->wu_time_sub_pkt.min_wu_time = 
                                       dev_status_r->rf_wu_time_hdr.min_wu_time;
        }

        /* Average warmup time */
        rf_wu_log_pkt->wu_time_sub_pkt.avg_wu_time = 
            (dev_status_r->rf_wu_time_hdr.cumulative_rf_time) / 
                                    (rf_wu_log_pkt->wu_time_sub_pkt.wu_count);

        /* Populate warmup time histogram*/
        for( ctr = 0; ctr < RF_HDR_WU_LOG_NUM_HIST_BINS ; ctr++ )
        {
          rf_wu_log_pkt->wu_time_sub_pkt.histogram_wu_time[ctr] = 
                            dev_status_r->rf_wu_time_hdr.histogram_wu_time[ctr];
        }/*for( ctr = 0; ctr < RF_HDR_WU_LOG_NUM_HIST_BINS ; ctr++ )*/

        /* Packet version */
        rf_wu_log_pkt->rf_header.pkt_version = RF_HDR_WU_LOG_PKT_VER; 
         
        /* Number of subpackets */
        rf_wu_log_pkt->rf_header.num_sub_pkt = RF_HDR_WU_LOG_NUM_SUB_PKTS;
  
        /* Device associated with warmup log */
        rf_wu_log_pkt->wu_time_sub_pkt.device = device; 
        
        /*Populate RxAGC fields*/
        rf_wu_log_pkt->wu_time_sub_pkt.rx_agc[0] = rx_agc_read.rx_agc[0];
        
        rf_wu_log_pkt->wu_time_sub_pkt.rx_agc[1] = rx_agc_read.rx_agc[1];

        rf_wu_log_pkt->wu_time_sub_pkt.rx_agc[2] = rx_agc_read.rx_agc[2];

        rf_wu_log_pkt->wu_time_sub_pkt.rx_agc[3] = rx_agc_read.rx_agc[3];

        /*Populate LNA state field*/
        rf_wu_log_pkt->wu_time_sub_pkt.lna_state = rx_agc_read.lna_state;

        /*Populate LNA gain word field*/
        rf_wu_log_pkt->wu_time_sub_pkt.lna_gain = rtr_lna_state;

        /*Populate expected warmup time field using pre-defined value*/
        rf_wu_log_pkt->wu_time_sub_pkt.expected_wu_time = 
                                               RF_HDR_WU_LOG_EXPECTED_WU_TIME;

        /*Commit Log packet*/
        log_commit( ( rf_hdr_log_rf_warmup_log_pkt* ) rf_wu_log_pkt );

      }/*if !(rf_wu_log_pkt->wu_time_sub_pkt.wu_count <= 0)*/
    }/* if( rf_wu_log_pkt != NULL ) */
    else
    {
      RF_MSG_1( RF_ERROR, "rf_hdr_log_wakeup_rx: Unable to allocate Log memory" 
                "for device : %d", device );
    }/* if( rf_wu_log_pkt == NULL  ) */
  }/*if!( rf_cdma_log_is_enabled(LOG_hdr_RF_WARMUP_C) )*/

  return;
} /* rf_hdr_log_wakeup_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Log (through Diag) a small amount of HDR samples from one narrowband carrier.

  @details
  If the appropraite log packet is enabled, this function will generate an RF
  log packet containing the most recent set of HDR samples.  The last sample
  is the log packet will be the current sample as of the time at which this
  function is called.

  HDR supports multi-carrier, so the caller must specify which carrier is to
  be logged.  This funciton will not automatically log all active carriers,
  just the one that is requested.

  This log is intended to be generated during sleep wakeup.  The sample length
  is @ref RF_HDR_SAMPLE_LOG_SAMPLE_COUNT, and is primarily limited by the
  maximum Diag buffer length.  This will capture one and a half HDR half-slots
  of data, and generally only captures the last gain step update.

  @param device
  The RF device that the samples should be taken from.

  @param carrier
  The carrier index that the samples should be taken from.
*/
void
rf_hdr_log_samples
(
  rfm_device_enum_type device,
  rfm_cdma_carrier_type carrier
)
{
  if ( log_status( LOG_HDR_RF_RX_QUALITY_C ) )
  {
    hdr_sample_log_type* log_buffer;

    RF_MSG( RF_LOW, "Logging HDR samples" );

    log_buffer = log_alloc( LOG_HDR_RF_RX_QUALITY_C,
                            sizeof(hdr_sample_log_type) );
    if ( log_buffer != NULL )
    {
      rf_cdma_msm_sample_type* sample_buffer = log_buffer->samples;

      log_buffer->rf_header.pkt_version = 1;
      log_buffer->rf_header.num_sub_pkt = 1;
      log_buffer->rf_header.hdr_reserved = 0;
      log_buffer->rf_subpacket_header.pkt_id = 18;
      log_buffer->rf_subpacket_header.pkt_ver = 0;
      log_buffer->rf_subpacket_header.pkt_size =
        RF_HDR_SAMPLE_LOG_SAMPLE_COUNT + 4;

      rf_hdr_msm_get_last_n_samples( device, carrier,
                                     RF_HDR_SAMPLE_LOG_SAMPLE_COUNT,
                                     sample_buffer );

      log_commit( log_buffer );
    } /* if ( log_buffer != NULL ) */
  } /* if ( log_status( LOG_RF_DEV_C ) ) */
} /* rf_hdr_log_samples() */
#endif /* FEATURE_CDMA1X */
/*! @} */


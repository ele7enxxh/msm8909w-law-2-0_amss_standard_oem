#ifndef RFTXPLIM_MM_REPORT_TX_POWER_H
#define RFTXPLIM_MM_REPORT_TX_POWER_H

/*!
   @file rftxplim_mm_report_tx_power.h

   @brief
   This file contains structure and function prototype for acquring TX power
   status from remote RTR

   @details

*/

/*===========================================================================
Copyright (c) 2010 - 2011  by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/sv/common/inc/rftxplim_mm_report_tx_power.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/10/11   Saul    SV Limiting. Initial support.
06/16/11   sar     Included internal rf hdr interface.
05/04/11   pl      SV-LTE SAR non-RPC implementation
05/04/11   pl      remove use of rfm_mode
10/14/10   kma     Fixed compilation warnings
09/13/10   kma     Fixed the boot up race condition on Fusion
07/16/10   kma     Exported rfsv_send_tx_measurement_report_proxy api
07/13/10   kma     Fixed TX power resolution bug
07/12/10   kma     Added FW support to acquire total filtered TX power
06/28/10   kma     Added API to convert agc to dbm for 1X TX power
                   Moved RPC API to a a separate file
06/08/10   kma     Initial version
============================================================================*/

#include "rfcom.h"
#include "rftxplim_mm_adjust_tx_power.h"
#include "rfm_hdr.h"
#include "timer.h"


/* Milliseconds of delay between Tx power limit adjustments in CDMA mode
*/
#define RF_TX_MEASUREMENT_REPORT_REPEAT_TIME    20

/*Global structure for tracking TX profile*/
typedef struct {
  rfm_device_enum_type      device;
  boolean                   tx_on;
  rfm_mode_enum_type        rf_mode;
  rf_card_band_type         rf_band;
  uint32                    num_band_chan; 
  uint32                    band_chan_array[MAX_NUM_CHANNELS_SUPPORTED]; 
} rfsv_data_tech_tx_status_type;

void rfsv_20_ms_isr( void );

void rfsv_start_voice_timer( rfsv_data_tech_tx_status_type voice_tech_tx_status );

void rfsv_stop_voice_timer( void );

void rfsv_send_tx_update_notification(  boolean                   tx_on,
                                        rfm_mode_enum_type        rf_mode,
                                        rf_card_band_type         rf_band,
                                        uint32                    num_band_chan,
                                        uint32                    band_chan_array[MAX_NUM_CHANNELS_SUPPORTED],
                                        boolean                   packet_check_override);

#endif /* RFTXPLIM_MM_REPORT_TX_POWER_H */


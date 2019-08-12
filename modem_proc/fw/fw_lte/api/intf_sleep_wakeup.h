/*!
  @file
  intf_sleep.h

  @brief
  This file contains data structures for sleep and wakeup requests
  and confirmations.
  
 
*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_sleep_wakeup.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef INTF_SLEEP_H
#define INTF_SLEEP_H

#include "intf_common.h"
#include "intf_config_app.h"

#include "lte_l1_types.h"
#include "lte_LL1_ue_types.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/

   

/*===========================================================================

      Typedefs

===========================================================================*/


// --------------------------------------------------------------------------
//
// Sleep Request
//
// --------------------------------------------------------------------------

/*! @brief  Sleep request message structure.
            This request is issued to start sleep.  This request 
            automatically stops all ue rx entities.  RX RF should not be
            disabled until after confirmation for request has been sent.
            Sleep and resource allocation mode change takes effect at the
            start of next (action time) subframe.
*/
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;      ///< common req header
  
  lte_LL1_sys_time_struct wakeup_sys_time;  ///< Wakeup System+Sub frame number
  
  lte_LL1_ue_res_mode_e   resource_mode;    ///< resource allocation mode

  uint8                   carrier_bitmask;  ///< Carrier bitmask to indicate all the valid/active carriers .
  uint16                  sleep_mode;       ///< bit0: deep sleep; bit1: deep light sleep
} lte_LL1_sys_sleep_req_struct;

LTE_LL1_INTF_MSG( sys_sleep_req );


/*! @brief  Sleep confirmation message structure.
            This message is sent after LL1 receives sleep request 
            and has stop all rx entities.
*/
typedef struct { 

  lte_LL1_carrier_type_enum carrier_type;  ///< Enum for dl carriers in  CA
  
  int32   rot_freq_correction;            ///< Most recent frequency correction at the rotator.
                                          ///< Q-factor: 32S12

  int32   vco_freq_correction;            ///< Most recent frequency correction at the VCO.
                                          ///< Q-factor: 32S12
    
  uint32  ftl_snr[LTE_LL1_MAX_NUM_UE_RX_ANT];                     ///< FTL SNR (linear in 32Q16)

  lte_LL1_rxagc_start_params_s  rxagc_curr_params[2];  //< current AGC param values
  boolean rxagc_can_be_resumed_flag;  ///< Rx-AGC ONL_DRX mode can be sent as RESUME 
    ///< instead of START in the next run of the current Rx-AGC mode
    
  /*! @brief DCC accum report table; one for each rx chain*/
  lte_LL1_dcc_accum_cfg_table_s dcc_table[LTE_MAX_NUM_UE_RX_ANT_PER_CAR];

  uint64  last_doppler_updated_rtc;       ///< RTC count when Doppler was last updated in FW  
}lte_LL1_sys_sleep_carrier_payload_struct;
 
typedef struct {
  lte_LL1_cnf_header_struct cnf_hdr;      ///< common cnf header
  
  uint64  sleep_start_time;               ///< Reference time at start of sleep
  
  
  uint32  sleep_duration;                 ///< The time from STMR stop to the 
                                          ///< requested system frame and subframe 
                                          ///< in Ts units. 
                                          
  uint32  pre_stmr_setup;                 ///< setup time (in Ts units) needed 
                                          ///< for resuming STMR.  
                                          
  uint32  decode_offset_online;           ///< STMR on to 1st subframe to decode in
                                          ///< on-line mode in Ts units
                                          
  uint32  decode_offset_offline;          ///< STMR on to 1st subframe to decode in
                                          ///< off-line mode in Ts units

  uint16  sfn;                            ///< sfn that corresponds with 
                                          ///< frame_timing_offset

  uint8   subframe_index;                 ///< Sub-frame index (0-9)
  uint32  frame_timing_offset;            ///< Most recent DL frame timing offset 
                                          ///< in Ts units relative to STMR.  
                                          ///< Range is 0 ~ 307199
                                          
  int32   rot_freq_correction;            ///< Most recent frequency correction at the rotator.
                                          ///< Q-factor: 32S16

  int32   vco_freq_correction;            ///< Most recent frequency correction at the VCO.
                                          ///< Q-factor: 32S16
  int32   ftl_il_disc;                    ///< il discrimnant 32S19
  boolean freeze_ftl;                     ///< True if FW detects large frequency FTL and freezes FTL loop
  
  uint8   carrier_bitmask;                ///< Carrier bitmask to specif valid/active carriers .
  uint16  sleep_mode;                     ///< bit0: deep sleep; bit1: deep light sleep

  uint16  dyn_odrx_effective_opts;        ///< Dyn ODRX Effective optimizations 
                                          ///< bit0 : 1=> Opt 1a happened (always)
                                          ///< bit1: 1=> Opt 2 took effect, 2nd FTL stage was skipped
                                          ///< bit2: 1=> Opt 3 took effect, 1st FTL stage was skipped

  lte_LL1_sys_sleep_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS]; ///< struct array  for CA.
}lte_LL1_sys_sleep_cnf_struct;

LTE_LL1_INTF_MSG( sys_sleep_cnf );


// --------------------------------------------------------------------------
//
// Wakeup Request
//
// There is no dedicated message structure for wakeup request.
// Use lte_LL1_async_config_app_req_struct
// --------------------------------------------------------------------------


#endif


/*!
  @file
  intf_config_app.h

  @brief
  This file contains data structures for config app 
  requests and confirmations.
 
*/

/*===========================================================================

  Copyright (c) 2009-10 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_config_app.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef INTF_CONFIG_APP_H
#define INTF_CONFIG_APP_H

#include "intf_common.h"
#include "lte_l1_types.h"
#include "lte_LL1_ue_types.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/
/*! @brief
  TDD Pipeline GCB(gain circular buffer) related constants used in config
  app req and conf.
 */
#define LTE_LL1_SYS_PIPELINE_BUFFER_INITIAL_GAIN_STATE 2
#define LTE_LL1_SYS_PIPELINE_BUFFER_ENTRY_BITWIDTH 3
#define LTE_LL1_SYS_PIPELINE_BUFFER_NUM_ENTRIES 10
#define LTE_LL1_SYS_PIPELINE_INVALID_PIPELINE_TIMING_REF 0xFFFFFFFF      

/*! @brief
  TDD Pipeline X2L idle related constants used in config app req and conf.
 */
#define LTE_LL1_TDD_PIPELINE_SCHEDULING_OFFSET 30600 ///< offset between TDD pipeline 
                                      ///< interrupt and DL subframe in
                                      ///< Ts units
#define LTE_LL1_TDD_PIPELINE_START_OFFSET_X2L_IDLE_GAP (1*30720-LTE_LL1_TDD_PIPELINE_SCHEDULING_OFFSET)
/*===========================================================================

      Typedefs

===========================================================================*/

// --------------------------------------------------------------------------
//
// sys & async config app
//
// --------------------------------------------------------------------------

/*! @brief
  TDD Pipeline AGC state structure

  */
typedef struct
{
  /*! Pipeline timing reference in RTC ticks % 10ms 
   
    The reference time of entry 0 in the pipeline_buffer
   */
  uint32 pipeline_timing_ref;


  /*! Each index represents the pipeline buffer for it's respective
    antenna.

    Buffer is essentially a compact array of pipeline state entries.
    The buffer will begin at the Least significant bit.  

    Each entry is of width LTE_LL1_SYS_PIPELINE_BUFFER_ENTRY_BITWIDTH 
    and the buffer contains LTE_LL1_SYS_PIPELINE_BUFFER_NUM_ENTRIES entries

    32       24       16        8        0
    |---------|--------|--------|--------|
        |9 |8  |7 |6 |5  |4 |3 | 2 |1 |0 |


    */
  uint32 pipeline_buffer[2];

} lte_LL1_pipeline_agc_state_s;

/*! @brief  config app request message structure.
            This request is issued to configure LTE app, which consists of
            the following controls:
            - module enabling/disabling/resuming
            - resource mode configuration
            - processing mode configuration             
            
            Enable restrictions:
            - sys time must be enabled in order to enable any other entity
            - MSTMR/WB sync cannot be enabled without sample recording enabled
            - TTL/FTL requires channel estimation to be enabled
            
            Mode change restrictions:
            - on-line/off-line transitions can only occur with stopped sample
              recording.
            - Only system time can be enabled in LTE_LL1_UE_RES_MODE_LTE_IDLE mode.
            - sample recording, AGC, TTL, FTL, channel est must be stopped upon
              entering LTE_LL1_UE_RES_MODE_LTE_IRAT mode
              
            Initial settings:
            Initial mode proc mode is on-line.
            Initial res mode is LTE_LL1_UE_RES_MODE_NO_LTE
            
            Sampling rate selection:
             for 20MHz BW, use sampling rate of 30.72MHz
              "  15MHz  "   "     "       "   " 30.72MHz
              "  10MHz  "   "     "       "   " 15.36MHz
              "  5MHz   "   "     "       "   "  7.68MHz
              
*/
typedef struct
{                            
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
                            
  uint16  enable_mask;      ///< OR of lte_LL1_ue_rx_mod_e values 
                            ///< to indicate what entity(s) to be enabled

  uint16  disable_mask;      ///< OR of lte_LL1_ue_rx_mod_e values 
                             ///< to indicate what entity(s) to be disabled
                                                        
  uint16  resume_mask;      ///< OR of lte_LL1_ue_rx_resumable_mod_e values 
                            ///< to indicate what entity(s) to resume.  

  /*! @brief  Parameters used when enabling sample recording */
  lte_LL1_ue_wb_samp_rate_e wb_samp_rec_rate_sel;     ///< WB sampling rate select

  /*! @brief  Parameters used when recording into the secondary LTE sample server */
  lte_LL1_samp_serv_s       samp_serv;

  /*! @brief  Parameters used when enabling AGC */
  lte_LL1_rxagc_mode_e    rxagc_start_mode;           ///< agc mode

  uint32                  two_pwr_33_over_dl_freq;    ///< 2^33/dl_freq
                                                      ///< Q-factor: Q19;
                                                      ///< It is needed becaue dl_carrier_freq
                                                      ///< does not have enough accuracy
  int32                   start_rot_freq_corr;        ///< Starting FTL frequency 
                                                      ///< correction at the rotator
                                                      ///< Q-factor: In 32S16
  int32                   start_vco_freq_corr;        ///< Starting VCO frequency 
                                                      ///< correction at the VCO
                                                      ///< Q-factor: In 32S16

  /*! @brief Parameters used for antenna correlation */
  uint16                  reporting_period;           ///< reporting period in ms units

  uint8                   slam_rot;                   ///< Slam the rotator 

/* Changes for online DRX aGc */
  uint8   agc_drx_step1_cnt;                         // Subframe count with FASt time constant
  uint8   agc_drx_step2_cnt;                         // Subframe count with Slow time constant
  uint8                   rxagc_start_lna_state[2];      ///< starting AGC LNA state
  lte_LL1_rxagc_start_params_s  rxagc_start_params[2];  //< starting AGC param values
  uint8   reset_dcc_start_stop_time_flag; ///< Reset the DC accum START/STOP times (current
    // use case is DCC START/STOP time reset after RF SW api used for retune)
  lte_l1_cell_duplex_e        duplexing_mode;          //< FDD/TDD indication before the Scell is configured 
                                                       // (should be same as Scell after camped on it)
  uint32 tdd_x2l_vcell_frame_boundary;                 // location of the VCell frame_boundary %307200 for TDD X2L idle AGC
  boolean                     rf_wakeup_mode_enabled;  ///< RF chip (RTR/WTR) wakeup mode is enabled
  lte_LL1_rf_wakeup_rx_on_e   rf_wakeup_rx_on;         ///< 1: turn on Rx, 0: turn off Rx via GRFC; 
                                                       ///< Note: only applicable if rf_wakeup_mode_enabled is 1.
  lte_LL1_pipeline_agc_state_s pipeline_agc_state;   ///< pipeline agc ref time and GCB buffer 
  
  /* DCC accum table; Only when (accum_table_valid is set AND the rxagc is enabled),
    the table is taken by the LL1 */
  boolean accum_table_valid;
  /*! @brief DCC accum value table; one for each rx chain*/
  lte_LL1_dcc_accum_cfg_table_s dcc_table[LTE_MAX_NUM_UE_RX_ANT_PER_CAR];
  boolean                       is_intra_band_ca;		   ///< flag indicating whether this is intra ca mode.
  lte_LL1_carrier_type_enum     rxagc_share_carrier_lna;   ///< used to signal to a carrier to share the LNA from another carrier 

  lte_LL1_ue_wb_df_mode_e wb_df_mode;     ///< WB DF receiver operation mode
  uint32                        shared_rf_chain_bitmap; ///< bit-field indicating if the RF chain is shared with another carrier/cell
                                                /// based on lte_LL1_carrier_type_enum
                                                /// At Minimum , the bit mask for the current carrier should be set.
                                                /// If more than 1 bit is set, FW will process the carriers corresponding to  
                                                /// the set bit as single Rx with the corresponding carriers.
                                                /// valid bitwidth = LTE_LL1_CARRIER_COUNT
                                               
  
} lte_LL1_sys_config_app_carrier_payload_t;

typedef struct
{
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for carriers to be supported for CA
 
  lte_LL1_nb_id_enum nb_id;  ///< Enum for NB id mapping

  uint8 rxlm_buffer_id_ant0; ///< The WB buffer id that the NB is connected to.
  
  uint8 rxlm_buffer_id_ant1; ///< The WB buffer id that the NB is connected to.
 
}lte_LL1_sys_nb_mux_settings_t;


typedef struct
{
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for carriers to be supported for CA

  uint8  hw_res_release_mask;  ///< Mask for resources to be released for this carrier, from lte_LL1_resource_e

  uint8  hw_res_acquire_mask; ///< Mask for resources to be released for this carrier, from lte_LL1_resource_e

} lte_LL1_sys_hw_res_req_carrier_t;

typedef struct
{
  uint8 num_carriers; ///< Number of carriers to be configured.

  lte_LL1_sys_hw_res_req_carrier_t hw_res_req[LTE_LL1_DL_NUM_CARRIERS]; ///< HW Resource rel/acq info per carrier.

} lte_LL1_sys_hw_res_req_payload_t;


typedef struct
{
  lte_LL1_ue_res_mode_e   resource_mode;              ///< resource allocation mode

  lte_LL1_ue_proc_mode_e  proc_mode;                  ///< processing mode
                                                      ///< (on-line or off-line)
                   
  lte_LL1_ue_mode_sys_e sys_mode;              ///< start, resume, stop (used to be LTE_LL1_UE_MOD_SYS_TIME) 

  lte_LL1_ue_df_enable_mode_e enable_mode;            ///< demfront enabling mode

  /*! @brief  Parameters used when enabling/resuming system time */
  lte_l1_cell_systime_s  sys_time_start_cell_time;    ///< starting cell time
  
  uint32                 sys_time_resume_elapsed_time;///< elapse time in Ts units for
                                                      ///< use with sys time resume
													  
  boolean                 not_ftm_mode_flag; ///< FTM mode indication (for RFCal)
  // NOTE: ML populates this with NOT_FTM=1 ALWAYS. RFSW implicitly sets FTM=1 by memsetting
  //CFG_APP payload to 0 

  lte_LL1_gap_type_e      gap_type;

  boolean dyn_odrx_optimizations_enable; ///<1/0: Ena/Dis for Dyn ODRX optimizations 
  
  uint8 num_nbs;
  lte_LL1_sys_nb_mux_settings_t  nb_switch_params[LTE_LL1_NB_ID_COUNT];

  boolean                       dual_demod_is_active; //enable in SCC_0 to enter into dual demod mode
  lte_LL1_dual_demod_map_struct dual_demod_mapping; //mapping setting when dual demod is enabled
  
  lte_LL1_sys_hw_res_req_payload_t hw_res_req_params;
}lte_LL1_sys_config_app_common_payload_t;

/*! @brief Config APP struct for CA

*/
typedef struct
{
  lte_LL1_req_header_struct req_hdr;            
  uint8 num_carriers; ///< Number of carriers to be configured.
  lte_LL1_sys_config_app_common_payload_t common;   ///<  Common request parameters
  lte_LL1_sys_config_app_carrier_payload_t carrier[LTE_LL1_DL_NUM_CARRIERS]; ///< parameters per CA.
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  lte_LL1_ant_setting_enum_t  ant_setting;             ///< ant setting
#endif
}
lte_LL1_sys_config_app_req_struct;

LTE_LL1_INTF_MSG( sys_config_app_req );


/*! @brief  Configure app confirmation message structure.
            This message is sent after LL1 starts the specified entities
            in the configure rx start request message.
*/
typedef struct {                      
  
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA

  uint16  ref_sfn;                        ///< SFN of the last radio frame 
                                          ///< corresponding to passed frame ref time

  uint64  ref_time;                       ///< 64 bit RTC time of last radio frame
                                          ///<  start corresponding to ref_sfn above 
                                          ///<  i.e. pair [ref time, SFN]

  int32   rot_freq_correction;            ///< Most recent frequency correction at the rotator
                                          ///< in Q12. Divide by 2^12 to get value in Hz.

  int32   vco_freq_correction;            ///< Most recent VCO frequency correction in Q12.
                                          ///< Divide by 2^12 to get value in Hz.
  
  uint8   lna_state;                      ///< most recent LNA state  
  lte_LL1_rxagc_start_params_s  rxagc_curr_params[2];  ///< current AGC param values
  lte_LL1_pipeline_agc_state_s pipeline_agc_state; ///< pipeline agc ref time and GCB buffer 

  /*! @brief DCC accumulation report; one for each rx chain*/
  lte_LL1_dcc_accum_cfg_table_s dcc_table[LTE_MAX_NUM_UE_RX_ANT_PER_CAR];
  
  uint64  last_doppler_updated_rtc;       ///< RTC count when Doppler was last updated in FW
}lte_LL1_sys_config_app_cnf_carrier_payload_struct;

typedef struct {
  lte_LL1_cnf_header_struct cnf_hdr;      ///< common cnf header
  uint8 num_carriers; ///< Number of carriers to be configured.
  lte_LL1_sys_config_app_cnf_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS]; ///< Cfg_app parameters per CA.
  int16  subframe_num;                   ///< subframe_num when sending config_app_cnf
}lte_LL1_sys_config_app_cnf_struct;

LTE_LL1_INTF_MSG( sys_config_app_cnf );


// --------------------------------------------------------------------------
//
// Async config app structures (for use in async message queue)
//
// --------------------------------------------------------------------------
typedef lte_LL1_sys_config_app_req_struct  lte_LL1_async_config_app_req_struct;

LTE_LL1_INTF_MSG( async_config_app_req );

typedef lte_LL1_sys_config_app_cnf_struct  lte_LL1_async_config_app_cnf_struct;

LTE_LL1_INTF_MSG( async_config_app_cnf );



// XXX alias to older name
typedef lte_LL1_sys_config_app_req_struct  lte_LL1_sys_config_rx_req_struct;

LTE_LL1_INTF_MSG( sys_config_rx_req );

typedef lte_LL1_sys_config_app_cnf_struct  lte_LL1_sys_config_rx_cnf_struct;

LTE_LL1_INTF_MSG( sys_config_rx_cnf );


#endif


#ifndef CXM_LOG_TYPES_H
#define CXM_LOG_TYPES_H
/*!
  @file
  cxm_log_types.h

  @brief
  Packed structures and types for CXM's debug log packet system

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/inc/cxm_log_types.h#1 $ $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/20/15   ckk     Update type7[rrc_c] using RAT layer 3 states 
07/02/14   cdh     Initial implementation

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <atomic_ops.h>
#include <IxErrno.h>
#include <comdef.h>
#include <coexistence_service_v02.h>
#include "coex_algos.h"
#include "log.h"
#include "coex_smem_intf.h"
#include "cxm_antenna.h"
#include "cxm_wcn_tx.h"

/*===========================================================================

                       DATA TYPES AND MACROS

===========================================================================*/
#define CXM_LOG_GSM_TL_SEG_SIZE 4

/* packed version of cxm_state_type */
typedef PACK(struct)
{
  uint32          active_tech_mask;
  uint32          conn_tech_mask;
  uint32          tx_pwr_tech_mask;
  boolean         uart_en_state;
  uint32          uart_handle;
} coex_state_type_pack;

/* packed version of atomic_word_t */
typedef PACK(struct) {
    volatile atomic_plain_word_t value;
} atomic_word_t_pack;

/* packed version of coex_antenna_state_v02 */
typedef PACK(struct) 
{
  uint16_t adv_notice;
  uint16_t duration;
} coex_antenna_state_v02_pack;

/* packed version of coex_tech_sync_state_v02 */
typedef PACK(struct) 
{
  uint8_t id;
  uint8_t sync_state_token;
} coex_tech_sync_state_v02_pack;

/* packed version of cxm_antenna_state_s */
typedef PACK(struct) 
{
  cxm_antenna_state_e           state;
  coex_antenna_state_v02_pack   info;
  coex_tech_sync_state_v02_pack sync;
  boolean                       token_wrap;
  trm_duration_t                grant_dur;
} cxm_antenna_state_s_pack;

/* packed version of cxm_tech_link_info_s */
typedef PACK(struct)
{
  uint32                  frequency; 
  uint32                  bandwidth; 
  uint32                  band;     
  uint32                  channel;   
  cxm_rfdev_type          rf_device;
  cxm_tech_link_direction direction;
  cxm_tech_link_type      type; 
} cxm_tech_link_info_s_pack;

/* packed version of cxm_lte_info_s */
typedef PACK(struct)
{
  lte_l1_frame_struct_e                 frame_structure; 
  lte_l1_tdd_ul_dl_cfg_index_e          tdd_config;      
  lte_l1_tdd_special_subframe_pattern_e ssp;            
  lte_l1_cp_e                           dl_cp;          
  lte_cphy_ul_cyclic_prefix_length_e    ul_cp;          
  uint32                                frame_offset;    
} cxm_lte_info_s_pack;

/* packed version of cxm_tech_specific_params_u */
typedef PACK(union)
{
  cxm_lte_info_s_pack lte_params;
  cxm_tds_info_s      tds_params; 
} cxm_tech_specific_params_u_pack;

/* packed version of cxm_tech_data_s */
typedef PACK(struct)
{
  uint8                           num_link_info_sets;                    
  cxm_tech_link_info_s_pack       link_list[CXM_MAX_SUPPORTED_LINK_SETS]; 
  cxm_tech_specific_params_u_pack params;                                
} cxm_tech_data_s_pack;

/* packed version of coex_wwan_tech_info */
typedef PACK(struct) 
{
  cxm_tech_data_s_pack      state_data;
  boolean                   tech_band_filter[CXM_MAX_SUPPORTED_LINK_SETS];
  uint32                    sleep_thresh;
  uint64_t                  wakeup_tick;
  cxm_tech_conn_state_e     conn_state;
  cxm_tech_sleep_state_e    sleep_state;
  cxm_tech_l3_state_s       l3;
} coex_wwan_tech_info_pack;

/* packed version of coex_tech_band_type */
typedef PACK(struct)
{
  uint32 freq_start;
  uint32 freq_stop;
} coex_tech_band_s_pack;

/* packed version of coex_band_filter_s */
typedef PACK(struct)
{
  boolean filter_on;
  uint32 ul_len;
  uint32 dl_len;
  coex_tech_band_s_pack ul_filter[COEX_WWAN_MAX_BANDS_TO_MONITOR_V01];
  coex_tech_band_s_pack dl_filter[COEX_WWAN_MAX_BANDS_TO_MONITOR_V01];
} coex_band_filter_s_pack;

/* packed version of coex_wwan_band_range_type_v01 */
typedef PACK(struct)
{
  coex_indicate_band_direction_v01 direction;
  coex_wwan_operating_dimension_v01 operating_dim;
  coex_wwan_tech_v01 tech;
  uint32_t freq_start;
  uint32_t freq_stop;
} coex_wwan_band_range_type_v01_pack;

/* packed version of coex_wcn_band_range_type_v01 */
typedef PACK(struct)
{
  coex_wlan_mode_mask_v01 mode;
  coex_wcn_tech_v01 tech;
  uint32_t freq_start;
  uint32_t freq_stop;
} coex_wcn_band_range_type_v01_pack;

/* packed version of coex_wwan_rx_imd_protection_params_type_v01 */
typedef PACK(struct)
{
  uint8_t block_tx;
  uint8_t static_backoff_tx_pwr;
} coex_wwan_rx_imd_protection_params_type_v01_pack;

/* packed version of coex_bt_afh_update_range_type_v01 */
typedef PACK(struct)
{
  uint8_t start_bt_channel;
  uint8_t end_bt_channel;
} coex_bt_afh_update_range_type_v01_pack;

/* packed version of coex_conflict_wcn_params_type_v01 */
typedef PACK(struct)
{
  coex_wwan_rx_imd_protection_params_type_v01_pack imd_protection_params;
  uint32_t bt_afh_update_len; 
  coex_bt_afh_update_range_type_v01_pack bt_afh_update[COEX_MAX_BT_AFH_CHANNEL_RANGE_V01];
} coex_conflict_wcn_params_type_v01_pack;

/* packed version of coex_wwan_tx_thlds_type_v01 */
typedef PACK(struct)
{
  int16_t tx_pwr_thld;
  uint16_t tx_bw_thld;
} coex_wwan_tx_thlds_type_v01_pack;

/* packed version of coex_frame_denial_duty_cycle_params_type_v01 */
typedef PACK(struct)
{
  uint32 max_allowed_frame_denials;
  uint32 frame_denial_window;
} coex_frame_denial_duty_cycle_params_type_v01_pack;

/* packed version of coex_wwan_tx_frame_denials_params_type_v01 */
typedef PACK(struct)
{
  coex_frame_denial_duty_cycle_params_type_v01_pack duty_cycle_params;
  uint32_t max_cont_denials;
} coex_wwan_tx_frame_denials_params_type_v01_pack;

/* packed version of coex_wcn_prio_params_type_v01 */
typedef PACK(struct)
{
  coex_wwan_tx_thlds_type_v01_pack tx_thlds;
  coex_wwan_tx_frame_denials_params_type_v01_pack frame_denials;
} coex_wcn_prio_params_type_v01_pack;

/* packed version of coex_rb_threshold_type_v01 */
typedef PACK(struct)
{
  uint8_t start;
  uint8_t end;
} coex_rb_threshold_type_v01_pack;

/* packed version of coex_tx_advanced_params_type_v01 */
typedef PACK(struct)
{
  uint32_t rb_thresh_len;
  coex_rb_threshold_type_v01_pack rb_thresh[COEX_MAX_RB_THRES_PER_CONFLICT_V01];
  int16_t tx_power_thresh;
} coex_tx_advanced_params_type_v01_pack;

/* packed version of coex_conflict_mdm_params_type_v01 */
typedef PACK(struct)
{
  coex_wcn_prio_params_type_v01_pack wcn_prio;
  coex_tx_advanced_params_type_v01_pack tx_adv;
  coex_pwr_limit_params_type_v01 pwr_lmt;
} coex_conflict_mdm_params_type_v01_pack;

/* packed version of coex_conflict_type */
typedef PACK(struct)
{
  coex_wwan_band_range_type_v01_pack      wwan;
  coex_wcn_band_range_type_v01_pack       wcn;
  coex_policy_config_mask_v01             mdm_policy;
  coex_wcn_policy_config_mask_v01         wcn_policy;
  coex_conflict_wcn_params_type_v01_pack  wcn_params;
  coex_conflict_mdm_params_type_v01_pack  mdm_params;
} coex_conflict_type_pack;

/* packed version of coex_config_params_v8 excluding tables */
typedef PACK(struct)
{
  uint8 version;
  uint8 cxm_coex_protocol;
  uint8 num_conflicts;
  wci2_uart_baud_type_e baud;
  float link_path_loss_threshold;
  float filtered_rb_threshold;
  float rb_filter_alpha;
  uint16 cxm_tx_active_adv_notice;
  uint16 mdm_conn_ind_holdoff_time;
  uint32 cxm_lte_sleep_notifi_thres;
  coex_late_tx_adv_toggle_type tx_adv_late_toggle;
} coex_config_params_v8_no_tables;

/* packed version of coex_config_params_v8 victim_tbl */
typedef PACK(struct)
{
/* Break up victim_tbl into 4 => COEX_MAX_VICTIM_TBL_ENTRIES must
   be divisible by 4. This error will be thrown when the defined
   value is changed from 32 in the future to alert the user that a
   different subdivision size of the victim_tbl might be needed */
#if (COEX_MAX_VICTIM_TBL_ENTRIES != 32)
#error Fix victim table log packets
#endif
  coex_conflict_type_pack victim_tbl[COEX_MAX_VICTIM_TBL_ENTRIES/4];
} coex_config_params_v8_victim_tbl;

/* packed version of coex_port_config_v01_s */
typedef PACK(struct)
{
  cxm_port_id_e   id;              
  cxm_port_type_e type;              
  uint32          addr;              
  int16           assert_offset_us;  
  int16           deassert_offset_us;
} cxm_port_config_v01_s_pack;

/* packed version of cxm_port_table_v01_s */
typedef PACK(struct)
{
  uint8                      num_ports;                  
  cxm_port_config_v01_s_pack port_info[CXM_PORT_MAX]; 
} cxm_port_table_v01_s_pack;

/* packed version of coex_lte_policy_params_s */
typedef PACK(struct)
{
  coex_apt_table_enum_v01 apt_table;
  float                   controller_tx_power_limit;
  float                   wci2_power_limit;
  uint16_t                wci2_tx_pwrlmt_timeout;
  uint16_t                controller_tx_pwrlmt_timeout;
  int16_t                 tx_power_threshold_for_adv_tx_notice;
  uint8_t                 rb_threshold_for_adv_tx_notice;
} coex_lte_policy_params_s_pack;

/* packed version of coex_tech_spec_policy_params_u */
typedef PACK(union)
{
  coex_lte_policy_params_s_pack lte;
} coex_tech_spec_policy_params_u_pack;

/* modified and packed version of coex_tech_policy_params */
typedef PACK(struct)
{
  coex_policy_state_e                               policy_state;
  coex_policy_config_mask_v01                       policy;
  int8_t                                            power_threshold;
  uint32_t                                          rb_threshold;
  uint32_t                                          tx_continuous_subframe_denials_threshold;
  coex_frame_denial_duty_cycle_params_type_v01_pack tx_subrame_denial_params;  
  coex_tech_spec_policy_params_u_pack               params;
} coex_tech_policy_info_pack;

/* packed version of coex_global_policy_params_s */
typedef PACK(struct)
{
  float    link_path_loss_threshold;
  float    rb_filter_alpha;
  float    filtered_rb_threshold;
  uint32   t7_con_holdoff;
  float t7_pwr_alpha;
  uint16_t t7_pwr_thresh;
  uint32   t7_pwr_holdoff;
} coex_global_policy_params_s_pack;

/* packed version of cxm_smem_sleep_wakeup_s */
typedef PACK(struct)
{
  boolean    is_waking_up; 
  uint32     duration; 
} cxm_smem_sleep_wakeup_s_pack;

/* packed version of cxm_smem_lte_frame_timing_s */
typedef PACK(struct)
{
  uint32            dl_frame_time;  
  uint32            ta;
} cxm_smem_lte_frame_timing_s_pack;

/* packed version of cxm_smem_lte_tx_adv_s */
typedef PACK(struct)
{
  uint8                       sfn;             
  cxm_smem_tx_adv_tx_e        transmit;        
  cxm_smem_lte_chan_e         channel_type;
  uint8                       rbs;    
  int8                        slot0_power;     
  uint8                       slot0_first_rb;  
  uint8                       slot0_last_rb;   
  int8                        slot1_power;     
  uint8                       slot1_first_rb;  
  uint8                       slot1_last_rb;   
} cxm_smem_lte_tx_adv_s_pack;

/* packed version of cxm_smem_high_prio_s */
typedef PACK(struct)
{
  uint32            start;   
  uint32            end;
  uint8             num_freqs;                  
  uint32            freq[CXM_SMEM_MAX_HIGH_PRIO_FREQ];                 
} cxm_smem_high_prio_s_pack;

/* packed version of cxm_smem_tds_frame_timing_s */
typedef PACK(struct)
{
  uint32            sfn;          
  uint32            ustmr_val;
  uint32            ta;              
} cxm_smem_tds_frame_timing_s_pack;

/* packed version of cxm_smem_tds_slot_info_s */
typedef PACK(struct)
{         
  cxm_smem_slot_type_e  slot_type;     
  int16                 tx_power;       
  cxm_smem_slot_prio_e  priority;       
  uint32                monitor_freq;    
} cxm_smem_tds_slot_info_s_pack;

/* packed version of cxm_smem_tds_act_adv_s */
typedef PACK(struct)
{
  uint16                         sfn;           
  cxm_smem_tds_slot_info_s_pack  slot[CXM_SMEM_NUM_TDS_SLOTS];  
} cxm_smem_tds_act_adv_s_pack;

/* packed version of cxm_smem_timing_slot_entry */
typedef PACK(struct) 
{
  uint32 frequency;
  uint32 bandwidth;    
  cxm_smem_slot_type_e  slot_type;
  uint32 start_time;
  uint32 end_time; 
  cxm_smem_slot_prio_e  prio;
  int32 pwr; 
} cxm_smem_timing_slot_entry_pack; 

/* Packed version of cxm_smem_gsm_params_s timeline.
   Each packet contains 4 entries */
typedef PACK(struct)
{
  uint8 msg_tot;
  uint8 segment;
  cxm_smem_timing_slot_entry_pack slot_list[CXM_LOG_GSM_TL_SEG_SIZE];
} cxm_smem_gsm_timeline;

/* Ensure that it is packed to prevent a crash in the future */
typedef PACK(union)
{
  PACK(struct)
  {
    uint32 ustmr_start : 27;  
    uint32 resv1       : 2;  
    uint32 rx	         : 1;   
    uint32 tx          : 1;      
    uint32 check1      : 1;      
    uint32 ustmr_stop  : 27;    
    uint32 resv2       : 4;
    uint32 check2      : 1;      
  } bitmap;
  PACK(struct)
  {
    uint32 word0;                     
    uint32 word1;                     
  } words;
  uint64 data;
} cxm_smem_rx_tx_activity_info_u_pack;

/* packed structure for smem fw data */
typedef PACK(struct)
{
  cxm_smem_rx_tx_activity_info_u_pack cxm_smem_fw;
} cxm_smem_fw_data;

/* packed structure for wcn tx state */
typedef PACK(struct)
{
  cxm_wcn_tx_client_e       clnt;
  cxm_wcn_tx_clnt_status_e  status;
  cxm_wcn_tx_state_e        last_rep_tx;
  cxm_wcn_tx_rsp_codes_e    last_rsp_code;
} cxm_wcn_tx_info_s_pack;

/* packed structure for wci2 type7 */
typedef PACK(struct)
{
  uint8 wwan_tx_active;
  uint8 wwan_tx_pwr_active;
  uint8 tx_ant_sel;
} cxm_mdm_state_s_pack;

#endif /* CXM_LOG_TYPES_H */



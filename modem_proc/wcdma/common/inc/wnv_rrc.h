/*============================================================================
                            W N V _ R R C . H
DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2014-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/common/inc/wnv_rrc.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
10/28/16    sr      FR 38167: Changes to restrict the code to specific PLMN
01/12/16   sn      Changes for FR30881
01/27/16    nr      CR967546: Changes to enable FR27891 by default
10/16/15    as      Added NV for LTA during activation time
09/10/15    ac      Port MSIM support from TH 2.0
02/20/15    bc      Changes to enable FR22503
02/03/15    sr      Changes to enable FR23776
12/29/14    bc      Made changes to control the range of frequencies to be skipped around a WCDMA cell by an nv.
01/20/15    sn      Changes to enable FR20470 
11/20/14    sp      Made changes to enable CPC by default with DSDS/DSDA concurrencies.
11/19/14    sp      Enable DCHSPDA by default with DSDA concurrency
11/05/14    as      Made changes to enabled HS-RACH and MAC-I features by default for single SIM
10/16/14    bc      Changes to enable FR21174 and FR21035
09/17/14    sg      Made changes to move all RRC nv macros to wnv_rrc.h
09/03/14    rkmk    Setting WNV_ID_RRC_GANSS_SUPPORT_NV_DEFAULT to TRUE
08/08/14    ac      Update NV defaults for rrc_dsds_concurrency, rrc_dsda_concurrency, rrc_tsts_concurrency
07/21/14    ac      Update RRC_VERSION_I_FOR_RRC_DEFAULT to 5
07/21/14    ac      Rename RRC_DSDS_CONCURRENCY_CELLFACH to DEFAULT_CONFIG_IN_CELL_FACH
07/15/14    ac      Initial checkin 
===========================================================================*/

#ifndef _WNV_RRC_H_
#define _WNV_RRC_H_

#include "comdef.h"
#include "wnv.h"
#include "msgcfg.h"


/*************************************************************************** 
                   Exported structure definitions
****************************************************************************/

#define  WNV_ID_RRC_WTOL_CM_SUPPORT_DEFAULT                   TRUE                        
#define  WNV_ID_RRC_PPAC_SUPPORT_DEFAULT                      TRUE        
#define  WNV_ID_RRC_SAVE_ACQDB_DEFAULT                        FALSE
#define  WNV_ID_RRC_DISABLE_CPC_FDPCH_DEFAULT                 0         
#define  WNV_ID_RRC_DISABLE_CRASH_DEBUG_INFO_DEFAULT          0         
#define  WNV_ID_RRC_WTOL_PS_HO_SUPPORT_DEFAULT                TRUE         
#define  WNV_ID_RRC_WTOL_TDD_PS_HO_SUPPORT_DEFAULT            TRUE           
#define  WNV_ID_RRC_IGNORE_CELL_BAR_RESERVE_STATUS_DEFAULT    0           
#define  WNV_ID_RRC_CSFB_SKIP_SIBL1_OPT_DEFAULT               0            
#define  WNV_ID_RRC_FREQ_LOCK_ITEM_DEFAULT                    0        
#define  WNV_ID_RRC_DO_NOT_FORWARD_PAGE_DEFAULT               0          
#define  WNV_ID_RRC_FREQ_SCAN_TIMER_IN_MS_DEFAULT             1800000           
#define  WNV_ID_RRC_DEEP_SLEEP_NO_SVC_TIMER47_IN_MS_DEFAULT   10000        
#define  WNV_ID_RRC_BPLMN_FREQ_SCAN_TIMER_I48N_MS_DEFAULT     900000         
#define  WNV_ID_RRC_SIB_SLEEP_SB_DEFAULT                      TRUE        
#define  WNV_ID_RRC_CONSTANT_320MS_DRX_DEFAULT                FALSE         
#define  WNV_ID_RRC_PRIORITIZE_850_BAND_DEFAULT               FALSE
#define  WNV_ID_RRC_DEVICE_TYPE_DEFAULT                       0
#define  WNV_ID_RRC_DISABLE_MEAS_DEFAULT                      0
#define  WNC_ID_RRC_DORMANCY_ENABLE_DEFAULT                   RRC_NV_DORMANCY_ENABLED
#define  WNV_ID_RRC_DEFAULT_CONFIG_IN_CELL_FACH_DEFAULT       0 /*Support HS-FACH and CPC by default*/

#define WNV_ID_WCDMA_RRC_ENABLE_PSC_LOCK_DEFAULT              FALSE

#define WNV_ID_WCDMA_RRC_CSG_PRUNE_COUNTER_DEFAULT            8640
#define WNV_ID_WCDMA_CSG_SRCH_CARRIER_SPACE_DEFAULT           FALSE
#define WNV_ID_WCDMA_ASF_TIMER_DEFAULT                        0

#define WNV_ID_WCDMA_RRC_SNOW3G_ENABLED_DEFAULT               FALSE

#define WNV_ID_WCDMA_RRC_SET_SRB2_ACT_TIME_DEFAULT            0

#define WNV_ID_WCDMA_RRC_FAST_RETURN_TO_LTE_AFTER_CSFB_DEFAULT          FALSE
#define WNV_ID_WCDMA_RRC_FAST_RETURN_TO_LTE_AFTER_CSFB_TIMER_DEFAULT    2000


#define WNV_ID_RRC_GANSS_SUPPORT_NV_DEFAULT                   TRUE

#define WNV_ID_RRC_FE_FACH_SUPPORT_DEFAULT                    0

#define WNV_ID_RRC_DSDS_CONCURRENCY_EXTENDED_DEFAULT          (NV_DS_CPC_CONCURRENCY | NV_DS_DCHSDPA_CONCURRENCY | NV_UL_COMPRESSION_CONCURRENCY)
#define WNV_ID_RRC_DSDA_CONCURRENCY_EXTENDED_DEFAULT          (NV_DS_CPC_CONCURRENCY | NV_DS_DCHSDPA_CONCURRENCY | NV_UL_COMPRESSION_CONCURRENCY)
#define WNV_ID_RRC_TSTS_CONCURRENCY_EXTENDED_DEFAULT          (NV_DS_DCHSDPA_CONCURRENCY | NV_UL_COMPRESSION_CONCURRENCY)

#define WNV_ID_RRC_NV_ENABLE_MFBI_SUPPORT_DEFAULT             0

#define WNV_ID_RRC_UL_COMPR_CAP_SUPPORT_DEFAULT             1

#define WNV_ID_WCDMA_HSUPA_CM_CTRL_I_DEFAULT                  TRUE
#define WNV_ID_WCDMA_HSUPA_DEFAULT_CATEGORY_I_DEFAULT         6
#define WNV_ID_WCDMA_OPTIONAL_FEATURE_LIST_I_DEFAULT          ~0

#define WNV_ID_INTERRAT_NACC_SUPPORT_I_DEFAULT                TRUE
#define WNV_ID_GERAN_FEATURE_PACK_1_I_DEFAULT                 FALSE

#define WNV_ID_RRC_INTEGRITY_ENABLED_I_DEFAULT                1
#define WNV_ID_RRC_CIPHERING_ENABLED_I_DEFAULT                1
#define WNV_ID_HSDPA_COMPRESSED_MODE_ENABLED_I_DEFAULT        TRUE
#define WNV_ID_RRC_FAKE_SECURITY_ENABLED_I_DEFAULT            0
#define WNV_ID_WCDMA_RRC_PDCP_DISABLED_I_DEFAULT              TRUE
#define WNV_ID_RRC_VERSION_I_FOR_RRC_DEFAULT                  5 /* value 5 indicates Rel99/Rel5/Rel6/REL7/Rel8/Rel9 */

#define WNV_ID_WCDMA_CS_VOICE_OVER_HSPA_ENABLED_I_DEFAULT FALSE
#define WNV_ID_WCDMA_DL_FREQ_ENABLED_I_DEFAULT            FALSE
#define WNV_ID_WCDMA_DL_FREQ_I_DEFAULT                    0

#define WNV_ID_RRC_FEATURE_DEFAULT                        (RRC_EFACH_ENABLED|RRC_EFACH_DRX_ENABLED|RRC_MAC_I_ENABLED|RRC_HSRACH_ENABLED)
#define WNV_ID_RRC_SIB7_EXP_TIME_DEFAULT                  0
#define WNV_ID_RRC_ENABLE_MEAS_ID_EXTN_DEFAULT            FALSE

#define WNV_ID_RRC_IDLE_FEATURES_DEFAULT                  0x1F

#define WNV_ID_RRC_DB_DC_BAND_COMB_CONTROL_NV_DEFAULT                 (1 << (RRC_DB_DC_BAND1_BAND8-1))

#define WNV_ID_RRC_PLMN_ID_DEFAULT                        0xF

#define WNV_ID_RRC_HALF_DRX_LEN_DEFAULT                   FALSE
#define WNV_ID_RRC_FREQ_SKIP_RANGE_DEFAULT                22
#define WNV_ID_RRC_M2M_FEATURE_ENABLE_NV_DEFAULT          TRUE
#define WNV_ID_RRC_M2M_BITMASK_NV_DEFAULT                 0

#define RRC_NV_DORMANCY_ENABLED 1
#define RRC_NV_LEGACY_DORMANCY_DISABLED 2
#define RRC_NV_DORMANCY_DISABLED 3

/* Dual-SIM concurrency NV values */
#define NV_DS_HSFACH_CONCURRENCY        0x1
#define NV_DS_EDRX_CONCURRENCY          0x2
#define NV_DS_MAC_I_CONCURRENCY         0x4 
#define NV_DS_HSRACH_CONCURRENCY        0x8
#define NV_DS_CPC_CONCURRENCY           0x10
#define NV_DS_DCHSDPA_CONCURRENCY       0x20
#define NV_DS_DCHSUPA_CONCURRENCY       0x40
#define NV_DS_3CHSDPA_CONCURRENCY       0x80 
#define NV_DS_DBDC_HSDPA_CONCURRENCY    0x100 
#define NV_FE_FACH_CONCURRENCY          0x200
#define NV_UL_COMPRESSION_CONCURRENCY   0x400

/* RRC feature NV values */
#define RRC_EFACH_ENABLED                  0x0001
#define RRC_EFACH_DRX_ENABLED              0x0004
#define RRC_HSRACH_ENABLED                 0x0008
#define RRC_MAC_I_ENABLED                  0x0010
#define RRC_SIB5_DELAY_CELL_UPDATE_ENABLED 0x0020
#define RRC_DB_DC_HSDPA_ENABLED            0x0040
#define RRC_EDPCCH_PWR_BOOST_ENABLED       0x0080
#define RRC_SB_3C_BANDII_SUPPORTED         0x0100 /* Single band 3 carrier on band II is supported */
/* RRC CPC disable NV values */
#define NV_FDPCH_DISABLE          0x1
#define NV_EFDPCH_DISABLE         0x2
#define NV_CPC_DTX_DISABLE        0x4

/* CRASH DEBUG disable NV */
#define NV_CRASH_DEBUG_INFO_DISABLE 0x1

#define  RRC_DB_DC_BAND1_BAND8   1
#define  RRC_DB_DC_BAND2_BAND4   2
#define  RRC_DB_DC_BAND1_BAND5   3
#define  RRC_DB_DC_BAND1_BAND9   4
#define  RRC_DB_DC_BAND2_BAND5   5

#define WRRC_ENABLE_LTA_DURING_ACT_TIME 1
#define WRRC_MIN_ACT_TIME_TO_ALLOW_LTA  100

/* Default value for pseudo SIB19 */
  #define WRRCNV_PSEUDO_SIB19_UTRA_PRI 3
  #define WRRCNV_PSEUDO_SIB19_S_PRI_SRCH1 13
  #define WRRCNV_PSEUDO_SIB19_THR_SERV_LOW 5
  #define WRRCNV_PSEUDO_SIB19_EUTRA_PRI 7
  #define WRRCNV_PSEUDO_SIB19_EUTRA_MEAS_BW 2 /* rrc_EUTRA_MeasurementBandwidth_mbw25 */
  #define WRRCNV_PSEUDO_SIB19_THR_X_HIGH 5
  #define WRRCNV_PSEUDO_SIB19_THR_X_LOW 5
  #define WRRCNV_PSEUDO_SIB19_EUTRA_QRXLEVMIN (-59)

/* Band priority configuration structure. */
typedef PACKED struct
{
  uint32 band_priority_list[10];	 /* List of Bands*/
} rrc_nv_band_priority_config_type;

typedef PACKED struct PACKED_POST
{
  uint8 enable_lta_during_act_time;	 /* Enable-1 Disable-0 */
  uint16 minimum_act_time_required_to_allow_lta; 
} wcdma_rrc_support_lta_type;

typedef PACKED struct PACKED_POST{ 
  
  /* UTRA FDD serving */
  uint8         utra_priority;
  uint8         s_priority_search1;
  uint8         thresh_serving_low;
  
  /* EUTRA Neighbors */
  uint8         eutra_priority;
  uint8         eutra_meas_bandwidth;
  uint8         eutra_thresh_high;
  uint8         eutra_thresh_low;
  int8          eutra_qrxlevmin;
} rrc_nv_pseudo_sib19_type;

/* Placeholder for RRC NV types to make memory allocations easier */
typedef union
{
  boolean rrc_wtol_cm_support_nv;
  boolean rrc_ppac_support_nv;                                       
  boolean rrc_save_acqdb_nv;                                         
  rrc_nv_band_priority_config_type  rrc_nv_band_priority_config;    
  wcdma_rrc_support_lta_type wcdma_rrc_support_lta;
  uint8 rrc_disable_cpc_fdpch_nv;                                    
  uint8 rrc_disable_crash_debug_info_nv;                             
  boolean wcdma_rrc_wtol_ps_ho_support_nv;                           
  boolean wcdma_rrc_wtol_tdd_ps_ho_support_nv;                       
  uint8 rrc_ignore_cell_bar_nv;                                      
  uint8 rrc_csfb_skip_sib11_opt_nv;                                  
  uint16 rrc_nv_channel_lock_item_value;                             
  uint8 rrc_do_not_forward_page_nv;                                  
  uint32 wcdma_rrc_freq_scan_timer_nv;                               
  uint32 wcdma_rrc_deep_sleep_no_svc_timer_nv;                       
  uint32 wcdma_rrc_bplmn_freq_scan_timer_nv;                         
  boolean wcdma_rrc_sib_sleep_sb_nv;                                 
  boolean rrc_constant_320ms_drx_nv;                                 
  boolean rrc_prioritize_850_band_nv;
  uint16 rrc_disable_meas_nv;
  uint8 rrc_dormancy_support_nv;
        
  /* NV ID for PSC lock: 70241  */
  boolean rrc_enable_psc_lock_nv;

  uint32 wcdma_rrc_csg_max_prune_count_nv; /* Number of ASF searches in one month */
  boolean wcdma_csg_srch_carrier_space_nv;
  uint8 wcdma_asf_timer_nv;


  boolean snow3g_security_algo_supported;

  uint8 rrc_set_srb2_act_time_nv;

  boolean rrc_fast_return_to_lte_after_csfb_nv;
  uint16 rrc_fast_return_to_lte_after_csfb_timer_nv ;

  boolean rrc_ganss_support_nv;

  uint8 rrc_fe_fach_support_nv;

  /*NV72581*/
  uint16 rrc_dsds_concurrency;

  /*NV72582*/
  uint16 rrc_dsda_concurrency;

  /*NV72583*/
    uint16 rrc_tsts_concurrency;

  /* NV# 72548*/
  boolean rrc_mfbi_support_nv;

  /* NV# 72576 */
  boolean rrc_ul_compr_cap_nv;

  uint8 rrc_device_type_nv;

  uint16 rrc_feature_nv_item_value;            

  uint8 rrc_default_cfg_in_cell_fach_nv;

  uint32 rrc_sib7_exp_time;
  
  boolean rrc_meas_id_extn_support;

  uint32 wcdma_rrc_idle_feature;

  uint16 wcdma_rrc_db_dc_band_comb_nv_control;

  boolean rrc_half_drx_len;

  uint8 wcdma_freq_skip_range_nv;

  boolean rrc_m2m_feature_enabled;

  uint8  rrc_m2m_bitmask_nv;

  rrc_nv_pseudo_sib19_type wcdma_rrc_psuedo_sib19;


  rrc_plmn_identity_type efs_plmn_id;
} wnv_rrc_default;


typedef union
{
  uint32  optional_feature_list;
  uint8   hsupa_category;
  boolean hsupa_cm_ctrl;
  boolean dl_freq_enabled;
  uint16  dl_freq;
  boolean cs_voice_over_hspa_enabled;
  uint8   rrc_version;
  boolean rrc_pdcp_disabled;
  boolean fake_security_enabled;
  uint8   hsdpa_compressed_mode_enabled;
  boolean rrc_ciphering_enabled;
  boolean rrc_integrity_enabled;
  boolean geran_feature_pack_1;
  uint8   interrat_nacc_support;
  boolean rrc_fake_security;
} wnv_rrc_legacy_default;


/*************************************************************************** 
                      Internal structure definitions
****************************************************************************/

/*************************************************************************** 
                          Exported functions NV default functions
****************************************************************************/
wnv_api_status wnv_default_rrc(wsub_id_e_type wsub_id, wnv_nv_id_enum nv_id, void* out_buffer, uint32 output_len, boolean write_to_file);

wnv_api_status wnv_default_rrc_feature(wsub_id_e_type wsub_id, wnv_nv_id_enum nv_id, void* out_buffer, uint32 output_len, boolean write_to_file);

wnv_api_status wnv_default_rrc_device(wsub_id_e_type wsub_id, wnv_nv_id_enum nv_id, void* out_buffer, uint32 output_len, boolean write_to_file);

wnv_api_status wnv_default_rrc_legacy(wsub_id_e_type wsub_id, wnv_nv_id_enum nv_id, void* out_buffer, uint32 output_len, boolean write_to_file);

#endif /* _WNV_RRC_H_*/

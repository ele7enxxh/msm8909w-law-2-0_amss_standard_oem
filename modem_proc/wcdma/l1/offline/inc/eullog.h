#ifndef EULLOG_H
#define EULLOG_H

/*============================================================================
              EULLOG.H

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
the logging related code for HSUPA uplink module  .

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006, 2008 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/eullog.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
06/08/15    tsk     Enhance diag submission logic by using boolean instead of array index overflow for 0x435D log.
03/13/15    ssv     Correctlng 0x4346/0x4323/0x4322 flush logic when log buffer in not full
12/03/14    ssv     Changing Apex Version for Jolokia to get mac- inactivty table generated in 0x422c log packet.
11/19/14    vr      Deprecating and removing 0x4312 log packet code.
11/12/14    tsk     Remove incorrect DCUPA featurization in ul_tx_rf_send_log_pkt_to_diag.
10/29/14    tsk     Fix for CPC inactivity timer increments.
07/28/14    pkg     Code Check-in for 16QAM feature.
07/17/14    ssv     Enhancing 0x422c log packet
06/20/14    ymu     Enhancement of the log packet 0x4307 to 0x435D
05/20/14    ymu     Fix KW errors for array idx out of boundary
05/15/14    tsk     Fix KW reported error for array index 0 used as pointer.
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
01/09/14    ymu     Logging change: add clk speed logging.
12/17/13    yw      Enabling Rel7 E-DPCCH boost feature and Rel7 MPR
12/04/13    ymu     Logging issue: Remove log packet submit from ISR to local cmd when deactivate DC-HSUPA
11/07/13    ymu     Fix 0x4344 logging size issue in SHO case 
08/20/13    ymu     KW errors check in
07/22/13    ymu     Remove DC_HSUPA flushing logging packet from EUL ISR to local cmd 
07/12/13    yw      CPC DTX support for DC-HSUPA
06/12/13    ymu     Fix KW errors, deadlock crash and 435B crash. 
06/04/13    ymu     Fixed Compilation errors with DCHSUPA framework feature disabled. 
05/22/13    oh      Adding DC-HSUPA framework support
05/23/13    geg     WSW decoupling (except for component of QCHAT)
02/01/12    gnk     Nikel Feature cleanup.
01/31/12    raj     Nickel feature cleanup. Mainlined the feature FEATURE_WCDMA_REL6_ENH_MPR
04/28/11    sj      upmerge from offline/main
04/26/11    gnk     Removed FEATURE_WCDMA_HSUPA_IRQ_DELAY_FLOW_CONTROL feature
09/02/10    gv      Code Checkin for CPC DTX
11/18/09    gv      Use db10 instead of db12 units for computation
09/22/09    sup     Removed log code assignments and placed them in 
                    log_codes_wcdma_int_v.h
01/30/09    ka      Added Support for 4313 log packet
10/03/08    hk      Bringing in 7k Mailine fixes
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
05/09/07    am      Add retx bit for 4307 logpkt
05/04/07    am      Added boolean flag to flush logpkt func
03/16/07    am      4312 logpkt changes
03/09/07    am      Fix for 4309 logpkt malformed when EUL is stopped
02/19/07    am      4309 srv_cell_idx and CFN fixes
02/05/07    am      4311 logpkt changes
12/12/06    am      Change edch_cell_psc_info[] in L1/MAC log to uint16
12/06/06    am      Fix EUL L1/MAC log packet
11/02/06    am      Added code for 4309 log pkt HICH changes
09/19/06    am      Added logging changes for 4304/4305/4307/4309
09/15/06    am      Added code for logging and review comments
08/14/06    yiz     Mainline HSUPA code
05/23/06    am      file created.

===========================================================================*/

/* <EJECT> */

/*===========================================================================*/

/*===========================================================================

                     DEFINE/UNDEFINE LOCAL FEATURES

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msm.h"
#include "log.h"
#include "log_codes.h"
#include "eulcfg.h"
#include "eulsg.h"
#include "l1macdata.h"
#include "log_codes_wcdma_int.h"

/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/

//#define FEATURE_WCDMA_EUL_UL_EDPCH_DBG_INFO_LOG_PKT

/* This macro is used for filling the bit fields in EUL uplink log packet*/
#define EUL_UL_LOG_PKT_SET_FIELD(buf, field1, field2, value) \
  (buf) = (((buf) & (~EUL_UL_LOG_PKT_##field1##_##field2##_BMSK)) | \
           (((value) << EUL_UL_LOG_PKT_##field1##_##field2##_SHFT) & \
           EUL_UL_LOG_PKT_##field1##_##field2##_BMSK))

/* Macro used for getting the fields from EUL log packet */
#define EUL_UL_LOG_PKT_GET_FIELD(value, field1, field2) \
    (((value) & (EUL_UL_LOG_PKT_##field1##_##field2##_BMSK)) >> \
           EUL_UL_LOG_PKT_##field1##_##field2##_SHFT)



#ifdef FEATURE_WCDMA_CPC_DTX 
#define e_ul_flush_logpkts(reset_l1_mac_logpkts)\
      e_ul_edpch_info_pkt_handler();\
      e_ul_send_etfc_restrict_info_to_diag();\
      e_ul_send_rm_info_to_diag();\
      e_ul_cleanup_log_pkts_data(reset_l1_mac_logpkts);\
        eul_dtx_send_fw_log_pkt_to_diag();\
        ul_tx_rf_send_log_pkt_to_diag()
#else
#define e_ul_flush_logpkts(reset_l1_mac_logpkts)\
      e_ul_edpch_info_pkt_handler();\
      e_ul_send_etfc_restrict_info_to_diag();\
      e_ul_send_rm_info_to_diag();\
      e_ul_cleanup_log_pkts_data(reset_l1_mac_logpkts)
#endif /*FEATURE_WCDMA_CPC_DTX*/



/*edl channel is not configured for cell*/
#define E_UL_LOG_PKT_DL_CHAN_NOT_CFG                    3

/*HICH chan not cfgured */
#define E_UL_LOG_PKT_HICH_NOT_CFG                       3
/*nak from srv hich and ack from non srv hich*/
#define E_UL_LOG_PKT_HICH_ACK_NS                        3

/* max number of samples for 10 ms TTI is 20*/
#define E_UL_LOG_PKT_SAMPLE_MAX_10MS_TTI                20

/* max number samples for 10 ms frame info is 20*/
#define E_UL_LOG_PKT_SAMPLE_MAX_10MS_TTI_PER_FRAME_INFO 20

/* max number of samples for 2 ms TTI is 40*/
#define E_UL_LOG_PKT_SAMPLE_MAX_2MS_TTI                 40

/* max number samples for 10 ms frame info is 20*/
#define E_UL_LOG_PKT_SAMPLE_MAX_2MS_TTI_PER_FRAME_INFO  8

/*max number of l1 mac log packet samples bufs*/
#define E_UL_L1_MAC_SAMPLE_BUF_MAX                      2

/* Double buffer size for log packet */
#define EUL_FLOW_CTRL_LOG_MAX_SIZE                      2

/*The edch and HICH buf difference */
#define E_UL_L1_MAC_EDCH_HICH_2MS_SAMPLE_BUF_DIFF       (int32)(-8)
#define E_UL_L1_MAC_EDCH_HICH_10MS_SAMPLE_BUF_DIFF      (int32)(-4)

/*log set time stamp sets this value for 1 sample*/
#define  E_UL_LOG_TIME_IN_10MS                          0x80000

/*Needs to wait 2 frames before filling the dl rl info*/
#define E_UL_L1_MAC_LOG_PKT_DL_CHAN_UP_FN               3

#define EUL_GET_ETTI()  e_ul_curr_phychan_cfg_db_ptr->tti
#define EUL_GET_ETFCI_TAB_IDX()  e_ul_curr_phychan_cfg_db_ptr->e_tfci_table_idx
#define E_UL_LOG_PKT_SAMPLE_MAX  E_UL_LOG_PKT_SAMPLE_MAX_2MS_TTI
#define E_UL_LOG_PKT_NUM_FRAMES_IN_10MS  5
#define E_UL_LOG_PKT_SAMPLE_MAX_PER_FRAME_INFO E_UL_LOG_PKT_SAMPLE_MAX_10MS_TTI_PER_FRAME_INFO

/*===========================================================================
 EUL UL Channel Configuration log packet
 Will generate this log packet when trying to setup,teardown and reconfig of E-DPCH.
===========================================================================*/

/*==========================BIT FIELD MASKS====================================*/
#define EUL_UL_LOG_PKT_CHAN_CFG_ACTION_TYPE_BMSK           0x03
#define EUL_UL_LOG_PKT_CHAN_CFG_ACTION_TYPE_SHFT           0

#define EUL_UL_LOG_PKT_CHAN_CFG_SLOT_FORMAT_BMSK           0x0C
#define EUL_UL_LOG_PKT_CHAN_CFG_SLOT_FORMAT_SHFT           2

#define EUL_UL_LOG_PKT_CHAN_CFG_HS_DPCCH_PRESENT_BMSK      0x10
#define EUL_UL_LOG_PKT_CHAN_CFG_HS_DPCCH_PRESENT_SHFT      4

#define EUL_UL_LOG_PKT_CHAN_CFG_DPDCH_PRESENT_BMSK         0x20
#define EUL_UL_LOG_PKT_CHAN_CFG_DPDCH_PRESENT_SHFT         5

#define EUL_UL_LOG_PKT_CHAN_CFG_IR_CC_BMSK                 0x40
#define EUL_UL_LOG_PKT_CHAN_CFG_IR_CC_SHFT                 6

#define EUL_UL_LOG_PKT_CHAN_CFG_TTI_BMSK                   0x80
#define EUL_UL_LOG_PKT_CHAN_CFG_TTI_SHFT                   7

#ifdef FEATURE_WCDMA_16_QAM
#define EUL_UL_LOG_PKT_CHAN_CFG_UE_MAX_SF_BMSK             0x0F
#define EUL_UL_LOG_PKT_CHAN_CFG_UE_MAX_SF_SHFT             0

#define EUL_UL_LOG_PKT_CHAN_CFG_MAX_EUL_SF_BMSK            0xF0
#define EUL_UL_LOG_PKT_CHAN_CFG_MAX_EUL_SF_SHFT            4

#define EUL_UL_LOG_PKT_CHAN_CFG_INTERP_BMSK                0x0001
#define EUL_UL_LOG_PKT_CHAN_CFG_INTERP_SHFT                0

#define EUL_UL_LOG_PKT_CHAN_CFG_D_T2TP_BMSK                0x000E
#define EUL_UL_LOG_PKT_CHAN_CFG_D_T2TP_SHFT                1

#define EUL_UL_LOG_PKT_CHAN_CFG_BOOST_VAL_BMSK             0x07F0
#define EUL_UL_LOG_PKT_CHAN_CFG_BOOST_VAL_SHFT             4

#define EUL_UL_LOG_PKT_CHAN_CFG_ETFCI_TAB_BMSK             0x1800
#define EUL_UL_LOG_PKT_CHAN_CFG_ETFCI_TAB_SHFT             11

#define EUL_UL_LOG_PKT_CHAN_CFG_AG_TAB_BMSK                0x2000
#define EUL_UL_LOG_PKT_CHAN_CFG_AG_TAB_SHFT                13

#define EUL_UL_LOG_PKT_CHAN_CFG_SG_TAB_BMSK                0x4000
#define EUL_UL_LOG_PKT_CHAN_CFG_SG_TAB_SHFT                14
#else
#define EUL_UL_LOG_PKT_CHAN_CFG_UE_MAX_SF_BMSK             0x07
#define EUL_UL_LOG_PKT_CHAN_CFG_UE_MAX_SF_SHFT             0

#define EUL_UL_LOG_PKT_CHAN_CFG_MAX_EUL_SF_BMSK            0x38
#define EUL_UL_LOG_PKT_CHAN_CFG_MAX_EUL_SF_SHFT            3
#endif /* FEATURE_WCDMA_16_QAM */

#define EUL_UL_LOG_PKT_CHAN_CFG_END_FN_LOW_BMSK            0xFF
#define EUL_UL_LOG_PKT_CHAN_CFG_END_FN_LOW_SHFT            0

#define EUL_UL_LOG_PKT_CHAN_CFG_END_FN_HIGH_BMSK           0x0F
#define EUL_UL_LOG_PKT_CHAN_CFG_END_FN_HIGH_SHFT           0

//#define EUL_UL_LOG_PKT_CHAN_CFG_START_FN_LOW_BMSK 0xF0
//#define EUL_UL_LOG_PKT_CHAN_CFG_START_FN_LOW_SHFT 4

#define EUL_UL_LOG_PKT_CHAN_CFG_START_FN_BMSK              0xFFF0
#define EUL_UL_LOG_PKT_CHAN_CFG_START_FN_SHFT              4

#define EUL_UL_LOG_PKT_CHAN_CFG_EDPCH_PO_BMSK              0x0F
#define EUL_UL_LOG_PKT_CHAN_CFG_EDPCH_PO_SHFT              0

#define EUL_UL_LOG_PKT_CHAN_CFG_PL_NON_MAX_BMSK            0xFF00
#define EUL_UL_LOG_PKT_CHAN_CFG_PL_NON_MAX_SHFT            8

#define EUL_UL_LOG_PKT_CHAN_CFG_IQ_MAP_BMSK                0x01
#define EUL_UL_LOG_PKT_CHAN_CFG_IQ_MAP_SHFT                0

#define EUL_UL_LOG_PKT_CHAN_CFG_SRV_GRANT_BMSK             0x7E
#define EUL_UL_LOG_PKT_CHAN_CFG_SRV_GRANT_SHFT             1

#define EUL_UL_LOG_PKT_CHAN_CFG_GRANT_SEL_BMSK             0x80
#define EUL_UL_LOG_PKT_CHAN_CFG_GRANT_SEL_SHFT             7

#define EUL_UL_LOG_PKT_CHAN_CFG_TWO_IDX_STEP_T_BMSK        0xFF
#define EUL_UL_LOG_PKT_CHAN_CFG_TWO_IDX_STEP_T_SHFT        0

#define EUL_UL_LOG_PKT_CHAN_CFG_THREE_IDX_STEP_T_BMSK      0xFF
#define EUL_UL_LOG_PKT_CHAN_CFG_THREE_IDX_STEP_T_SHFT      0

#define EUL_UL_LOG_PKT_CHAN_CFG_MAX_TX_PWR_BMSK            0xFF
#define EUL_UL_LOG_PKT_CHAN_CFG_MAX_TX_PWR_SHFT            0

#define EUL_UL_LOG_PKT_CHAN_CFG_REF_ETFCI_BMSK             0x7F
#define EUL_UL_LOG_PKT_CHAN_CFG_REF_ETFCI_SHFT             0

#define EUL_UL_LOG_PKT_CHAN_CFG_REF_ETFCI_CFGED_BMSK       0x80
#define EUL_UL_LOG_PKT_CHAN_CFG_REF_ETFCI_CFGED_SHFT       7

#define EUL_UL_LOG_PKT_CHAN_CFG_REF_ETFCI_PO_BMSK          0x1F00
#define EUL_UL_LOG_PKT_CHAN_CFG_REF_ETFCI_PO_SHFT          8

#ifdef FEATURE_WCDMA_DC_HSUPA 
#define EUL_UL_LOG_PKT_CHAN_CFG_EDPDCH_INTERP_BMSK         0x00000001
#define EUL_UL_LOG_PKT_CHAN_CFG_EDPDCH_INTERP_SHFT         0

#define EUL_UL_LOG_PKT_CHAN_CFG_DELTA_T2TP_BMSK            0x0000000E
#define EUL_UL_LOG_PKT_CHAN_CFG_DELTA_T2TP_SHFT            1

#define EUL_UL_LOG_PKT_CHAN_CFG_ETFCI_BOOST_BMSK           0x000007F0
#define EUL_UL_LOG_PKT_CHAN_CFG_ETFCI_BOOST_SHFT           4

#define EUL_UL_LOG_PKT_CHAN_CFG_SCRAM_TYPE_S_BMSK          0x00000800
#define EUL_UL_LOG_PKT_CHAN_CFG_SCRAM_TYPE_S_SHFT          11

#define EUL_UL_LOG_PKT_CHAN_CFG_SCRAM_CODE_S_BMSK          0xFFFFF000
#define EUL_UL_LOG_PKT_CHAN_CFG_SCRAM_CODE_S_SHFT          12

#define EUL_UL_LOG_PKT_CHAN_CFG_PCP_LEN_S_BMSK             0x0007
#define EUL_UL_LOG_PKT_CHAN_CFG_PCP_LEN_S_SHFT             0

#define EUL_UL_LOG_PKT_CHAN_CFG_DPCCH_OFFSET_S_BMSK        0x07F8
#define EUL_UL_LOG_PKT_CHAN_CFG_DPCCH_OFFSET_S_SHFT        3

#define EUL_UL_LOG_PKT_CHAN_CFG_MIN_BETA_ED_P_BMSK         0x0F
#define EUL_UL_LOG_PKT_CHAN_CFG_MIN_BETA_ED_P_SHFT         0

#define EUL_UL_LOG_PKT_CHAN_CFG_MIN_BETA_ED_S_BMSK         0xF0
#define EUL_UL_LOG_PKT_CHAN_CFG_MIN_BETA_ED_S_SHFT         4
#endif /*FEATURE_WCDMA_DC_HSUPA */

/*==========================INFO STRUCTURE====================================*/
/* EDPCH config parameters received from RRC*/
typedef PACKED struct PACKED_POST {
 /*FIrst Byte:
  ** TTI             : 0: 2ms TTI,1: 10ms TTI
  ** IR              : 0: Increment Redundancy,1: Chase combining
  ** DPDCH_PRES      : Boolean. Indicates whether R99 DPDCH is present or not
  ** HS_DPCCH_PRES   :Boolean. Indicates whether HSDPA UL is present or not
  ** SLOT_FORMAT     :Range 0..3. DPCCH slot format
  ** ACTION_TYPE     :Disable: 0,Enable: 1,Reconfigurations: 2
*/
 uint8 tti_ir_dpdch_hsdpcch_slot_action;

 /*Second Byte:
  ** MAX_EUL_SF: Signaled max EUL spreading factor. Highest supported 
  ** spreading factor (0..7): 
  ** 0 = sf4x2-and-sf2x2,  1 = sf2x2,  2 = sf4x2,  3 = sf4,  
  ** 4 = sf8, 5 = sf16,  6 = sf32,  7 = sf64,
  **                    
  ** UE_MAX_SF : UE derived max EUL spreading factor Highest supported 
  **             spreading factor (0..7): encoded as above NOTE: Depends
  **             on the presence of DPDCH signaled at configuration time.
*/
 uint8 eul_ue_max_sf;
 
 /*Third Byte: low byte of end frame
  ** END_FRAME_NUM : Ending EUL Frame number or Subframe number for E-DPCH
  ** Range: 0..255. Frame number if TTI = 10ms
  ** Range: 0..1279. Subframe number if TTI = 2ms
  ** For disabling and reconfiguration                            
*/
 uint8 low_end_fn;

 /*4th byte :high nibble of end frame and low nibble of start frame
  ** 5th byte : high byte of start frame
  ** START_FRAME_NUM:Starting EUL Frame number or Subframe number for 
  ** E-DPCH. Range: 0..255. Frame number if TTI = 10ms
  ** Range: 0..1279. Subframe number if TTI = 2ms
  ** For enabling and reconfiguration
 */                                 
 uint16 start_fn_high_end_fn;

 /*next word:
  ** EDPCCH_PO: Range 0..8. Power offset for E-DPCCH relative to DPCCH
  ** PL_NON_MAX: Range: 44..100.Indicates the puncturing limit for non-maximum
  ** number of codes. PLnon-max = PL_NON_MAX / 100 corresponding 
  ** to range from 0.44 to 1.0.
*/
 uint16 pl_non_max_edpcch_po;

 /*IQ_MAP: E-DPDCH I/Q mapping 0: E-DPDCH1 mapped onto I 
  ** 1: E-DPDCH1 mapped onto Q 
  ** SERVING_GRANT: Initial serving grant value
  ** GRANT_SEL: 0 = primary, 1 = secondary
 */
 uint8 grant_sel_srv_grant_iq_map;

 /* 2IDX_STEP_T: Range: 0..37. ETFCI threshold switching from 1-step to 2-step 
  ** ETFCI increments due to RGCH command 
*/
 uint8 two_idx_step_t;

 /*3IDX_STEP_T:  Range: 0..37. ETFCI threshold switching from 2-step to 3-step 
  ** ETFCI increments due to RGCH command
*/
 uint8 three_idx_step_t;

 /*MAX_TX_PWR: Range: -50..33. Integer value.Indicates the maximum allowed/
  ** supported UE tx power in dBm
*/                                                  
 uint8 max_tx_pwr;
 
} eul_log_ul_chan_cfg_info_struct_type;

#ifdef FEATURE_WCDMA_DC_HSUPA 
typedef PACKED struct PACKED_POST
{
  /*
  ** bits 0th     : EDPDCH Power Interpolation
  ** bits 1  -  3 : Delta t2tp
  ** bits 4  - 10 : ETFCI Boost Value
  ** bits 11th    : Scrambling Code Type of Secondary
  ** bits 12 - 31 : Scrambling Code of Secondary
  */
  uint32 scramCode_type_etfcBoost_t2tp_interp;
  /*
  ** bits 0  -  2 : Power Control Preamble length of Secondary
  ** bits 3  - 10 : DPCCH power offset of Secondary
  */
  uint16 dpcchPowOff_pcpLen;
  /*
  ** bits 0  -  3 : Min Beta Ed Primary
  ** bits 4  -  7 : Min Beta Ed Secondary
  */
  uint8  minBetaEdPrim_sec;
} dchsupa_sec_eul_dpch_cfg_info_struct_type;
#endif /*FEATURE_WCDMA_DC_HSUPA */

/*==========================LOG PKT DEFINITION====================================*/

/*log packet definition for 4303-EUL UL Channel Configuration log packet 
   This log packet is generated when trying to setup, teardown and reconfigure 
   UL E-DPCH
*/
LOG_RECORD_DEFINE(EUL_UL_CHAN_CFG_INFO_LOG_PKT)
  #if defined(FEATURE_WCDMA_DC_HSUPA) || defined(FEATURE_WCDMA_16_QAM)
  uint8                                     version;
  uint8                                     carrier_state;
  #endif /* FEATURE_WCDMA_DC_HSUPA || FEATURE_WCDMA_16_QAM */

  /*The config information received for E-DPCH*/
  eul_log_ul_chan_cfg_info_struct_type  eul_ul_chan_cfg_info;

  /*For each reference power offset (static table of 8)
  ** TFCI_CONFIG: Is this Ref E-TFCI Configured  
  **              0: Not configured
  **              1: Configured
  ** REF_TFCI      :Range: 1..127. Reference E-TFCI
  ** REF_PWR_OFFSET:Range: 0..29. Reference Power Offset
  */
  uint16  po_etfci_cfged_ref_etfci_val[L1_MAX_REF_E_TFCI_LIST];
  
#ifdef FEATURE_WCDMA_DC_HSUPA 
  dchsupa_sec_eul_dpch_cfg_info_struct_type sec_dpch_cfg_info;

  uint32 vpe_speed;
  uint32 offlineclk_speed;
  uint32 q6clk_speed;
#endif /*FEATURE_WCDMA_DC_HSUPA */

  #ifdef FEATURE_WCDMA_16_QAM
  /*
  ** EDPDCH_INTERP : 0th bit
  ** DELTA_T2TP    : 1 - 3 bits
  ** ETFCI_BOOST   : 4 - 10 bits
  ** ETFCI_TABLE   : 11 - 12 bits
  ** AG_TABLE      : 13th bit
  ** SG_TABLE      : 14th bit 
  */
  uint16 boost_ag_sg_tab;
  #endif /* FEATURE_WCDMA_16_QAM */
LOG_RECORD_END

#define EUL_UL_CHAN_CFG_INFO_LOG_PKT_SIZE() (sizeof(EUL_UL_CHAN_CFG_INFO_LOG_PKT_type))

/*==========================================================================
EUL R_combo Tables log packet (Internal)
Logged only when E-DCH is configured or reconfigured.If compressed mode is cfged,
then multiple log packets will be triggered for the different compressed mode slot
format.
==========================================================================*/

/*==========================BIT FIELD MASKS====================================*/

#define EUL_UL_LOG_PKT_R_COMBO_TABLE_NUM_DTX_SLOTS_BMSK 0x07
#define EUL_UL_LOG_PKT_R_COMBO_TABLE_NUM_DTX_SLOTS_SHFT 0

#define EUL_UL_LOG_PKT_R_COMBO_TABLE_NUM_ETFCI_BMSK     0xFF00
#define EUL_UL_LOG_PKT_R_COMBO_TABLE_NUM_ETFCI_SHFT     8

#define EUL_UL_LOG_PKT_R_COMBO_TABLE_SF_BMSK            0x0F
#define EUL_UL_LOG_PKT_R_COMBO_TABLE_SF_SHFT            0

#define EUL_UL_LOG_PKT_R_COMBO_TABLE_NE_DATA_J_BMSK     0xFFFF
#define EUL_UL_LOG_PKT_R_COMBO_TABLE_NE_DATA_J_SHFT     0

#define EUL_UL_LOG_PKT_R_COMBO_TABLE_R_COMBO_J_BMSK     0xFFFFFFFFFFFFFFFFULL
#define EUL_UL_LOG_PKT_R_COMBO_TABLE_R_COMBO_J_SHFT     0

/*==========================INFO STRUCTURE====================================*/
/*The strcture for R combo table info*/
typedef PACKED struct PACKED_POST {
  /*SF : Range 0..10. Bits 3:0. Spreading factor and number of codes
             0 = sf4x2-and-sf2x2, 1 = sf2x2, 2 = sf4x2, 3 = sf4, 4 = sf8,
             5 = sf16,  6 = sf32,  7 = sf64,  8 = sf128,  9 = sf256, 
             10 = unsupported  Bits 7:4 are unused.
  */
  uint8  sf;
 
  /*NE_DATA_J: Range: 0..57600. Total number of bits available for 
             E-DCH transmission per TTI with E-TFCI = j 
  */
  uint16 ne_data_j;

   /* R_COMBO : Internal variable R_comboj for each E-TFCI with index j (0..127).
              Value is normalized by 2^14. 
  */
  #ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
  uint64 r_combo_j;
  #else
  uint32 r_combo_j;
  #endif /* FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST */
}eul_log_r_combo_table_info_struct_type;

/*==========================LOG PKT DEFINITION====================================*/
/*log packet definition for 4304-2.2.2 EUL R_combo Tables log packet (Internal)
   This log packet is generated  only when E-DCH is configured or reconfigured. 
   If compressed mode is configured, then multiple log packets will be triggered
   for the different compressed mode slot format.
*/
LOG_RECORD_DEFINE(EUL_R_COMBO_TABLE_INFO_LOG_PKT)
  #ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
  uint8 version;
  uint8 carrier_state;
  #endif /* FEATURE_WCDMA_16_QAM */

  /*NUM_DTX_SLOTS: Range 0..7. Bits 2:0. Bits 7:3 are unused.
              Number of DTX slots can vary from 0..7 due to compressed mode
    NUM_ETFCI: Range 0..127. Number of etfci used in the rcombo tables
  */
  uint16  num_etfci_dtx_slots;

  /* The R combo table info*/
  eul_log_r_combo_table_info_struct_type  
            eul_r_combo_table_info[L1_UPA_UL_MAX_NUM_ETFC_PER_TAB];
 
LOG_RECORD_END

/*Rcombo log packet size based on num etfci */
#define EUL_R_COMBO_TABLE_INFO_LOG_PKT_SIZE(num_etfci)  ( \
    FPOS(EUL_R_COMBO_TABLE_INFO_LOG_PKT_type, eul_r_combo_table_info)+  \
     ((num_etfci)*sizeof(eul_log_r_combo_table_info_struct_type)))


/*==========================================================================
 EUL UL E-DPCH log packet 
 will have a sample for this packet every TTI. For 10ms TTI, it will contain 20
 samples.
===========================================================================*/

#ifndef EUL_UL_EDPCH_INFO_LOG_PKT
#ifndef FEATURE_WCDMA_EUL_UL_EDPCH_DBG_INFO_LOG_PKT
#define EUL_UL_EDPCH_INFO_LOG_PKT EUL_UL_EDPCH_INFO_0x4305_LOG_PKT 
#else
#define EUL_UL_EDPCH_INFO_LOG_PKT EUL_UL_EDPCH_INFO_0x4306_LOG_PKT 
#endif
#else
#ifndef FEATURE_WCDMA_EUL_UL_EDPCH_DBG_INFO_LOG_PKT
#error "Redefinition of the log command code: EUL_UL_EDPCH_INFO_LOG_PKT (0x4305)"
#else
#error "Redefinition of the log command code: EUL_UL_EDPCH_INFO_LOG_PKT (0x4306)"
#endif
#endif

/*==========================BIT FIELD MASKS====================================*/
#define EUL_UL_LOG_PKT_EDPCH_INFO_NUM_SAMPLES_BMSK        0x003F
#define EUL_UL_LOG_PKT_EDPCH_INFO_NUM_SAMPLES_SHFT        0

#ifdef FEATURE_WCDMA_16_QAM
#define EUL_UL_LOG_PKT_EDPCH_INFO_TAB_IDX_BMSK            0x00C0
#define EUL_UL_LOG_PKT_EDPCH_INFO_TAB_IDX_SHFT            6

#define EUL_UL_LOG_PKT_EDPCH_INFO_TTI_BMSK                0x0100
#define EUL_UL_LOG_PKT_EDPCH_INFO_TTI_SHFT                8

#define EUL_UL_LOG_PKT_EDPCH_INFO_NUM_FRAMES_BMSK         0x7E00
#define EUL_UL_LOG_PKT_EDPCH_INFO_NUM_FRAMES_SHFT         9
#else
#define EUL_UL_LOG_PKT_EDPCH_INFO_TAB_IDX_BMSK            0x40
#define EUL_UL_LOG_PKT_EDPCH_INFO_TAB_IDX_SHFT            6

#define EUL_UL_LOG_PKT_EDPCH_INFO_TTI_BMSK                0x80
#define EUL_UL_LOG_PKT_EDPCH_INFO_TTI_SHFT                7

#define EUL_UL_LOG_PKT_EDPCH_INFO_NUM_FRAMES_BMSK         0x1F00
#define EUL_UL_LOG_PKT_EDPCH_INFO_NUM_FRAMES_SHFT         8
#endif /* FEATURE_WCDMA_16_QAM */

#define EUL_UL_LOG_PKT_EDPCH_INFO_START_SAMPLE_FN_BMSK    0x0FFF
#define EUL_UL_LOG_PKT_EDPCH_INFO_START_SAMPLE_FN_SHFT    0

#define EUL_UL_LOG_PKT_EDPCH_INFO_MAX_RETX_BMSK           0xF000
#define EUL_UL_LOG_PKT_EDPCH_INFO_MAX_RETX_SHFT           12

#define EUL_UL_LOG_PKT_EDPCH_INFO_NP_N_BMSK               0x07
#define EUL_UL_LOG_PKT_EDPCH_INFO_NP_N_SHFT               0

#define EUL_UL_LOG_PKT_EDPCH_INFO_NP_C_BMSK               0x70
#define EUL_UL_LOG_PKT_EDPCH_INFO_NP_C_SHFT               4

#define EUL_UL_LOG_PKT_EDPCH_INFO_CM_FRAME_BMSK           0x80
#define EUL_UL_LOG_PKT_EDPCH_INFO_CM_FRAME_SHFT           7

#define EUL_UL_LOG_PKT_EDPCH_INFO_N_SLOTS_C_BMSK          0x0F
#define EUL_UL_LOG_PKT_EDPCH_INFO_N_SLOTS_C_SHFT          0

#define EUL_UL_LOG_PKT_EDPCH_INFO_N_MAX_BMSK              0x0F
#define EUL_UL_LOG_PKT_EDPCH_INFO_N_MAX_SHFT              0

#define EUL_UL_LOG_PKT_EDPCH_INFO_N_TX1_BMSK              0xF0
#define EUL_UL_LOG_PKT_EDPCH_INFO_N_TX1_SHFT              4

#define EUL_UL_LOG_PKT_EDPCH_INFO_RSN_BMSK                0x03
#define EUL_UL_LOG_PKT_EDPCH_INFO_RSN_SHFT                0

#define EUL_UL_LOG_PKT_EDPCH_INFO_HAPPY_BIT_BMSK          0x04
#define EUL_UL_LOG_PKT_EDPCH_INFO_HAPPY_BIT_SHFT          2

#define EUL_UL_LOG_PKT_EDPCH_INFO_RETX_NUM_BMSK           0x78
#define EUL_UL_LOG_PKT_EDPCH_INFO_RETX_NUM_SHFT           3

#define EUL_UL_LOG_PKT_EDPCH_INFO_NUM_DTX_SLOTS_BMSK      0x07
#define EUL_UL_LOG_PKT_EDPCH_INFO_NUM_DTX_SLOTS_SHFT      0

#define EUL_UL_LOG_PKT_EDPCH_INFO_ERAM_BANK_IDX_BMSK      0x70
#define EUL_UL_LOG_PKT_EDPCH_INFO_ERAM_BANK_IDX_SHFT      4

#define EUL_UL_LOG_PKT_EDPCH_INFO_DPDCH_PRESENT_BMSK      0x80
#define EUL_UL_LOG_PKT_EDPCH_INFO_DPDCH_PRESENT_SHFT      7

#define EUL_UL_LOG_PKT_EDPCH_INFO_ETFCI_VAL_BMSK          0x7F
#define EUL_UL_LOG_PKT_EDPCH_INFO_ETFCI_VAL_SHFT          0

#define EUL_UL_LOG_PKT_EDPCH_INFO_BETA_ED_ON_BMSK         0x01
#define EUL_UL_LOG_PKT_EDPCH_INFO_BETA_ED_ON_SHFT         0

#define EUL_UL_LOG_PKT_EDPCH_INFO_SF_BMSK                 0xF0
#define EUL_UL_LOG_PKT_EDPCH_INFO_SF_SHFT                 4

#ifdef FEATURE_WCDMA_EUL_UL_EDPCH_DBG_INFO_LOG_PKT
#define EUL_UL_LOG_PKT_EDPCH_DBG_INFO_OVSF_CODE_BMSK      0xFF
#define EUL_UL_LOG_PKT_EDPCH_DBG_INFO_OVSF_CODE_SHFT      0

#define EUL_UL_LOG_PKT_EDPCH_DBG_INFO_PRE_RM_BITS_BMSK    0xFFFF
#define EUL_UL_LOG_PKT_EDPCH_DBG_INFO_PRE_RM_BITS_SHFT    0

#define EUL_UL_LOG_PKT_EDPCH_DBG_INFO_NB_BLOCKS_BMSK      0x07
#define EUL_UL_LOG_PKT_EDPCH_DBG_INFO_NB_BLOCKS_SHFT      0

#define EUL_UL_LOG_PKT_EDPCH_DBG_INFO_NB_BITS_BMSK        0xFFF8
#define EUL_UL_LOG_PKT_EDPCH_DBG_INFO_NB_BITS_SHFT        3

#define EUL_UL_LOG_PKT_EDPCH_DBG_INFO_NB_FILLER_BITS_BMSK 0xFFFF
#define EUL_UL_LOG_PKT_EDPCH_DBG_INFO_NB_FILLER_BITS_SHFT 0
#endif

/*==========================HEADER STRUCTURE====================================*/

/*The  logpacket header*/
typedef PACKED struct PACKED_POST {
  /*NUM_SAMPLES: Number of samples 0..20 if TTI = 10ms, 0..40 if TTI =2ms
  ** TTI: 0: TTI = 10ms , 1: TTI = 2ms
  ** TABLE: Range 0..1. E-TFCI table index 0 or 1
  ** NUM_FRAMES: Number of 10ms frames data in log pkt.
  **             20 for 10 ms and 8 for 2 ms TTI
  */
  uint16 num_samples_frames_tti_tabidx_info;

  /* START_FRAME_NUM: EUL frame or subframe number for first sample of E-DPCH
  **                Frame number, Range 0..255 if TTI = 10ms
  **                Subframe number, Range 0..1279 if TTI = 2ms
  **  MAX_RE_TRANS: Range 0..15. Maximum number of retransmissions   
  */
  uint16 max_retx_first_sample_fn;
}eul_log_edpch_info_hdr_struct_type;

/*==========================INFO STRUCTURE====================================*/

/* This structure is logged for for every 10 ms frame. so 20 samples for 10 msTTI
   and 8 samples for 2 ms TTI
*/
typedef PACKED struct PACKED_POST {
  /*CM_FRAME: 0: Not compressed, 1: Compressed frame
  ** NP_C: Range 0..8. N_pilot,C, Number of pilot bits per slot on DPCCH in 
  **         current frame
  ** NP_N: Range 3..8. N_pilot,N, Number of pilot bits per slot on DPCCH in 
  **        non-compressed frame
  */                                  
   uint8  cm_frame_np_c_np_n_info;

  /*NSLOTS_C: Range 0..15. N_slots,C , Number of non DTX slots in the 
  **           compressed frame
  */      
  uint8  n_slots_c;

  /*N_TX1: Range 8..15. n_tx1/15 is the scaling factor for the number of 
  **      bits available to E-DCH for TTI =10ms.
  ** N_MAX: Range 8..15. Defined as in [7]
  */
  uint8  n_max_and_n_tx1;
}eul_log_10ms_frame_info_struct_type;

/*structure for EDPCH info*/
 typedef PACKED struct PACKED_POST {
   /*RSN: Range 0..3. Retransmission sequence number
  ** H_BIT: Range 0..1. Happy bit
  ** Retransmission number: Range 0..15. 0 means the 1st transmission
   */
   uint8 edpcch_info;

   /* DPCH_PRESENT: 0: no E-DPCCH/E-DPDCH present this TTI (DTX)
  **               1: E-DPCCH/E-DPDCH present this TTI
  **         NOTE: E-DPCCH and E-DPDCH are always sent together .
  **               For debugging purpose, Beta_Ed may be set to 0 
  **               while Beta_Ec is non-zero to create UL CRC error. 
  **            
  ** ERAM_IDX:   ERAM bank index for TTI = 10ms (Range; 0..3);
  **                                 TTI = 2ms (Range: 0..7).
  ** NUM_DTX: Range 0..7. Number of DTX slots can vary from 0..7 
  **          due to compressed mode
   */
   uint8 dpdch_eram_bank_num_dtx_slots;

   /*ETFCI:Range 0..127. E-TFCI chosen for this TTI*/
   uint8 tab_idx_tfci_val;

   /*BETA_ED_ON: 0: No power on E-DPDCH (Beta_Ed = 0)
  **            1: Power on E-DPDCH (Beta_Ed > 0)
  **      NOTE: Beta_Ec will not be affected, regardless whether 
  **            Beta_Ed is zero or not
  ** SF : Range 0..10. Spreading factor and number of codes
  **      0 = sf4x2-and-sf2x2, 1 = sf2x2, 2 = sf4x2, 3 = sf4, 4 = sf8,
  **      5 = sf16,  6 = sf32,  7 = sf64,  8 = sf128,  9 = sf256, 
  **      10 = no E-DPDCH PhCh                      
   */                           
   uint8 sf_beta_ed_on;

   /*debug version (Internal)- info for log packet 4306*/
   #ifdef FEATURE_WCDMA_EUL_UL_EDPCH_DBG_INFO_LOG_PKT
   /* OVSF_CODE: 4 bytes info: each byte has  OVSF code for E-DPDCH_i, (i = 0..3)
  **            Range 0..255.
   */
   uint8 edpdch_ovsf_code[4];

   /*  PRE_RM_BITS: Range 0..60120. Number of bits before rate matching*/
   uint16 pre_rm_bits;

   /* NB_ BLOCKS: Range 0..4. Number of turbo coded blocks. 
  ** NB_BITS:Range 0..5112. Number of bits per turbo coded block.
   */
   uint16 num_turbo_block_and_bits_per_block;

   /* NB_FILLER_BITS: Number of filler bits to match nearest transport block 
  **                  size based on E-TFCI table.
   */
   uint16 num_filler_bits;
   #endif
 } eul_log_edpch_info_struct_type;

/*==========================SAMPLE STRUCTURE====================================*/

/*The following structure is used for 10 ms frame info*/
typedef struct{
  uint8  cm_frame;
  uint8  np_c;
  uint8 np_n;
  uint8  n_slots_c;
  uint8  n_max;
  uint8  n_tx1;
}eul_edpch_frame_info_struct_type;

 /* The following structure is used to get a sample every frame.*/
typedef struct{
  uint8 retx_num;
  uint8 rsn;
  boolean happy_bit;
  boolean edpch_present;
  uint8 eram_bank;
  uint8 num_dtx_slots;
  uint8  tfci_val;
  uint8 sf;
  boolean  beta_ed_on;
 #ifdef FEATURE_WCDMA_EUL_UL_EDPCH_DBG_INFO_LOG_PKT
  uint8 edpdch_ovsf_code[4];
  uint16 pre_rm_bits;
  uint8 num_turbo_blocks;
  uint16 bits_per_block;
  uint16 num_filler_bits;
 #endif
}eul_edpch_info_sample_struct_type;

typedef struct{
  uint32 frame_num;
  uint32 sample_num;
  eul_edpch_info_sample_struct_type sample_info[E_UL_LOG_PKT_SAMPLE_MAX];
  eul_edpch_frame_info_struct_type frame_info[E_UL_LOG_PKT_SAMPLE_MAX_PER_FRAME_INFO];
}eul_edpch_log_pkt_struct_type;
/*==========================LOG PKT DEFINITION====================================*/

/*This packet is delivered  every 200ms for 10ms TTI and 80ms for 2ms TTI,
  or upon reconfiguration. We will have a sample for this packet every TTI.
  For 10ms TTI, it will contain 20 samples; and for 2ms TTI, it will contain
  40 samples.
*/
LOG_RECORD_DEFINE(EUL_UL_EDPCH_INFO_LOG_PKT)
  #if defined(FEATURE_WCDMA_DC_HSUPA) || defined(FEATURE_WCDMA_16_QAM)
  uint8                                      version;
  uint8                                      carrier_state;
  #endif /* FEATURE_WCDMA_DC_HSUPA || FEATURE_WCDMA_16_QAM */

  /* The header info edpch info*/
  eul_log_edpch_info_hdr_struct_type edpch_hdr_info[WL1_MAX_UL_CARR_IDX];

  /* The EDPCH info. If the samples are less than E_UL_LOG_PKT_SAMPLE_MAX 
  ** then only the info corresponding to the samples will be send to diag.
  ** This is Variable length field
  */
  eul_log_edpch_info_struct_type edpch_info[WL1_MAX_UL_CARR_IDX][E_UL_LOG_PKT_SAMPLE_MAX];

  /*The per frame info. This info will be filled after the num_samples of 
  ** EDPCH info. If there are less than E_UL_LOG_PKT_SAMPLE_MAX samples for 
  ** edpch info, the info of frame_info is moved and filled the holes left 
  ** by the edpch_info with the frame_info data of  
  ** E_UL_LOG_PKT_SAMPLE_MAX_PER_FRAME_INFO.This is Variable length field
   */
  eul_log_10ms_frame_info_struct_type frame_info[WL1_MAX_UL_CARR_IDX][E_UL_LOG_PKT_SAMPLE_MAX_PER_FRAME_INFO];
LOG_RECORD_END

/*The variable length of the combined l1 mac log packet */
#define EUL_UL_EDPCH_INFO_LOG_PKT_SIZE(num_samples, num_frames, carr_idx)  ( \
    FPOS(EUL_UL_EDPCH_INFO_LOG_PKT_type, edpch_hdr_info)+  \
         ((carr_idx + 1) * sizeof(eul_log_edpch_info_hdr_struct_type)) + \
         ((carr_idx + 1) * num_samples * sizeof(eul_log_edpch_info_struct_type)) + \
         ((carr_idx + 1) * num_frames * sizeof(eul_log_10ms_frame_info_struct_type)))


 /*=======================================================================
 EUL E-TFC Restriction log packet (Internal)
 will have a sample for this packet every TTI. For 10ms TTI, it will contain 20
 samples.
=======================================================================*/
/*==========================BIT FIELD MASKS====================================*/

#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_NUM_SAMPLES_BMSK     0x7F
#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_NUM_SAMPLES_SHFT     0

#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_TTI_BMSK             0x80
#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_TTI_SHFT             7

#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_NUM_FRAMES_BMSK      0x1F00
#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_NUM_FRAMES_SHFT      8

#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_START_SAMPLE_FN_BMSK 0xFFF
#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_START_SAMPLE_FN_SHFT 0

#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_NUM_DTX_SLOTS_BMSK   0x07
#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_NUM_DTX_SLOTS_SHFT   0

#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_BETA_C_BMSK          0x0F
#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_BETA_C_SHFT          0

#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_BETA_D_BMSK          0xF0
#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_BETA_D_SHFT          4

#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_HARQ_PO_BMSK         0x0F
#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_HARQ_PO_SHFT         0

#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_RETX_BMSK            0x10
#define EUL_UL_LOG_PKT_ETFC_RESTRICT_INFO_RETX_SHFT            4

/*==========================HEADER STRUCTURE====================================*/
/*The header info structure*/
typedef PACKED struct PACKED_POST {

  /* NUM_SAMPLES: Number of samples 0..20 if TTI = 10ms, 0..40 if TTI =2ms
     TTI: 0: TTI = 10ms , 1: TTI = 2ms
     NUM_FRAMES: Number of 10ms frames data in log pkt.
                 20 for 10 ms and 8 for 2 ms TTI
  */
  uint16 tti_num_samples_frames_info;

  /*START_FRAME_NUM: EUL frame or subframe number for first sample of E-DPCH
                     Frame number if TTI = 10ms and
                     Subframe number if TTI = 2ms
 */
  uint16 first_sample_fn;

  uint8 etfci_boost_val;
}eul_log_etfc_restrict_info_hdr_struct_type;

/*==========================INFO STRUCTURE====================================*/
/* The ETFCI restrict info*/
 typedef PACKED struct PACKED_POST {
  /*BETA_C: Range: 0..15. Bits 3:0.Indicates beta_c value
     BETA_D:  Range: 0..15. Bits 7:4Indicates beta_d value
      Note: Beta c/d ratio is calculated as the value of beta_d/beta_c 
  */
   uint8 beta_d_beta_c;
  
   /*BETA_ED: Range: 5..168.Indicates the E-DPDCH to DPCCH beta ratio 
                     Value corresponds to beta_ed x 15
                     NOTE: If number of codes = 4, then this is the beta_ed on 
                     the codes with the highest spreading factor)
   */                
   uint16 beta_ed;
   uint16 beta_ed_sf2;
   
   /*UNQUANT_BETA_HS:Range: 0-65535. Bits 16
                             Indicates un-quantized hs-DPCCH to PDCCH beta
                             ratio Value corresponds to beta_hs x 15 x 2^9
   */  
   uint16 unquant_beta_hs;
   
   /*UNQUANT_BETA_EC: Range: 0-65535. Bits 16
                             Indicates the un-quantized E-DPCCH to DPCCH
                             beta ratio Value corresponds to beta_ec x 15 x 2^9
   */                                    
   uint8 unquant_beta_ec;
   
   /* DPCCH_PWR: DPCCH Power (dBm) = int (DPCCH_PWR) / 12.
                            Current DPCCH power level in dBm
   */                       
   int16 dpcch_power;
   
   /* RCOMBO_SUP_T: Threshold for determining Max_sup_ETFCI */
   uint64 r_combo_max_sup;
   
   /* RCOMBO_SG_T:  Threshold for determining Max_SG_ETFCI */
   uint64 r_combo_sg;
   
   /* MAS_SUP_ETFCI: Range: 0..128 Highest supported ETFCI based on 
                             available power
   */
   uint8 max_sup_etfci;
   
   /* HARQ_PO: Range: 0..6. Bits: 3:0.
                      HARQ power offset used for this transmission
      RETX:boolean: Bit 4:
                    TRUE if it is retx else FALSE
   */
   uint8 harq_po_retx;

   /*SERVING_GRANT: The serving grant index based on AGCH and RGCH. 
               Range: 0..37. : 0..37 means E-DPDCH/DPCCH from
                    (5/15)^2 to 6*(168/15)^2 and 0xFF means no grant
                     available. 
   */
   uint8 srv_grant;

   /*MAX_SG_ETFCI: Range: 0..128 Highest supported ETFCI based 
                               on serving grant
   */
   uint8 max_sg_etfci;

   uint8 etfci_sel;
  } eul_log_etfc_restrict_info_struct_type;

/*==========================LOG PKT DEFINITION====================================*/

 /*EUL E-TFC Restriction log packet (Internal) */
LOG_RECORD_DEFINE(EUL_UL_ETFC_RESTRICT_INFO_LOG_PKT)
  uint8 version;
  uint8 carrier_state;

  /* The hdr info for etfc restrict info*/
  eul_log_etfc_restrict_info_hdr_struct_type etfc_restrict_hdr_info;

  /*The etfc restrict info.If the samples are less than E_UL_LOG_PKT_SAMPLE_MAX 
       then only the info corresponding to the samples will be send to diag.This is
       Variable length field
  */
  eul_log_etfc_restrict_info_struct_type  etfc_restrict_info[E_UL_LOG_PKT_SAMPLE_MAX];

  /* for every 10 ms frame,log NUM_DTX_SLOTS: Range: 0..7. Bits 2:0.
       Number of DTX slots can vary from 0..7 due to compressed mode.This is
       Variable length field
  */
  uint8  num_dtx_slots[E_UL_LOG_PKT_SAMPLE_MAX_PER_FRAME_INFO];
LOG_RECORD_END

/*The variable length of the etfc restrict packet */
#define EUL_UL_ETFC_RESTRICT_INFO_LOG_PKT_SIZE(num_samples,num_frames)  ( \
    FPOS(EUL_UL_ETFC_RESTRICT_INFO_LOG_PKT_type, etfc_restrict_info)+  \
     ( num_samples * sizeof(eul_log_etfc_restrict_info_struct_type)) + \
      ( num_frames * sizeof(uint8)))

/*==========================================================================
 EUL Rate Matching log packet (Internal) 
 will have a sample for this packet every TTI. For 10ms TTI, it will contain 20
 samples.
===========================================================================*/

/*==========================BIT FIELD MASKS====================================*/
#define EUL_UL_LOG_PKT_RATE_MATCHING_NUM_SAMPLES_BMSK      0x3F
#define EUL_UL_LOG_PKT_RATE_MATCHING_NUM_SAMPLES_SHFT      0

#define EUL_UL_LOG_PKT_RATE_MATCHING_TTI_BMSK              0x80
#define EUL_UL_LOG_PKT_RATE_MATCHING_TTI_SHFT              7

#define EUL_UL_LOG_PKT_RATE_MATCHING_START_SAMPLE_FN_BMSK  0xFFF
#define EUL_UL_LOG_PKT_RATE_MATCHING_START_SAMPLE_FN_SHFT  0

#define EUL_UL_LOG_PKT_RATE_MATCHING_RV_IDX_BMSK           0x3
#define EUL_UL_LOG_PKT_RATE_MATCHING_RV_IDX_SHFT           0

#define EUL_UL_LOG_PKT_RATE_MATCHING_NE_J_BMSK             0xFFFF
#define EUL_UL_LOG_PKT_RATE_MATCHING_NE_J_SHFT             0

#define EUL_UL_LOG_PKT_RATE_MATCHING_E_INI_SYS_BMSK        0xFFFF
#define EUL_UL_LOG_PKT_RATE_MATCHING_E_INI_SYS_SHFT        0

#define EUL_UL_LOG_PKT_RATE_MATCHING_E_PLUS_SYS_BMSK       0xFFFF
#define EUL_UL_LOG_PKT_RATE_MATCHING_E_PLUS_SYS_SHFT       0

#define EUL_UL_LOG_PKT_RATE_MATCHING_E_MINUS_SYS_BMSK      0xFFFF
#define EUL_UL_LOG_PKT_RATE_MATCHING_E_MINUS_SYS_SHFT      0

#define EUL_UL_LOG_PKT_RATE_MATCHING_E_INI_P1_BMSK         0xFFFF
#define EUL_UL_LOG_PKT_RATE_MATCHING_E_INI_P1_SHFT         0

#define EUL_UL_LOG_PKT_RATE_MATCHING_E_PLUS_P1_BMSK        0xFFFF
#define EUL_UL_LOG_PKT_RATE_MATCHING_E_PLUS_P1_SHFT        0

#define EUL_UL_LOG_PKT_RATE_MATCHING_E_MINUS_P1_BMSK       0xFFFF
#define EUL_UL_LOG_PKT_RATE_MATCHING_E_MINUS_P1_SHFT       0

#define EUL_UL_LOG_PKT_RATE_MATCHING_E_INI_P2_BMSK         0xFFFF
#define EUL_UL_LOG_PKT_RATE_MATCHING_E_INI_P2_SHFT         0

#define EUL_UL_LOG_PKT_RATE_MATCHING_E_PLUS_P2_BMSK        0xFFFF
#define EUL_UL_LOG_PKT_RATE_MATCHING_E_PLUS_P2_SHFT        0

#define EUL_UL_LOG_PKT_RATE_MATCHING_E_MINUS_P2_BMSK       0xFFFF
#define EUL_UL_LOG_PKT_RATE_MATCHING_E_MINUS_P2_SHFT       0

/*==========================HEADER STRUCTURE====================================*/
/*The header info*/
typedef PACKED struct PACKED_POST {
/* NUM_SAMPLES: Number of samples 0..20 if TTI = 10ms, 0..40 if TTI =2ms
  ** TTI: 0: TTI = 10ms , 1: TTI = 2ms
*/
uint8  num_sample_tti_info;

 /*START_FRAME_NUM: EUL frame or subframe number for first sample of E-DPCH
  **                 Frame number if TTI = 10ms and
  **                 Subframe number if TTI = 2ms
 */
uint16 start_sample_fn;
}eul_log_rate_matching_info_hdr_struct_type;

/*==========================INFO STRUCTURE====================================*/
/*The rate matching info */
 typedef PACKED struct PACKED_POST {
   /*E-DCH RV index: Range 0..3:Redundancy version index for HARQ rate-matching,
  **                can be derived from RSN
   */                            
   uint8 rv_idx;
   /*N e,j: The total number of bits in a TTI before rate matching*/
   #ifdef FEATURE_WCDMA_16_QAM
   uint32 ne_j;
   #else
   uint16 ne_j;
   #endif /* FEATURE_WCDMA_16_QAM */
   /*E_INI_SYS:E_INI for system bit */
   uint16 e_ini_sys;
   /*E_PLUS_SYS: E_PLUS for system bit*/
   uint16 e_plus_sys;
   /*E_MINUS_SYS:E_MINUS for system bit*/
   uint16 e_minus_sys;
   /*E_INI_P1:E_INI for parity 1 bit*/
   uint16 e_ini_p1;
   /*E_PLUS_P1:E_PLUS for parity 1 bit*/
   uint16 e_plus_p1;
   /*E_MINUS_P1:E_MINUS for parity 1 bit*/
   uint16 e_minus_p1;
   /*E_INI_P2: E_INI for parity 2 bit*/
   uint16 e_ini_p2;
   /*E_PLUS_P2:E_PLUS for parity 2 bit*/
   uint16 e_plus_p2;
   /*E_MINUS_P2:E_MINUS for parity 2 bit*/
   uint16 e_minus_p2;
  } eul_log_rate_matching_info_struct_type;

/*==========================LOG PKT DEFINITION==================================*/
/*This packet is delivered  every 200ms for 10ms TTI and 80ms for 2ms TTI,
  or upon reconfiguration. We will have a sample for this packet every TTI.
  For 10ms TTI, it will contain 20 samples; and for 2ms TTI, it will contain
  40 samples.
*/
LOG_RECORD_DEFINE(EUL_UL_RATE_MATCHING_INFO_LOG_PKT)
  #if defined(FEATURE_WCDMA_DC_HSUPA) || defined(FEATURE_WCDMA_16_QAM)
  uint8                                      version;
  uint8                                      carrier_state;
  #endif /* FEATURE_WCDMA_DC_HSUPA || FEATURE_WCDMA_16_QAM */

 /*The rate matching header info*/
  eul_log_rate_matching_info_hdr_struct_type rate_matching_hdr_info[WL1_MAX_UL_CARR_IDX];

 /*The rate matchiing info.If the samples are less than E_UL_LOG_PKT_SAMPLE_MAX 
  ** then only the info corresponding to the samples will be send to diag.This is
  ** Variable length field
 */
  eul_log_rate_matching_info_struct_type     rate_matching_info[WL1_MAX_UL_CARR_IDX][E_UL_LOG_PKT_SAMPLE_MAX];
LOG_RECORD_END

/*The variable length of the rate matching packet */
#define EUL_UL_RATE_MATCHING_INFO_LOG_PKT_SIZE(num_samples, carr_idx)  ( \
    ( (dword) (((EUL_UL_RATE_MATCHING_INFO_LOG_PKT_type *) 0)->rate_matching_hdr_info)) +  \
    ((carr_idx + 1) * sizeof(eul_log_rate_matching_info_hdr_struct_type)) + \
    ((carr_idx + 1) * num_samples * sizeof(eul_log_rate_matching_info_struct_type)) )

/*============================================================================
 EUL combined L1/MAC log packet
 will have a sample for this packet every TTI. For 10ms TTI, it will contain 20
 samples.
=============================================================================*/

/*==========================BIT FIELD MASKS====================================*/

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_NUM_SAMPLES_BMSK      0x3F
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_NUM_SAMPLES_SHFT      0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_TTI_BMSK              0x40
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_TTI_SHFT              6

#ifdef FEATURE_WCDMA_16_QAM
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_ETFCI_TAB_IDX_BMSK    0x30
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_ETFCI_TAB_IDX_SHFT    4

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_AG_TAB_IDX_BMSK       0x40
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_AG_TAB_IDX_SHFT       6

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_SG_TAB_IDX_BMSK       0x80
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_SG_TAB_IDX_SHFT       7

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_MOD_TYPE_BMSK         0x80
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_MOD_TYPE_SHFT         7
#else
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_ETFCI_TAB_IDX_BMSK    0x80
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_ETFCI_TAB_IDX_SHFT    7
#endif /* FEATURE_WCDMA_16_QAM */
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_START_SAMPLE_FN_BMSK  0xFFF
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_START_SAMPLE_FN_SHFT  0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_START_HARQ_ID_BMSK    0x7000
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_START_HARQ_ID_SHFT    12

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_NUM_CELLS_BMSK        0x03
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_NUM_CELLS_SHFT        0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_SRV_CELL_IDX_BMSK     0x0C
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_SRV_CELL_IDX_SHFT     2

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_COMBINED_HICH_BMSK    0x03
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_COMBINED_HICH_SHFT    0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_COMBINED_RGCH_BMSK    0x0C
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_COMBINED_RGCH_SHFT    2

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_DTX_FLAG_BMSK         0x10
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_DTX_FLAG_SHFT         4

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_HAPPY_BIT_BMSK        0x40
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_HAPPY_BIT_SHFT        6

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_CM_OVERLAP_BMSK       0x80
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_CM_OVERLAP_SHFT       7

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_HICH_CELL0_BMSK       0x03
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_HICH_CELL0_SHFT       0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_HICH_CELL1_BMSK       0x0C
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_HICH_CELL1_SHFT       2

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_HICH_CELL2_BMSK       0x30
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_HICH_CELL2_SHFT       4

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_HICH_CELL3_BMSK       0xC0
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_HICH_CELL3_SHFT       6

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RGCH_CELL0_BMSK       0x03
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RGCH_CELL0_SHFT       0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RGCH_CELL1_BMSK       0x0C
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RGCH_CELL1_SHFT       2

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RGCH_CELL2_BMSK       0x30
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RGCH_CELL2_SHFT       4

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RGCH_CELL3_BMSK       0xC0
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RGCH_CELL3_SHFT       6

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_AGCH_PRESENT_BMSK     0x01
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_AGCH_PRESENT_SHFT     0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_AG_VALUE_BMSK         0x3E
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_AG_VALUE_SHFT         1

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_AG_SCOPE_BMSK         0x40
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_AG_SCOPE_SHFT         6

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_P_OR_S_BMSK           0x80
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_P_OR_S_SHFT           7

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_MACD_PRESENT_BMSK     0xFF
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_MACD_PRESENT_SHFT     0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RETX_COUNT_BMSK       0x0F
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RETX_COUNT_SHFT       0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_PAYLOAD_RESTRICT_BMSK 0xF0
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_PAYLOAD_RESTRICT_SHFT 4

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_SG_IDX_BMSK           0x3F
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_SG_IDX_SHFT           0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_NON_SRV_RG_BMSK       0xC0
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_NON_SRV_RG_SHFT       6

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_SG_LUPR_IDX_BMSK      0x3F
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_SG_LUPR_IDX_SHFT      0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_SI_PRESENT_BMSK       0xC0
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_SI_PRESENT_SHFT       6

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_ETFCI_BMSK            0x7F
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_ETFCI_SHFT            0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RLS_ID_BMSK           0x07
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_RLS_ID_SHFT           0

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_PSC_BMSK              0xFF8
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_PSC_SHFT              3

#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_TPC_ID_BMSK           0x7000
#define EUL_UL_LOG_PKT_COMBINED_L1_MAC_TPC_ID_SHFT           12

/*==========================SAMPLE STRUCTURE====================================*/
 /* The following structure is used to get a sample every frame.*/
typedef struct{
  uint8 combined_hich;
  uint8 combined_rgch;
  boolean dtx_flag;
  uint8 si_present;
  boolean happy_bit;
  boolean cm_gap_info;
  uint8 hich_per_cell[4];
  uint8 rgch_per_cell[4];
  boolean agch_present;
  uint8 ag_value;
  uint8 ag_scope;
  boolean p_or_s;
  uint8 mac_d_present;
  uint8 retx_ctr;
  uint8 pay_load_restrict_info;
  uint8 sg_idx;
  uint8 comb_non_srv_rg;
  uint8 sg_lupr_idx;
  uint8 etfci_value;
  uint8 modType;
}eul_combined_l1_mac_info_sample_struct_type;

/*The header info*/
typedef struct {

  /*NUM_SAMP:Number of samples in this log packet.
             Range: 0..20 for 10 ms TTI and 0-40 for 2 ms TTI
    TTI: Transmission time interval : 0: 2ms TTI :1: 10ms TTI
    ETFCI_TABLE:E-TFC table index: 0 or 1
  */
  uint8   tti;
  uint8   tab_idx;

  /* START_FN: Starting EUL Frame number or Subframe number for E-DPCH.
                Range: 0..255. Frame number if TTI = 10ms
                Range: 0..1279. Subframe number if TTI = 2ms
     START_HARQ_ID:Indicates which HARQ process ID corresponds to the first
                frame or subframe number in this log packet. Range: 0..7
  */
  uint16 start_frame_num;
  uint8   harq_id;

  /*NUM_CELLS:Indicates the number of E-DCH cells present. 
                0 = 1 E-DCH cell present 
                1 = 2 E-DCH cells present 
                2 = 3 E-DCH cells present 
                3 = 4 E-DCH cells present 
    SRV_CELL: The E-DCH serving cell index.
  */
  uint8   srv_cell_idx;
  uint8   num_cells;
  uint16  edch_cell_psc_info[EUL_MAX_RL];
  uint8   edch_cell_rlsid_info[EUL_MAX_RL];
  uint8   edch_cell_tpcid_info[EUL_MAX_RL];
  qword time_stamp;
}eul_combined_l1_mac_info_hdr_struct_type;

typedef struct{
  uint32 cur_edch_buf_idx;
  uint32 cur_edch_sample_num;
  uint32 cur_hich_buf_idx;
  int32 cur_hich_sample_num[E_UL_L1_MAC_SAMPLE_BUF_MAX];
  eul_combined_l1_mac_info_hdr_struct_type  hdr_info[E_UL_L1_MAC_SAMPLE_BUF_MAX];

  eul_combined_l1_mac_info_sample_struct_type
                 sample_info[E_UL_L1_MAC_SAMPLE_BUF_MAX][E_UL_LOG_PKT_SAMPLE_MAX];

}eul_l1_mac_info_buf_struct_type;

typedef struct {
  boolean valid;
  uint32 sample_num;
  eul_combined_l1_mac_info_hdr_struct_type    hdr_info;
  eul_combined_l1_mac_info_sample_struct_type  sample_info[8];
}eul_l1_mac_info_flush_struct_type;

/*==========================HEADER STRUCTURE====================================*/

/*The header info*/
typedef PACKED struct PACKED_POST {

  /*NUM_SAMP:Number of samples in this log packet.
             Range: 0..20 for 10 ms TTI and 0-40 for 2 ms TTI
     TTI: Transmission time interval : 0: 2ms TTI :1: 10ms TTI
     ETFCI_TABLE:E-TFC table index: 0 or 1
  */
  uint8   num_samples_tti_tabidx_info;

  /* START_FN: Starting EUL Frame number or Subframe number for E-DPCH.
                Range: 0..255. Frame number if TTI = 10ms
                Range: 0..1279. Subframe number if TTI = 2ms
     START_HARQ_ID:Indicates which HARQ process ID corresponds to the first
                frame or subframe number in this log packet. Range: 0..7
  */
  uint16  start_frame_num_harq_id;

  /*NUM_CELLS:Indicates the number of E-DCH cells present. 
                0 = 1 E-DCH cell present 
                1 = 2 E-DCH cells present 
                2 = 3 E-DCH cells present 
                3 = 4 E-DCH cells present 
    SRV_CELL: The E-DCH serving cell index.
  */
  uint8   srv_cell_idx_num_cells;
  
  #ifdef FEATURE_WCDMA_16_QAM
  uint8 etfci_boost_val;
  #endif /* FEATURE_WCDMA_16_QAM */
}eul_log_combined_l1_mac_info_hdr_struct_type;

/*==========================INFO STRUCTURE====================================*/

/*The combined l1 mac info*/
typedef PACKED struct PACKED_POST {

 /*COMB_HICH:Indicates the combined ACK/NAK for the transport block 
             sent in this HARQ.       0: DTX,   1: ACK,  2: NAK
   COMB_RGCH:Indicates the combined RGCH determining the serving grant
             for the transport block sent in this subframe 
             0: HLD,  1: UP ,  2: DOWN
   DTX_FLAG: Indicates if the transmission for this TTI was DTX
             0: non-DTX, 1: DTX
   CM_GAP:Indicates if the transmission for this TTI overlapped with compressed
            mode slots
   SI_FLAG:Indicates if scheduling information was present  in this TTI
   HAPPY:Indicates the value of the happy bit
 */
 uint8 hich_rgch_dtx_happy_cmgap_info;

 /*HICH_CELL :1..4:Indicates the ACK/NAK reported per cell for the transport block 
             sent in this HARQ. Results are given for up to 4 E-DCH cells
             0: DTX, 1: ACK,  2: NAK
 */
 uint8 hich_per_cell_info;

 /* RGCH_CELL: 1..4: Indicates the RGCH per cell determining the serving grant
              for the transport block sent in this subframe. Results are given 
              for up to 4 E-DCH cells     0: HLD, 1: UP, 2: DOWN
 */                         
 uint8 rgch_per_cell_info;

 /*AGCH_FLAG:Whether the AGCH is present or not (if not present, the AG value 
             is undefined)
    AG_VALUE:Absolute grant value. Range: 0..31
    AG_SCOPE:Absolute grant scope   0: All , 1: per-HARQ, 
    P_OR_S: Primary or secondary E-RNTI received
*/
 uint8 agch_info;

 /* MAC_D_PRESENCE: Indicates for each Mac-d flow (max 8) whether it was 
                  present in this TTI or not. For example, bit 0 (LSB) indicates
                  whether MAC-d flow 0 is present in this transport block.
                 0: this Mac-d flow is absent, 1: this Mac-d flow was present
 */
 uint8 mac_d_presence;
 
 /*RETX_CTR:Indicates how many times this transport block has been retransmitted
            excluding this transmission. Range: 0..15
     PAYLOAD_REASON: 0: serving payload limited by max power
              1: serving payload limited by serving grant
              2: serving payload limited by buffer occupancy (lack of data)
*/
 uint8 retx_payload_restrict_info;

 /*SG_INDEX:The serving grant index based on AGCH and RGCH. Range: 0..37
              0..37 means E-DPDCH/DPCCH from (5/15)2 to 6*(168/15)2
              0x3F means no grant available.
  COMBINED NON SRV RGCH: Indicates the non srv rg determining the serving grant
              for the transport block sent in this subframe.
              0: HLD, 1: UP, 2: DOWN
 */
 uint8 sg_idx_non_srv_rg;

 /*LUPR:This is the SG index after mapping the reference_ETPR to the closest higher
        SG in the relative grant calculation  (25.321 sect. 9.2.5.2.1), also denoted 
        SGLUPR
 */             
 uint8 sg_lupr_idx_si_info;

 /*ETFCI [0 - 6] : E-TFCI. Range: 0..127*/
 /*MOD_TYPE [7th]: Modulation Type: 0-QPSK, 1-16QAM*/
 uint8 etfci_val;
} eul_log_sg_mac_info_struct_type;

/*==========================LOG PKT DEFINITION====================================*/
/*This packet is delivered  every 200ms for 10ms TTI and 80ms for 2ms TTI,
    or upon reconfiguration or upon active set update. We will have a sample for
    this packet every TTI. For 10ms TTI, it will contain 20 samples; and for 2ms 
    TTI, it will contain 40 samples.
*/
LOG_RECORD_DEFINE(EUL_COMBINED_L1_MAC_LOG_PKT)
  #if defined(FEATURE_WCDMA_DC_HSUPA) || defined(FEATURE_WCDMA_16_QAM)
  uint8                                     version;
  uint8                                     carrier_state;
  #endif /* FEATURE_WCDMA_DC_HSUPA || FEATURE_WCDMA_16_QAM */

  /* The header info*/
  eul_log_combined_l1_mac_info_hdr_struct_type l1_mac_hdr_info[WL1_MAX_UL_CARR_IDX];

  /*combined l1 mac info.If the samples are less than E_UL_LOG_PKT_SAMPLE_MAX 
  ** then only the info corresponding to the samples will be send to diag.This is
  ** Variable length field
  */
  eul_log_sg_mac_info_struct_type  eul_sg_mac_log_info[WL1_MAX_UL_CARR_IDX][E_UL_LOG_PKT_SAMPLE_MAX];

  /*For each cell in the E-DCH active set, indicated by NUM_CELLS:
  ** RLS_ID :0..7:(bit 2:0) :E-DCH RLS ID for this cell.
  ** PSC :0..511:(bit 11:3):PSC for this cell 
  ** This info will be filled after the num_samples of combined l1 mac info. 
  ** If there are less than E_UL_LOG_PKT_SAMPLE_MAX samples for the info, 
  ** the  info of edch_cell_psc_rlsid_info is moved and filled the holes left by
  ** the combined l1 mac info with the psc rls id info for num cells.This is
  ** Variable length field
  */
  uint16  edch_cell_psc_rlsid_info[WL1_MAX_UL_CARR_IDX][EUL_MAX_RL];
LOG_RECORD_END

/*The variable length of the combined l1 mac log packet */
#define EUL_COMBINED_L1_MAC_LOG_PKT_SIZE(num_samples, num_edch_cells_pri, num_edch_cells_sec, carr_idx)  ( \
     FPOS(EUL_COMBINED_L1_MAC_LOG_PKT_type, l1_mac_hdr_info)+  \
       ((carr_idx + 1) * sizeof(eul_log_combined_l1_mac_info_hdr_struct_type))+ \
       ((carr_idx + 1) * num_samples * sizeof(eul_log_sg_mac_info_struct_type)) + \
       ((num_edch_cells_pri + num_edch_cells_sec) * sizeof(uint16)))

/*macro for deciding to fill the dat to be flushed into flush buffer*/
#define EUL_L1_MAC_LOG_PKT_FLUSH(flush_log_pkt, carr_idx) \
    (flush_log_pkt == TRUE) &&(e_ul_l1_mac_log_pkt_flush_info[carr_idx].valid == FALSE)

/*get the hdr info for the 4 samples from different buffer that need to be flushed
 due to RL add/drop.decremnt the qword by index *(E_UL_LOG_TIME_IN_10MS)*/
#define EUL_L1_MAC_LOG_PKT_FILL_FLUSH_DATA_HDR_DB(cur_hdr_idx, num_samples, carr_idx) \
    e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info= \
                      e_ul_l1_mac_log_pkt_sample_info[carr_idx].hdr_info[cur_hdr_idx];\
    e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.start_frame_num -= num_samples;\
    qw_dec(e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.time_stamp,\
           (num_samples *(E_UL_LOG_TIME_IN_10MS)));\
    e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.harq_id =\
           (e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.harq_id-num_samples\
                                      + EUL_HARQ_RTT_10MS)%EUL_HARQ_RTT_10MS

/*get the hdr info for the 4 samples from same buffer that need to be flushed due 
to RL add/drop.increment the qword by index *(E_UL_LOG_TIME_IN_10MS)*/
#define EUL_L1_MAC_LOG_PKT_FILL_FLUSH_DATA_HDR_SB(cur_hdr_idx, num_samples, carr_idx) \
    e_ul_l1_mac_log_pkt_flush_info[carr_idx].valid = TRUE;\
    e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info=\
                      e_ul_l1_mac_log_pkt_sample_info[carr_idx].hdr_info[cur_hdr_idx];\
    qw_inc(e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.time_stamp,\
           (num_samples  *(E_UL_LOG_TIME_IN_10MS)));\
    e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.start_frame_num += num_samples;\
    e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.harq_id =\
           (e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.harq_id  +\
                                            num_samples )%EUL_HARQ_RTT_10MS

/*get the downlink hdr info for the 4 samples from downlink variables */
#define EUL_L1_MAC_LOG_PKT_FILL_FLUSH_DL_DATA_HDR(carr_idx)\
    for(index=0;index<(EUL_GET_EDCH_NUM_CELLS());index++)\
    {\
      e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.edch_cell_tpcid_info[index]=\
                                           EUL_GET_ACTIVE_RL_TPC_IDX(index, carr_idx);\
      e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.edch_cell_psc_info[index]=\
                                           EUL_GET_ACTIVE_RL_PSC(index, carr_idx);\
      e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.edch_cell_rlsid_info[index]=\
                                           EUL_GET_ACTIVE_RL_RG_COMB_IDX(index, carr_idx);\
    }\
    e_ul_l1_mac_log_pkt_flush_info[carr_idx].hdr_info.num_cells = (EUL_GET_EDCH_NUM_CELLS())-1
    
#define EUL_L1_MAC_LOG_PKT_SEND_FLUSH_DATA_TO_DIAG(num_samples, carr_idx)\
     (e_ul_l1_mac_log_pkt_flush_info[carr_idx].valid == TRUE ) &&( num_samples > 0)

/*================================================================
  EUL L1 MAC statistics log packet 
  This packet is logged periodically every 200ms, or when a reconfiguration occurs.
 ================================================================*/

/*==========================BIT FIELD MASKS====================================*/
#define EUL_UL_LOG_PKT_L1_MAC_STATS_START_SAMPLE_FN_BMSK 0x0FFF
#define EUL_UL_LOG_PKT_L1_MAC_STATS_START_SAMPLE_FN_SHFT 0

#define EUL_UL_LOG_PKT_L1_MAC_STATS_TTI_BMSK             0x1000
#define EUL_UL_LOG_PKT_L1_MAC_STATS_TTI_SHFT             12

#ifdef FEATURE_WCDMA_16_QAM
#define EUL_UL_LOG_PKT_L1_MAC_STATS_ETFCI_TAB_IDX_BMSK   0x6000
#define EUL_UL_LOG_PKT_L1_MAC_STATS_ETFCI_TAB_IDX_SHFT   13
#else
#define EUL_UL_LOG_PKT_L1_MAC_STATS_ETFCI_TAB_IDX_BMSK   0x2000
#define EUL_UL_LOG_PKT_L1_MAC_STATS_ETFCI_TAB_IDX_SHFT   13
#endif /* FEATURE_WCDMA_16_QAM */
/*==========================HEADER STRUCTURE====================================*/
/* The header info*/
typedef PACKED struct PACKED_POST {
 /*TTI: Transmission time interval : 0: 2ms TTI :1: 10ms TTI
  ** START_FN: Starting EUL Frame number or Subframe number for E-DPCH.
  **           Range: 0..255. Frame number if TTI = 10ms
  **           Range: 0..1279. Subframe number if TTI = 2ms
  ** ETFCI_TABLE:E-TFC table index: 0 or 1                    
 */
 uint16 tab_idx_etti_start_fn;

 /*NUM_SAMP:Number of frames since last time this log packet was triggered */    
 uint8 num_samples;
} eul_log_l1_mac_stats_info_hdr_struct_type;

/*==========================INFO STRUCTURE====================================*/
/* The l1 mac stats info*/
typedef PACKED struct PACKED_POST {
/*NUM_S_RG_UP:Number of serving RGCH Up commands*/
 uint8 num_srv_rg_up;

 /*NUM_S_RG_DN:Number of serving RGCH Down commands*/
 uint8 num_srv_rg_down;

 /*NUM_S_RG_HOLD:Number of serving RGCH Hold commands*/
 uint8 num_srv_rg_hold;

 /*NUM_NS_RG_DN:Number of non-serving RGCH Down commands*/
 uint8 num_non_srv_rg_down;

 /*NUM_NS_RG_HOLD:Number of non-serving RGCH Hold commands*/
 uint8 num_non_srv_rg_hold;

 /*NUM_AGCH:Number of  AGCH  successfully received*/
 uint8 num_ag_received;

 /*SUM_AGCH:SUM of AGs received is sent to diag and Mean value of AGCH 
  ** (i.e. range=0..31) will be displayed*/
 uint16 sum_ag;

 /*sum_SG:SUM of SGs available per TTI is sent to diag and Mean serving grant 
  ** value (i.e. range=0..37 ) is displayed 
  */
 uint16 sum_sg;

/*NUM_NEW_TX:Number of new transmissions*/
 uint8 num_new_tx;

/*NUM_RETX:Number of retransmissions*/
 uint8 num_re_tx;

/*NUM_DTX:Number of DTX*/ 
 uint8 num_dtx;

 /*NUM_MAX_RETX_NAK:Number of times NAK was received after max number of
  **                  retransmission was reached (residual error). This is used to 
  **                  determine residual BLER. 
 */
 uint8 num_max_re_tx_nak;

 /*NUM_RESETS:Number of MAC-e resets*/
 uint8 num_mac_e_reset;

 /*NUM_FIRST_TX:Number of times the first retransmission was successfully 
  ** ACKed. Enables to calculate BLER of first transmission statistics 
  */
 uint8 num_first_tx_acked;

 /*SUM_ETFCI:SUM of ETFCIs sent (for new tx) is sent to diag and Mean value of
  ** E-TFCI is displayed 
  */
 uint16 sum_etfci;

 /*SUM_RAW_BITS:SUM of RAW bits sent for newtx and retx divided by 64 is sent
  ** to diag and Average number of transmitted bits per TTI counting both  new and 
  ** retxs is  displayed.
 */
 uint16 sum_raw_bits;
 
 /*SUM_SCHED_BITS:SUM of schedule bits sent for newtx  divided by 64 is sent
  ** to diag and Average number of transmitted scheduled bits per TTIcounting only
  ** new transmissions is displayed 
  */
 uint16 sum_sched_bits;

 /*SUM_NS_BITS:SUM of non schedule bits sent for newtx divided by 64 is sent
  ** to diag and Average number of transmitted non-scheduled bits per TTI, 
  ** counting only new transmissions is displayed
  */ 
 uint16 sum_non_sched_bits;

 /*SUM_PWR_BITS:SUM of pwr limiting bits found for newtx divided by 64 is sent
  ** to diag and Average number of bits that could have been transmitted 
  ** per TTI based on available power is displayed
  */
 uint16 sum_pwr_bits;

 /*SUM_SG_BITS:SUM of SG limiting bits found for newtx divided by 64 is sent
  ** to diag and Average number of bits that could have been transmitted
  ** per TTI based on SG is displayed
 */
 uint16 sum_sg_bits;

 /*SUM_BUF_BITS:SUM of buffer bits found for newtx divided by 64 is sent
  ** to diag and Average number of bits that could have been transmitted 
  ** per TTI based on available buffer status (scheduled flows only) is disaplyed 
  **             NOTE While summing up over all TTIs: For all TTIs for which the 
  **             buffer status is greater than the max TB size, use the max TB size
  **             instead of the actual buffer status in the summation.
 */
 uint16 sum_buf_bits;
 
 /*SUM_DPCCH:SUM of dpcch pwr in dBM is sent diag and Average DPCCH 
  ** power in dBm (signed integer) is displayed
  */
 int16 sum_dpcch_pwr;

 /*SUM_T2P_D:SUM of (beta_d*15/beta_c)^2 for dpdch divided by 64 is sent to diag 
  ** and Average of  (beta_d/beta_c)^2 is displayed 
  */
 uint16 sum_beta_d_by_beta_c;

 /*SUM_T2P_HS:SUM of (beta_hs*15/beta_c)^2 for HS divided by 64 is sent to diag 
  ** and Average of (beta_hs/beta_c)^2 is displayed 
  */
 uint16 sum_beta_hs_by_beta_c;

 /*SUM_T2P_EC:SUM of (beta_ec*15/beta_c)^2 for edpcch divided by 64 is sent to 
  ** diag and Average of (beta_ec/beta_c)^2 is displayed 
  */
 uint16 sum_beta_ec_by_beta_c;

 /*SUM_T2P_ED:SUM of (beta_ed*15/beta_c)^2 for all edpdch divided by 64 is sent 
  ** to diag and Average of  (beta_ed/beta_c)^2for all edpdch is displayed 
  ** NOTE:  summation over all E-DPDCH beta ratios in case of multicode
 */
 uint16 sum_beta_ed_by_beta_c;

 /*MAX_POW:Max power allowed in dBm (signed integer)*/
 int8 max_pwr;
 
/*mac continuous info*/
 eul_log_l1_mac_stats_mac_info_struct_type mac_cont_info;

 /*NUM_HAPPY:Number of TTIs UE was happy*/
 uint8  num_happy;

 /*reserved field*/
 uint8 reserved;

 /*AGCH_QUAL:
  ** Bits [3:0]  The 4 MSB of the mean AGCH energy value, counting only TTIs where 
  **             AGCH CRC passes
  ** Bits [5:4]  The number of times Viterbi decoder lands on the incorrect final state, 
  **             counting only TTIs where AGCH CRC passes
  ** Bits [15:6] The sum of the number of errors corrected by the Viterbi decoder, 
  **             counting only TTIs where AGCH CRC passes
 */                               
 uint16 ag_qual_info;

 /*RGCH_QUAL:
  ** Bits [0]   Indicates whether serving RGCH RLS is present for the time interval
  **            represented in this log packet
  ** Bits [3:1] The number of non-serving RGCH RLS present for the time interval 
  **            represented in this log packet
  ** Bits [4:6] The average RGCH decision quality metric for the first RLS. This is 
  **            calculated as the average over all valid RGCH decision samples, i.e. 
  **            only when the RGCH energy exceeds the threshold. 
  **            The quality metric is encoded as follows:
  **            1.0   <= Energy/Threshold < 1.25 :Quality = 0
  **            1.25 <= Energy/Threshold < 1.5 :  Quality = 1
  **            1.5   <= Energy/Threshold < 1.75 :Quality = 2
  **            1.75 <= Energy/Threshold < 2.0 :  Quality = 3
  **            2.0   <= Energy/Threshold < 2.5 :  Quality = 4
  **            2.5   <= Energy/Threshold < 3.0 :  Quality = 5
  **            3.0   <= Energy/Threshold < 4.0 :  Quality = 6
  **            4.0   <= Energy/Threshold :           Quality = 7
  **            NOTE: If the serving RLS is present, it will be logged as the first RLS.
  ** Bits [7:9] RGCH decision quality logging for the second RLS
  ** Bits [10:12] RGCH decision quality logging for the third RLS
  ** Bits [13:15] RGCH decision quality logging for the fourth RLS
 */ 
 uint16 rg_qual_info;
  
 /*HICH_QUAL:
  ** Bits [3:1] The number of HICH RLS present for the time interval represented in 
  **            this log packet
  ** Bits [4:6] The average HICH decision quality metric for the first RLS. This is 
  **            calculated as the average over all valid HICH decision samples, i.e.
  **            only when the HICH energy exceeds the threshold. 
  **            The quality metric is encoded in the same way as for RGCH
  ** Bits [7:9] HICH decision quality logging for the second RLS
  ** Bits [10:12] HICH decision quality logging for the third RLS
  ** Bits [13:15] HICH decision quality logging for the fourth RLS
 */
 uint16 hich_qual_info;
} eul_log_l1_mac_stats_info_struct_type;

/*This strcture is used mainly to decrese the division operations in the 
** interrupt context 
*/
typedef struct{
/*sum of raw bits sent for newtx and retx*/
 uint32 sum_raw_bits;
/*sum of raw bits sent for newtx */
 uint32 sum_pwr_bits;
/*sum of raw bits sent for newtx */
 uint32 sum_sg_bits;
/*sum of 225x(beta_d/beta_c)^2*/
 uint32 sum_beta_d;
/*sum of (A_hsx15)^2*/
 uint32 sum_beta_hs;
/*sum of (A_ecx15)^2*/
 uint32 sum_beta_ec;
/*sum of (A_edx15)^2 for all EDPDCH*/
 uint32 sum_beta_ed;
/*The sum of 4 MSB of the AGCH energy value, counting only TTIs where 
  ** AGCH CRC passes 
  */
 uint16 sum_ag_4msb_eng;
/*The sum of the number of errors corrected by the Viterbi decoder, 
  ** counting only TTIs where AGCH CRC passes
  */
 uint16 sum_ag_errors;
/*The number of times Viterbi decoder lands on the incorrect final state, 
  ** counting only TTIs where AGCH CRC passes.saturates at 3
  */
 uint8 num_ag_qual_bad;
/*sum of quality metrics per rls when up/down */
 uint16 rg_qual[EUL_MAX_RL];
/*sum of quality metrics per rls when ack/nack is received */
 uint16 hich_qual[EUL_MAX_RL];
/*num of times quality metric is calculated ie. up/down is received*/
 uint8 num_rg_qual_valid[EUL_MAX_RL];
/*num of times quality metric is calculated ie.ack/nack is received*/
 uint8 num_hich_qual_valid[EUL_MAX_RL];
 uint8 num_hich_rls;
 uint8 num_non_srv_rg_rls;
 boolean srv_rg_rls_present;
}eul_log_l1_mac_stats_sample_struct_type;

/* l1 mac stats packet is logged periodically every 200ms.so for 10 ms TTI it is
** 20 frames and for 2 ms TTI it is 100
*/    
#define EUL_L1_MAC_STATS_LOG_PKT_REPORT_FRAMES  100
#define EUL_L1_MAC_STATS_LOG_PKT_REPORT_FRAMES_10MS_TTI  20

/*==========================LOG PKT DEFINITION===============================*/
/*This packet is logged periodically every 200ms, or when a reconfig occurs.*/
LOG_RECORD_DEFINE(EUL_L1_MAC_STATS_LOG_PKT)
  #if defined(FEATURE_WCDMA_DC_HSUPA) || defined(FEATURE_WCDMA_16_QAM)
  uint8                                     version;
  uint8                                     carrier_state;
  #endif /* FEATURE_WCDMA_DC_HSUPA || FEATURE_WCDMA_16_QAM */
 /* The header info*/
  eul_log_l1_mac_stats_info_hdr_struct_type l1_mac_stats_hdr_info[WL1_MAX_UL_CARR_IDX];
 /* The l1 mac stats info*/
  eul_log_l1_mac_stats_info_struct_type     l1_mac_stats_info[WL1_MAX_UL_CARR_IDX];
LOG_RECORD_END

// need to change later, hard code for now
#define EUL_L1_MAC_STATS_LOG_PKT_SIZE(carr_idx) ( \
	    ( (dword) ((( EUL_L1_MAC_STATS_LOG_PKT_type *) 0)->l1_mac_stats_hdr_info))+ \
        ((carr_idx + 1) * sizeof(eul_log_l1_mac_stats_info_hdr_struct_type)) + \
        ((carr_idx + 1) * sizeof(eul_log_l1_mac_stats_info_struct_type)))

/*
#define EUL_L1_MAC_STATS_LOG_PKT_SIZE(carr_idx) (FPOS(EUL_L1_MAC_STATS_LOG_PKT_type, l1_mac_stats_hdr_info) + \
(carr_idx * sizeof(eul_log_l1_mac_stats_info_hdr_struct_type)) + \
(carr_idx * sizeof(eul_log_l1_mac_stats_info_struct_type)))
*/


/* 4313 log packet is useful for analyzing and debugging RF Tx related issues
specially in the presence of EUL. The sample of this packet is collected every
10ms. This packet is delivered after collecting 20 samples or upon reconfiguration
of either DPCH, or HS-DPCCH, or E-DPCH.*/

#define UL_TX_RF_NUM_MAX_SAMPLES 20
typedef PACKED struct PACKED_POST
{
  uint16 beta_c_hw;         /* Beta C HW value returned by FW */
  uint16 beta_d_hw;         /* Beta D HW value returned by FW */
  uint16 beta_hs_hw;        /* Beta HS HW value returned by FW */
  uint16 beta_ec_hw;        /* Beta EC HW value returned by FW */
  uint16 beta_ed_hw0;       /* Beta ED HW value returned by FW  */
  uint16 beta_ed_hw1;
  uint16 beta_ed_hw2;
  uint16 beta_ed_hw3;
  uint16 a_ed_sw;            /* A_ed value returned by L1 */
  uint16 a_ed_sw_sf2;        /* A_ed value returned by L1 for SF2 channel */
  uint8 tx_agc_buffered;    /* Buffered TxAGC value in AGC units */
  uint8 mpr_bin;            /* MPR Bin value calculated by L1 */
  uint8 mpr_reduction;
  uint8 tx_agc_offset;      /* TxAGC offset applied by RF drivers in TX AGC */
  uint16 tx_scale_factor;   /* Tx Scale factor applied by FW, scaled by FW */
}ul_tx_rf_log_pkt_struct_type;

#ifdef FEATURE_WCDMA_DC_HSUPA 
typedef PACKED struct PACKED_POST
{
  uint16 beta_c_sec_hw;         /* Beta C HW value returned by FW for secondary*/
  uint16 beta_ec_sec_hw;        /* Beta EC HW value returned by FW for secondary*/
  uint16 beta_ed_sec_hw0;       /* Beta ED HW value returned by FW  for secondary*/
  uint16 beta_ed_sec_hw1;
  uint16 beta_ed_sec_hw2;
  uint16 beta_ed_sec_hw3;
  uint16 a_ed_sec_sw;            /* A_ed value returned by L1 for secondary */
  uint16 a_ed_sec_sw_sf2;        /* A_ed value returned by L1 for SF2 channel for secondary */
} ul_sec_tx_rf_log_pkt_struct_type;
#endif /* FEATURE_WCDMA_DC_HSUPA  */

LOG_RECORD_DEFINE(UPLINK_TX_RF_DEBUG_LOG_PKT)
  uint8 version;
  uint8 carrier_state;
  uint8 ul_tx_rf_start_cfn;
  uint8 ul_tx_num_samples;
  ul_tx_rf_log_pkt_struct_type ul_tx_rf_log_pkt_info[UL_TX_RF_NUM_MAX_SAMPLES];
  #ifdef FEATURE_WCDMA_DC_HSUPA 
  ul_sec_tx_rf_log_pkt_struct_type ul_sec_tx_rf_log_pkt_info[UL_TX_RF_NUM_MAX_SAMPLES];
  #endif /* FEATURE_WCDMA_DC_HSUPA  */
LOG_RECORD_END

#ifdef FEATURE_WCDMA_CPC_DTX
/* 1.1  CPC DTX (Re)-Configuration log packet (External) 0x4XXX
This log packet is generated when UE sets up or tears down CPC DTX mode or when
the parameter information is reset. In this log Bit 0 represents LSB
*/
typedef PACKED struct PACKED_POST
{
  /*Enumerated (0, 1, 2, 4, 8, 16, 32, 64, 128) in radio frames */
  uint8 dtx_enabling_delay;

  /* Integer (0.. 159) */
  uint8 dtx_drx_offset;

  
  /* Bits 4:0. Represents values (1, 4, 5, 8, 10, 16, 20) in subframes
  ** Bits 7:5 Represents values (1, 2, 5) in subframes  */
  uint8  dtx_cycle1_burst1;

  /*   Enumerated (1, 4, 8, 16, 32, 64, 128, 256) in E-DCH TTIs */
  uint8  dtx_cycle2_inact_thresh;

  /* Bits 9:0. Represent values (4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 128, 160) sub frames
  ** Bits 12:10 Represents values (1, 2, 5) sub frames
  ** Bits 15:14 Take values (0, 1 and 2). Enumerated (2, 4, and 15).
  ** Mapping :
  **   0 - 2
  **   1 - 4
  **   2 - 15  */
  uint16  dtx_cycle2_burst2_long_preambl;

  /*  Enumerated (0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, Infinity) in subframes
       Use 0xFFFF to represent infinity  */
  uint16 cqi_timer;


  /* Integer (0..37,38) */
  uint8  def_sg_cycle_2;

  /* Enumerated (1, 3, 4) */
  uint8  dpcch_slot_format;

  /* Enumerated (1, 4, 5, 8, 10, 16, 20)  in subframes */
  uint8  mac_dtx_cycle;

  /* Enumerated (0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, Infinity)
  ** Use 0xFFFF to represent infinity  */
  uint16 mac_inact_thresh;

}eul_dtx_conf_reconf_log_pkt_struct_type;

LOG_RECORD_DEFINE(EUL_DTX_CONFIG_RECONFIG_LOG_PKT)
uint8   version;
uint8   cpc_dtx_start_cfn;
uint16  dtx_drx_status;
eul_dtx_conf_reconf_log_pkt_struct_type  eul_dtx_conf_reconf_log_pkt;
LOG_RECORD_END


#define DTX_DEBUG_FW_MAX_NUM_SAMPLES 20
#define EUL_FW_DTX_LOG_PKT_VER         5

typedef PACKED struct PACKED_POST
{

  /* DTX Cycle value from Firmware 0 - cycle 1, 1 - cycle 2 */
  uint8 dtx_cycle;

  /* CFN value for which the DTX logging is done
  ** One element per subframe in rising order, holding a total of 2 frames of data.
  ** It records the cycle 2 inactivity threshold down counter for the corresponding
  ** subframe. The counter counts down from cycle 2 inactivity threshold to 0. So if
  ** this is a non-zero value, we are in cycle 1, otherwise in cycle 2.   
  */
   uint16 cpc_dtx_inact_counter[5];

  /* Bit position represents these meanings.
  **   CM_GAP            bit 7
  **   HS Preamble       bit 5
  **   CQI               bit 4
  **   ACK               bit 3
  **   EDCH_PREPOST      bit 2
  **   EDCH              bit 1
  **   BURST             bit 0     
  */
  uint8 cpc_dtx_txstate[15];
}eul_cpc_dtx_fw_state_log_struct_type;

LOG_RECORD_DEFINE(CPC_DTX_FW_STATE_MACHINE_LOG_PKT)
 uint8  version;
 uint8  cpc_dtx_start_cfn;
 uint8  cpc_dtx_tti;
 uint8  dtx_fw_log_pkt_num_samples; 
 uint8  carrier_state;
 eul_cpc_dtx_fw_state_log_struct_type eul_cpc_dtx_fw_log_pkt_info[WL1_MAX_UL_CARR_IDX][DTX_DEBUG_FW_MAX_NUM_SAMPLES];

/* Parser need below buffer after allocation of structure eul_cpc_dtx_fw_state_log_struct_type to form a table */
 uint16 cpc_mac_inactivity_counter[WL1_MAX_UL_CARR_IDX][DTX_DEBUG_FW_MAX_NUM_SAMPLES][5];

#define CPC_DTX_FW_STATE_MACHINE_LOG_PKT_SIZE(carr_idx, samples) (\
        (FPOS(CPC_DTX_FW_STATE_MACHINE_LOG_PKT_type, eul_cpc_dtx_fw_log_pkt_info))+\
        ((carr_idx + 1)*samples*sizeof(eul_cpc_dtx_fw_state_log_struct_type))+\
        ((carr_idx + 1)*samples*sizeof(uint16)*5))
        
LOG_RECORD_END

#endif /*FEATURE_WCDMA_CPC_DTX*/

#ifdef FEATURE_WCDMA_DC_HSUPA 

#define WCDMA_EUL_LASTENTITY_MPRVALS0_MASK  0x7
#define WCDMA_EUL_LASTENTITY_MPRVALS0_SHIFT 0

#define WCDMA_EUL_LASTENTITY_MPRVALS1_MASK  0x38
#define WCDMA_EUL_LASTENTITY_MPRVALS1_SHIFT 3

#define WCDMA_EUL_LASTENTITY_MPRVALS2_MASK  0x1C0
#define WCDMA_EUL_LASTENTITY_MPRVALS2_SHIFT 6

#define WCDMA_EUL_LASTENTITY_WCBHSS0_MASK   0x1E00
#define WCDMA_EUL_LASTENTITY_WCBHSS0_SHIFT  9

#define WCDMA_EUL_LASTENTITY_WCBHSS1_MASK   0x1E000
#define WCDMA_EUL_LASTENTITY_WCBHSS1_SHIFT  13

#define WCDMA_EUL_LASTENTITY_WCBHSS2_MASK   0x1E0000
#define WCDMA_EUL_LASTENTITY_WCBHSS2_SHIFT  17

#define WCDMA_SET_EUL_NEW_MPR_LOG_INFO_FIELD(buf, field1, field2, value) \
(buf) = (((buf) & (~WCDMA_EUL_##field1##_##field2##_MASK))|(((value) << WCDMA_EUL_##field1##_##field2##_SHIFT) &WCDMA_EUL_##field1##_##field2##_MASK))

typedef PACKED struct PACKED_POST
{
  uint8  sample_Num;
  uint16 startsfn;
}eul_new_mpr_dc_logging_header;

typedef PACKED struct PACKED_POST
{
  uint16 beta_c_hw[2];
  uint16 beta_hs_hw;
  uint16 beta_ec_hw[2];
  uint16 beta_ed_hw_0[2];
  uint16 beta_ed_hw_1[2];
  uint16 beta_ed_hw_2[2];
  uint16 beta_ed_hw_3[2];
  uint32 mprVal_wcBetaHS_3slots;
} eul_new_mpr_dc_logging_sample;

LOG_RECORD_DEFINE(WCDMA_MULTI_CARR_EUL_MPR_LOG_PACKET)
  uint8                         version;
  uint8                         carrier_state;
  eul_new_mpr_dc_logging_header hdr_info;
  eul_new_mpr_dc_logging_sample sample_info[E_UL_LOG_PKT_SAMPLE_MAX_2MS_TTI];
LOG_RECORD_END

#define WCDMA_MULTI_CARR_EUL_MPR_LOG_PACKET_SIZE(num_samples) ( \
   (FPOS(WCDMA_MULTI_CARR_EUL_MPR_LOG_PACKET_type, sample_info))+  \
   (num_samples * sizeof(eul_new_mpr_dc_logging_sample)))
#endif

#ifdef FEATURE_WCDMA_DC_HSUPA 
#define WCDMA_EUL_ENTITY0_STARFN_MASK  0x7FF
#define WCDMA_EUL_ENTITY0_STARFN_SHIFT 0

#define WCDMA_EUL_ENTITY0_STHQID_MASK  0x3800
#define WCDMA_EUL_ENTITY0_STHQID_SHIFT 11

#define WCDMA_EUL_ENTITY0_BOOST_MASK   0x4000
#define WCDMA_EUL_ENTITY0_BOOST_SHIFT  14

#define WCDMA_EUL_ENTITY1_DPS0C1_MASK  0x3FF
#define WCDMA_EUL_ENTITY1_DPS0C1_SHIFT 0

#define WCDMA_EUL_ENTITY1_DPS1C1_MASK  0xFFC00
#define WCDMA_EUL_ENTITY1_DPS1C1_SHIFT 10

#define WCDMA_EUL_ENTITY1_DPS2C1_MASK  0x3FF00000
#define WCDMA_EUL_ENTITY1_DPS2C1_SHIFT 20

#define WCDMA_EUL_ENTITY1_SF2IC1_MASK  0xC0000000
#define WCDMA_EUL_ENTITY1_SF2IC1_SHIFT 30

#define WCDMA_EUL_ENTITY2_DPS0C2_MASK  0x3FF
#define WCDMA_EUL_ENTITY2_DPS0C2_SHIFT 0

#define WCDMA_EUL_ENTITY2_DPS1C2_MASK  0xFFC00
#define WCDMA_EUL_ENTITY2_DPS1C2_SHIFT 10

#define WCDMA_EUL_ENTITY2_DPS2C2_MASK  0x3FF00000
#define WCDMA_EUL_ENTITY2_DPS2C2_SHIFT 20

#define WCDMA_EUL_ENTITY2_SF2IC2_MASK  0xC0000000
#define WCDMA_EUL_ENTITY2_SF2IC2_SHIFT 30

#define WCDMA_EUL_ENTITY3_TXAGC0_MASK  0x3FF
#define WCDMA_EUL_ENTITY3_TXAGC0_SHIFT 0

#define WCDMA_EUL_ENTITY3_TXAGC1_MASK  0xFFC00
#define WCDMA_EUL_ENTITY3_TXAGC1_SHIFT 10

#define WCDMA_EUL_ENTITY3_TXAGC2_MASK  0x3FF00000
#define WCDMA_EUL_ENTITY3_TXAGC2_SHIFT 20

#define WCDMA_EUL_ENTITY3_EInva1_MASK  0x40000000
#define WCDMA_EUL_ENTITY3_EInva1_SHIFT 30

#define WCDMA_EUL_ENTITY3_EInva2_MASK  0x80000000
#define WCDMA_EUL_ENTITY3_EInva2_SHIFT 31

#define WCDMA_EUL_ENTITY4_RETX12_MASK  0x03
#define WCDMA_EUL_ENTITY4_RETX12_SHIFT 0

#define WCDMA_EUL_ENTITY4_HRQPO1_MASK  0x1C
#define WCDMA_EUL_ENTITY4_HRQPO1_SHIFT 2

#define WCDMA_EUL_ENTITY4_HRQPO2_MASK  0xE0
#define WCDMA_EUL_ENTITY4_HRQPO2_SHIFT 5

#define WCDMA_EUL_ENTITY6_HRQPONS_MASK  0x7
#define WCDMA_EUL_ENTITY6_HRQPONS_SHIFT 0

#define WCDMA_EUL_ENTITY6_MTPL10_MASK  0x1FF8
#define WCDMA_EUL_ENTITY6_MTPL10_SHIFT 3

#define WCDMA_EUL_ENTITY6_AEDCX1_MASK  0x3FE000
#define WCDMA_EUL_ENTITY6_AEDCX1_SHIFT 13

#define WCDMA_EUL_ENTITY6_AEDCX2_MASK  0x7FC00000
#define WCDMA_EUL_ENTITY6_AEDCX2_SHIFT 22

#define WCDMA_EUL_ENTITY9_AHSCX_MASK   0x3F
#define WCDMA_EUL_ENTITY9_AHSCX_SHIFT  0

#define WCDMA_EUL_ENTITY9_SG1_MASK     0xFC0
#define WCDMA_EUL_ENTITY9_SG1_SHIFT    6

#define WCDMA_EUL_ENTITY9_SG2_MASK     0x3F000
#define WCDMA_EUL_ENTITY9_SG2_SHIFT    12

#define WCDMA_EUL_ENTITY9_METFCI1_MASK  0x1FC0000
#define WCDMA_EUL_ENTITY9_METFCI1_SHIFT 18

#define WCDMA_EUL_ENTITY9_METFCI2_MASK  0xFE000000
#define WCDMA_EUL_ENTITY9_METFCI2_SHIFT 25

#define WCDMA_EUL_DB10_OFFSET_VAL       512

#define WCDMA_SET_EUL_POWER_ALLOC_LOG_INFO_FIELD(buf, field1, field2, value) \
(buf) = (((buf) & (~WCDMA_EUL_##field1##_##field2##_MASK))|(((value) << WCDMA_EUL_##field1##_##field2##_SHIFT) &WCDMA_EUL_##field1##_##field2##_MASK))

typedef PACKED struct PACKED_POST
{
  uint8 sample_Num;
  uint16 startSfn_startHarq_boost;
} eul_power_alloc_dc_logging_header;

typedef PACKED struct PACKED_POST
{
  uint32 dpcch_power_3slots_sf_index_c1;
  uint32 dpcch_power_3slots_sf_index_c2;
  uint32 txagc3slots;
  uint8  retx_harqPo_c1_c2;
  uint16 non_sg_payload;
  uint32 harqPoNs_mtpl_aedcx15C1C2;
  uint8  a_ecx15_c1;
  uint8  a_ecx15_c2;
  uint32 ahsx15_sgC1C2_maxEtfciC1C2;
  uint32 powerAllocC1;
  uint32 powerAllocC1C2;
  uint32 powerAllocC2;
} eul_power_alloc_dc_logging_sample;

LOG_RECORD_DEFINE(WCDMA_MULTI_CARR_EUL_POWER_ALLOC_LOG_PACKET)
  uint8                             version;
  uint8                             carrier_state;
  eul_power_alloc_dc_logging_header hdr_info;
  eul_power_alloc_dc_logging_sample sample_info[E_UL_LOG_PKT_SAMPLE_MAX_2MS_TTI];
LOG_RECORD_END

#define WCDMA_MULTI_CARR_EUL_POWER_ALLOC_LOG_PKT_SIZE(num_samples) ( \
   (FPOS(WCDMA_MULTI_CARR_EUL_POWER_ALLOC_LOG_PACKET_type, sample_info))+  \
   (num_samples * sizeof(eul_power_alloc_dc_logging_sample)))

#endif

/*======================================================================
                  END OF LOG PACKET DEFINITIONS
======================================================================*/

/* The strcture is used to send local cmd to l1m from EUL FIQ handler for later
   processing and sending to DIAG
*/
typedef enum{
  /* channel cfg info log packet is done local cmd. This cmd is sent from cfg
    event handler to l1m*/
  EUL_UL_CHAN_CFG_INFO_LOG_PKT_DONE,
  /*combined l1 mac info log packet done local cmd. This cmd is sent from FIQ 
  handler to l1m*/
  EUL_COMBINED_L1_MAC_LOG_PKT_DONE,
  /* This local cmd is used to submit other uplink log packets from FIQ handler
   to l1m*/
  EUL_PERIODIC_LOG_PKT_DONE
}eul_ul_logging_done_cmd_enum_type;

/*This strcture is used to fill the downlink results in the uplink log packets*/
typedef struct {
  uint32  applied_sg_lupr[WL1_MAX_UL_CARR_IDX];
  boolean first_tx_acked[WL1_MAX_UL_CARR_IDX];
  boolean max_retx_nacked[WL1_MAX_UL_CARR_IDX];
  uint16  eul_cur_cfn_num[WL1_MAX_UL_CARR_IDX];
}e_ul_log_pkt_dl_results_struct_type;

extern uint8 e_ul_log_pkt_max_num_samples;
extern uint8 e_ul_log_pkt_max_num_4311_samples;
extern uint8 e_ul_log_pkt_max_num_10ms_fr_samples;
/* ========================================================================
                 ** Public Data Declarations
** =======================================================================*/
extern eul_l1_mac_info_flush_struct_type e_ul_l1_mac_log_pkt_flush_info[WL1_MAX_UL_CARR_IDX];
extern eul_l1_mac_info_buf_struct_type  e_ul_l1_mac_log_pkt_sample_info[WL1_MAX_UL_CARR_IDX];
extern boolean e_ul_l1_mac_log_sample_info_dirty_bit[2];
extern uint8 e_ul_l1_mac_log_sample_info_buf_idx;
extern uint8 e_ul_l1_mac_log_sample_info_log_submit_buf_idx;

extern EUL_COMBINED_L1_MAC_LOG_PKT_type  e_ul_l1_mac_info_log_pkt;

extern eul_edpch_log_pkt_struct_type  e_ul_edpch_log_pkt_samples[WL1_MAX_UL_CARR_IDX][2];
extern EUL_UL_EDPCH_INFO_LOG_PKT_type  e_ul_edpch_info_log_pkt[2];
extern uint8 e_ul_edpch_log_pkt_buf_idx;
extern uint8 e_ul_edpch_log_pkt_log_submit_buf_idx;
extern boolean e_ul_edpch_log_pkt_dirty_bit[2];

extern uint32 e_ul_etfc_restrict_log_pkt_sample_num[2];
extern uint32 e_ul_etfc_restrict_log_pkt_frame_num[2];
extern boolean e_ul_etfc_restrict_submit_to_diag[2];

extern EUL_UL_ETFC_RESTRICT_INFO_LOG_PKT_type e_ul_etfc_restrict_info_log_pkt[2];
extern uint8 e_ul_etfc_restrict_buf_idx;
extern uint8 e_ul_etfc_restrict_log_submit_buf_idx;
extern boolean e_ul_etfc_restrict_dirty_bit[2];

extern uint32 e_ul_rm_log_pkt_sample_num[WL1_MAX_UL_CARR_IDX][2];
extern EUL_UL_RATE_MATCHING_INFO_LOG_PKT_type e_ul_rate_match_info_log_pkt[2];

extern boolean e_ul_rate_match_info_dirty_bit[2];
extern uint8 e_ul_rate_match_info_buf_idx;
extern uint8 e_ul_rate_match_info_log_submit_buf_idx;
/*CR 141417*/
extern uint32 e_ul_l1_mac_stats_log_pkt_sample_num[WL1_MAX_UL_CARR_IDX][2];
extern EUL_L1_MAC_STATS_LOG_PKT_type e_ul_l1_mac_stats_info_log_pkt[2];
extern eul_log_l1_mac_stats_sample_struct_type e_ul_l1_mac_stats_sum_data[WL1_MAX_UL_CARR_IDX][2];

extern boolean e_ul_l1_mac_stats_log_pkt_num_dirty_bit[2];
extern uint8 e_ul_l1_mac_stats_log_pkt_buf_idx;
extern uint8 e_ul_l1_mac_stats_log_pkt_log_submit_buf_idx;


/*===========================================================================
                    FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
FUNCTION         E_UL_EDPCH_INFO_PKT_HANDLER

DESCRIPTION     This routine sends edpch info log packets to diag.
                This packet is delivered every 200ms or upon reconfiguration 
                         

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_ul_edpch_info_pkt_handler(void);

/*===========================================================================
FUNCTION         E_UL_L1_MAC_PKT_HANDLER

DESCRIPTION     This routine sends combined l1 mac info log packets to diag.
                This packet is delivered every 200ms or upon reconfiguration 
                or active set update

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_ul_l1_mac_pkt_handler(void);

/*===========================================================================
FUNCTION        E_UL_L1_MAC_PKT_FLUSH_HANDLER

DESCRIPTION     This routine decides which sample buffers to read from and fill 
                the log packet buffer and also the flush buffer(with the last 
                four samples) and sends combined l1 mac info log packets to diag.
                This is called upon reconfiguration or active set update

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void e_ul_l1_mac_pkt_flush_handler(boolean edl_recfg);

/*===========================================================================
FUNCTION         E_UL_SEND_ETFC_RESTRICT_INFO_TO_DIAG

DESCRIPTION     This routine sends etfc restrict info log packets to diag.
                This packet is delivered every 200ms or upon reconfiguration 
                         

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void   e_ul_send_etfc_restrict_info_to_diag(void);

/*===========================================================================
FUNCTION         E_UL_SEND_RM_INFO_TO_DIAG

DESCRIPTION     This routine sends rm info log packets to diag.
                This packet is delivered every 200ms or upon reconfiguration 
                         

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void   e_ul_send_rm_info_to_diag(void);

/*===========================================================================
FUNCTION         E_UL_SEND_L1_MAC_STATS_INFO_TO_DIAG

DESCRIPTION     This routine sends l1 mac stats info log packets to diag.
                This packet is delivered every 200ms or upon reconfiguration 
                        

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void   e_ul_send_l1_mac_stats_info_to_diag(void);

/*===========================================================================
FUNCTION         E_UL_SEND_EDPCH_INFO_PKT_TO_DIAG

DESCRIPTION     This routine sends edpch info log packets to diag.
                This packet is delivered every 200ms or upon reconfiguration 
                         

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void   e_ul_send_edpch_info_pkt_to_diag(void);

/*===========================================================================
FUNCTION         E_UL_LOGGING_CMD_HANDLER

DESCRIPTION      This routine sends hsupa uplink module log packets to diag

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_ul_logging_cmd_handler(eul_ul_logging_done_cmd_enum_type log_pkt_type);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================
FUNCTION         DC_HSUPA_ACTIVATION_LOGGING_CMD_HANDLER

DESCRIPTION     This routine sends e_ul logs to diag when DC is configured 
                and activates the secondary EUL and EDL. This called when
                HS SCCH order is rcvd. to activate Secondary UL.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void dc_hsupa_activation_logging_cmd_handler(void);
/*===========================================================================
FUNCTION         DC_HSUPA_DEACTIVATION_LOGGING_CMD_HANDLER

DESCRIPTION     This routine sends e_ul logs to diag when DC is activated and 
                         deactivated the secondary EUL and EDL. This called when HS 
                         SCCH order is rcvd. to activate Secondary UL.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void dc_hsupa_deactivation_logging_cmd_handler(void);

#endif

/*===========================================================================
FUNCTION      E_UL_CLEANUP_LOG_PKTS_DATA

DESCRIPTION   This routine initializes all the global variable information
              maintained for EUL logpkts

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  All the logpkts are re-initialized if reset_l1_mac_logpkts is true
              Else all the logpkts except 4309/4311 are flushed

===========================================================================*/
extern void e_ul_cleanup_log_pkts_data(boolean reset_l1_mac_logpkts);
#endif /*EULLOG_H*/


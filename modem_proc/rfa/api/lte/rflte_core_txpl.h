#ifndef RFLTE_CORE_TXPL_H
#define RFLTE_CORE_TXPL_H

/*!
  @file
  rflte_core_txpl.h

  @brief
  Provides TXPL control.

  @details


*/

/*===========================================================================

Copyright (c) 2010 - 2015 by QUALCOMM TECHNOLOGY, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/lte/rflte_core_txpl.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/30/15   bir     Added support for tx max pwr based EARFCN NV
09/11/15   as      64QAM initial support
11/10/14   pkg     Interface changes to support LTE UL CA Feature.
08/18/14   kab     TxPL changes to support SGLTE Tx power backoff with Coex.
07/25/14    vb     NLIC clock mode support
07/31/14   ndb     Added support for extended EARFCN for LTE
07/16/14   sbm     return mtpl for srs_settings_per_sf. 
05/16/14    pv     Added support for Carrier index to TxPL enable API.
05/06/14    pv     Reduced the FBRx Power threshold to -40dB based on RF Systems's reco.
04/17/14    pv     Added support for passing threshold for power tracking method
04/17/14   svi     PA static NV and code redesign
03/14/14   bsh     NlIC changes: ML1 API call with ul power - API name change
03/14/14   bsh     NlIC changes: ML1 API call with ul power
02/24/14    pv     Added support for FBRx Gain error processing with MTPL.
02/21/14   cri     Fix null access crash during freq comp
02/18/14    pv     Added support for FBRx Gain error processing with Common FBRx Trigger. 
02/18/14   cri     Only allocate freq comp during tx tune
02/13/14    pv     Added support for FBRx Gain error processing.
02/05/14   cri     Freq comp optimization for DM population
09/19/13   bar     ET optimizations for freq comp/pin/pout/delay 
07/29/13   as      Added support for relative backoff on SAR limit based on 
                   TDD-LTE ULDL configuration 
07/24/13    pv     Support for Delay vs Temp comp Enum.
07/02/13   ndb     Added dbm10 to TXAGC & TXAGC to dbm10 conversion macros
06/12/13   php     Increase HDET buffer size to accomodate increase in PDET sample size 
05/07/13   jf      Add multiple freq comp bins support for XPT   
05/02/13   cri     Store pin/pout comp in common structure for calv4
03/22/13   jf      Add 2D lin_vs_temp_vs_freq_comp 
03/15/13   gvn     Update sgsar, coex_WWAN, coex_WLAN limits using MSGR 
03/14/13   jf      Add XPT path delay support 
03/08/13   yzw     Add # define for max tx power 
02/19/13   sbm     Bandedge max power backoff support in LTE(Nikel CR375880) 
12/26/12   gvn     LTE Max Pwr limiting support during WLAN Coex scenario 
                   (merge from Nikel CR401193) 
12/10/12   pl      Remove obsolete TXPL interface 
12/10/12   pl      Add new API for PHR backoff reporting support 
12/04/12   php     Add support for getting HDET reads from FW IND message
11/20/12   gvn     Featurize LTE for Triton 
09/20/12   pl      Identify interfaces referenced by ML1
05/04/12   pl      New MTPL Implemenation
03/22/12   pl      Remove unused interface
03/22/12   pl      Remove obsolete interface
05/23/12   as      LTE Tx power limit for SGLTE SAR and CoEx
04/04/12   pl      Remove obsolete interfaces
01/11/12   cri     Update to LTE Tx Frequency comensation algorithm
11/03/11   pl      Support PHR-Less reporting
09/14/11   can     Merge MDM9K tips to NikeL.
08/18/11   pl      remove unused interface, removed exposed interface that are 
                   supposed to be internal functions
05/26/11   can     HDET Fast Correction (Super ACQ) mode support.
03/04/11   can     LTE SAR support based on index from rfm layer and NV
02/09/11   aca     FTM dac setpoint value override support
01/24/11   aca	   FTM Override DAC Setpoint
12/13/10   kma     Added API rflte_core_txpl_get_pcmax() to acquire pcmax 
11/08/10   can     Added support for emergency overheat TX power backoff.
10/22/10   can     Perform lim vs freq based on the new HDET MSGR message containing Physical RB allocation.
10/22/10   can     Support to calculate MTPL & freq comp for slot1, slot2 and SRS accurately.
10/20/10   can     Support to return the delta between the MTPL and the desired limit.
08/31/10   can     Change to get the DAC setpoint from RFSW TXPL code.
08/11/10   can     Support for LTE TXAGC logging and code clean-up.
08/05/10   can     Updates to SAR backoff implemetation for PHR update.
08/04/10   whc     Changed mtpl_to_l1 calculation to dB/10 using int16.
07/14/10   can     Changes to optimize HDET Low Power Mode.
07/12/10   can     Support for when FW reads HDET ADC and passes RFSW the value.
06/28/10   can     Added SAR support for SVLTE.
05/27/10   can     LOW PDET MODE HDET support.  
05/25/10   can     LIM vs FREQ based on RB allocation.  
05/21/10   can     Added LTE TX RB based FREQ COMP support.  
05/17/10   can     LTE MPR, AMPR and PEMAX support.
05/06/10   tnt     Adding p_cmax routine
05/05/10   can     Initial version.

============================================================================*/

#include "rfcom.h"

#ifdef FEATURE_LTE
#include "rffw_lte_intf.h"
#include "rflte_msg.h"
#include "nv_items.h"
#include "rflte_ext_mc.h"
#include "rflm_api_lte.h"


#define RFLTE_CORE_TXPL_MAX_NUM_SUBFRAMES_PER_FRAME  10
/* -70 dBm in dB10 = -700, 0dBm = 700 in TXAGC count */
#define RFLTE_CORE_TXPL_TXAGC_COUNT_MIN_OFFSET       700
#define RFLTE_CORE_TXPL_HDET_RD_RESULT_NUM_TRANS 12

#define RFLTE_CORE_TXPL_TXAGC_RESOLUTION         10


#define RFLTE_CORE_TXPL_MIN_TX_PWR_FLOOR_DBM        -70
#define RFLTE_CORE_TXPL_MIN_TX_PWR_FLOOR_DB10       (RFLTE_CORE_TXPL_MIN_TX_PWR_FLOOR_DBM * RFLTE_CORE_TXPL_TXAGC_RESOLUTION)

/* min value for TX power limit in dbm */
#define RFLTE_CORE_TXPL_MIN_TX_PWR_LIM           -44
#define RFLTE_CORE_TXPL_MIN_TX_PWR_LIM_DB10      (RFLTE_CORE_TXPL_MIN_TX_PWR_LIM * RFLTE_CORE_TXPL_TXAGC_RESOLUTION)

/* maximum value for TX power limit in dbm */
#define RFLTE_CORE_TXPL_MAX_TX_PWR_LIM           23
#define RFLTE_CORE_TXPL_MAX_TX_PWR_LIM_DB10      (RFLTE_CORE_TXPL_MAX_TX_PWR_LIM * RFLTE_CORE_TXPL_TXAGC_RESOLUTION)

/* max value for Tx power in dbm*/
#define RFLTE_CORE_TXPL_MAX_TX_POWER_DBM        32.3
#define RFLTE_CORE_TXPL_MAX_TX_POWER_DB10       (RFLTE_CORE_TXPL_MAX_TX_POWER_DBM * RFLTE_CORE_TXPL_TXAGC_RESOLUTION)

/* FBRx Power Threshold */
#define RFLTE_CORE_TXPL_FBRX_PWR_THRESHOLD_DB10         -400

#ifdef RFLTE_BOLT_UL_CA_SUPPORTED
/* Max number of Uplink Carrier Components Supported */
#define MAX_UL_CA_CARRIERS 2
#endif /* RFLTE_BOLT_UL_CA_SUPPORTED */

/*===========================================================================

  MACROS for TXAGC to DBM10 conversion and DBM10 to TXAGC conversion

===========================================================================*/
#define RFLTE_GET_PWR_IN_DBM10_FROM_TXAGC_COUNT(txagc) \
        ( txagc - RFLTE_CORE_TXPL_TXAGC_COUNT_MIN_OFFSET);

/*=========================================================================*/

#define RFLTE_GET_TXAGC_COUNT_FROM_PWR_IN_DBM10(pwrdbm10) \
        ( pwrdbm10 + RFLTE_CORE_TXPL_TXAGC_COUNT_MIN_OFFSET);

/*=========================================================================*/

/* Structure Definitions */
/*--------------------------------------------------------------------------*/
typedef struct
{
    uint16 addr; /*only 8bits for now*/
    uint8 value;
} rflte_core_txpl_hw_rd_type;

typedef struct
{
    rflte_core_txpl_hw_rd_type hdet_rd[RFLTE_CORE_TXPL_HDET_RD_RESULT_NUM_TRANS];
    uint8 num_trans;
} rflte_core_txpl_hdet_rd_result_type;

#ifdef RFLTE_BOLT_UL_CA_SUPPORTED

/* ENUM to indicate current Uplink band configuration */
typedef enum
{
  /* Default scenario. One carrier Component,
  ** one Resource Block */
  SINGLE_CARRIER = 0,

  /* Intra Band CA with One carrier Two 
  ** Resource Blocks */
  MULTI_CARRIER_INTRA_BAND_CA = 1,

  /* Inter Band CA with Two Carrier Components */
  MULTI_CARRIER_INTER_BAND_CA = 2,

  LTE_UPLINK_MAX_BAND = 3,
}rflte_core_ul_carrier_config_type_e;

typedef struct
{
  /* Number of Component(Uplink) Carriers this ML1 request is for*/
  uint8                                     num_cc;
  /* Carrier Index bitmask for which this request needs to be 
  ** processed from ML1. Each SET bit indicates the active
  ** carrier.
  ** Ex: 0000 0011 indicates that Primary and Secondary Carrier
  **     Components are active. */
  uint8                                     carrier_index_bmsk;
  /*  Current carrier configuration */
  rflte_core_ul_carrier_config_type_e       curr_ul_carrier_config;
  /* Sub-Frame number for which the MTPL and SRS Values are to be
  ** provided to FED (value ranges between 0-9) */
  /* Action subframe number: srs_subframe_num */
  uint8                                     subframe_num[MAX_UL_CA_CARRIERS];
  uint8                                     srs_subframe_num[MAX_UL_CA_CARRIERS];
  /* Slot 1 Starting Resource Block value */
  uint8                                     rb_start_slot1[MAX_UL_CA_CARRIERS];
  /* Start RB number for the SRS Sub Frame for each Tx Cell in 
  ** Single Tx CA cycle */
  uint8                                     srs_rb_start[MAX_UL_CA_CARRIERS];
  /* Slot 2 Starting Resource Block value */
  uint8                                     rb_start_slot2[MAX_UL_CA_CARRIERS];
  /* Resource Block */
  uint8                                     rb_block[MAX_UL_CA_CARRIERS];
  /* RB Block Size for the SRS Sub Frame for each Tx Cell in 
  ** Single Tx CA cycle */
  uint8                                     srs_rb_block[MAX_UL_CA_CARRIERS];
  /* Used for Stand-alone UpLink Type of NS requested for the Sub Frame */
  uint8                                     ns_x[MAX_UL_CA_CARRIERS];
  /* P_emax for the subframe */
  int16                                     p_emax[MAX_UL_CA_CARRIERS];
  /* Pe_Max for the SRS Sub frame */
  int16                                     srs_p_emax[MAX_UL_CA_CARRIERS];
  /* Co-Ex Max for the subframe */
  int16                                     cxm_pmax[MAX_UL_CA_CARRIERS];
  /* Flag to indicate if SRS is active */
  boolean                                   is_srs_active[MAX_UL_CA_CARRIERS];
  /* If PHR reporting is enabled, return MTPL including 
  ** CxM power limit. Otherwise, ie. PHRLess == TRUE, 
  ** return MTPL disregarding the CxM power limit */
  boolean                                   phrless_flag[MAX_UL_CA_CARRIERS];
  /* TRUE  : If CA NS Back-off has been requested by Network
  ** FALSE : If CA NS Back-off has not been requested by Network */
  boolean                                   ca_ns_active[MAX_UL_CA_CARRIERS];
  /* Compute reference pcmax as requested by ML1 */
  boolean                                   compute_ref_pcmax[MAX_UL_CA_CARRIERS];
  /* Type of UL modulation */
  rflm_lte_bw_type_s                        modulation_type[MAX_UL_CA_CARRIERS];
  /* Action subframe type */
  rflm_lte_ul_chan_type_e                   sf_type[MAX_UL_CA_CARRIERS];
} rflte_core_compute_mtpl_input_param_s;

typedef struct
{
  /* Final calculated MTPL */
  int16                       final_mtpl[MAX_UL_CA_CARRIERS];
  /* Final calculated Power headroom */
  int16                       final_dtpl[MAX_UL_CA_CARRIERS];
  /* Computed Ref PcMax for ML1 */
  int16                       ref_pcmax[MAX_UL_CA_CARRIERS];
} rflte_core_compute_mtpl_output_param_s;

#endif /* RFLTE_BOLT_UL_CA_SUPPORTED */

/* 2D Lin vs temp vs freq P_in and P_out comp structure */
typedef enum
{
  RFLTE_CORE_TXPL_LEGACY_COMP = 0,
  RFLTE_CORE_TXPL_XPT_P_IN = 1,
  RFLTE_CORE_TXPL_XPT_P_OUT = 2,
}rflte_core_txpl_linearizer_comp_type;


typedef enum
{
    RFLTE_CORE_NLIC_WB = 1,
    RFLTE_CORE_NLIC_NB,
    RFLTE_CORE_NLIC_WB_NB,
    RFLTE_CORE_NLIC_NONE,
}rflte_core_nlic_wb_nb_type;


/* L1 External */
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_set_mtpl_for_overheat(int16 overheat_limit_backoff_dB10);
/*--------------------------------------------------------------------------*/
uint16 rflte_core_txpl_get_dac_setpoint (rfcom_lte_band_type band_dac_setpoint);
/*--------------------------------------------------------------------------*/
int16 rflte_core_txpl_get_actual_vs_digital_tx_pwr_delta (void);
/*--------------------------------------------------------------------------*/
int16 rflte_core_txpl_get_sar(void);
/*--------------------------------------------------------------------------*/
int16 rflte_core_txpl_get_sar_pcmax_offset(void);
/*--------------------------------------------------------------------------*/
void rflte_core_clear_freq_comp_offsets(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
int32 rflte_core_compute_srs_settings_per_sf(uint8 srs_subframe_num,
                                            uint8 srs_rb_start, 
                                            uint8 srs_rb_block, 
                                            uint8 srs_ns_x, 
                                            int16 srs_p_emax);
/*--------------------------------------------------------------------------*/
int16 rflte_core_compute_p_cmax_per_sf_with_phr(uint8 subframe_num,
                                       uint8 rb_start_slot1, 
                                       uint8 rb_start_slot2,
                                       uint8 rb_block, 
                                       rflm_lte_mod_type_s mod, 
                                       uint8 ns_x, 
                                       int16 p_emax, 
                                       boolean phrless_flag,
                                       int16 cxm_pmax );
/*--------------------------------------------------------------------------*/
int16 rflte_core_compute_p_cmax_per_sf_with_phr_backoff(uint8 subframe_num,
                                       uint8 rb_start_slot1, 
                                       uint8 rb_start_slot2,
                                       uint8 rb_block, 
                                       rflm_lte_mod_type_s mod, 
                                       uint8 ns_x, 
                                       int16 p_emax, 
                                       boolean phrless_flag,
                                       int16 cxm_pmax,
                                       int16 *phr_backoff_db10 );
/*--------------------------------------------------------------------------*/
void rflte_core_compute_p_cmax_update(uint32 carrier_idx,
                                      uint8 subframe_num,
                                      rfa_rf_lte_txpl_params_s *tpl_params,
                                      uint8 pa_swpt_backoff);

#ifdef RFLTE_BOLT_UL_CA_SUPPORTED
/*!---------------------------------------------------------------------------------------
  @brief
  This API calculates the Power Head Room for ML1 and provides the MTPL value to FED
  based on the information passed by ML1 per Sub Frame.

  @details
  This API calculates the Power Head Room for ML1 and provides the MTPL value to FED
  based on the information passed by ML1 per Sub Frame.
  Case 1: Normal Slot
          In case of normal slot (slot 1, slot 2 with out srs) this API computes MTPL
          and PHR and provides it to ML1.
  Case 2: SRS
          In case of SRS slot this API computes MTPL and provies it to ML1. PHRless
          flag is used to skip PHR calculation for SRS. 

  For Single Tx CA cycle, it calculates the Freq Comp for all the Tx Cells in the Single Tx
  CA and updated the average to FED

  @param input_params pointer

  @param output_params pointer
 
  @result
  The Final MTPL calculated for the Sub Frame.
-------------------------------------------------------------------------------------------*/
void rflte_core_compute_mtpl_per_sf(
                                    rflte_core_compute_mtpl_input_param_s   *input_params,
                                    rflte_core_compute_mtpl_output_param_s  *output_params
                                   );
#endif /* RFLTE_BOLT_UL_CA_SUPPORTED */

/* RFA internal */
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_init(void);
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_enable(uint32 carrier_idx,boolean enable,rfcom_lte_earfcn_type tx_channel);
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_set_hdet_low_power_mode (boolean enable);
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_set_mtpl_for_sar(int16 sar_limit_dBm);
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_set_mtpl_for_sg(uint32 plimit_mask, int16 sar_limit_dB10, int16 coex_limit_dB10, int16 wcn_limit_dB10, int16 subframe_coex_limit_dB10);

/*--------------------------------------------------------------------------*/
void rflte_core_txpl_do_mtpl_fw_reads_hdet_phy_rb(int16 txpl_txagc_val, 
                                                 uint16 hdet_adc,
                                                 uint8 phy_rb_block,
                                                 uint8 phy_start_rb,
                                                 rflte_core_txpl_hdet_rd_result_type hdet_rd_event_result);  

/* This API will support the FBRx MTPL update */
void rflte_core_txpl_do_fbrx_based_mtpl_update(uint8 carrier_idx, 
                                               int16 txpl_txagc_val, 
                                               int16 fbrx_gain_err,
                                               uint8 phy_rb_block,
                                               uint8 phy_start_rb,
                                               uint8 sys_bw );  
/*--------------------------------------------------------------------------*/
int32 rflte_core_txpl_get_mtpl_for_logging(void);
/*--------------------------------------------------------------------------*/
int16 rflte_core_txpl_get_sar_limit_for_logging(void);
/*--------------------------------------------------------------------------*/
uint16 rflte_core_txpl_get_hdet_adc_for_logging(void);
/*--------------------------------------------------------------------------*/
int16 rflte_core_txpl_get_hdet_for_logging(void);
/*--------------------------------------------------------------------------*/
uint8 rflte_core_txpl_get_mpr_for_logging(void);
/*--------------------------------------------------------------------------*/
uint8 rflte_core_txpl_get_ampr_for_logging(void);
/*--------------------------------------------------------------------------*/
uint8 rflte_core_txpl_get_ns_flag_for_logging(void);
/*--------------------------------------------------------------------------*/
uint8 rflte_core_txpl_get_hdet_mode_for_logging(void);
/*--------------------------------------------------------------------------*/
boolean rflte_core_txpl_set_dac_setpoint(uint16 dac_setpoint_override_enable, uint16 dac_setpoint_value);
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_set_sar_limit_index(void);
/*--------------------------------------------------------------------------*/
uint16 rflte_core_txpl_get_pdet_mode_swpt(void);
/*--------------------------------------------------------------------------*/
boolean rflte_core_txpl_get_low_power_mode_enabled(void);
/*--------------------------------------------------------------------------*/
int16 rflte_core_txpl_get_pcmax(void);
/*--------------------------------------------------------------------------*/
uint16 rflte_core_txpl_get_hdet_threshold(void);
/*--------------------------------------------------------------------------*/
uint8 rflte_core_txpl_get_lim_vs_freq_rb_block(void);
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_update_param(rfa_rf_lte_txpl_params_s *txpl_params);
/*--------------------------------------------------------------------------*/
int16 rflte_core_txpl_get_min_tx_power_nv(rfcom_lte_band_type rfcom_lte_band);
/*--------------------------------------------------------------------------*/
int16 rflte_core_txpl_get_nv_txpl(void);
/*--------------------------------------------------------------------------*/
boolean rflte_core_txpl_set_mtpl_for_coex_wlan( int16 coex_limit_db10);
/*--------------------------------------------------------------------------*/
int32 rflte_core_xpt_path_delay_comp(rfcom_lte_earfcn_type tx_channel,
                                     rfcom_lte_bw_type bw,
                                     uint8 rb_start,
                                     uint8 rb_block,
                                     boolean delav_vs_temp_comp_enabled,
                                     boolean ftm_mode_char_enabled,
                                     uint16 therm_scaled_read);
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_set_tdd_config_backoff(rfa_rf_lte_tdd_ul_dl_cfg_type tdd_ul_dl_cfg);
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_set_sar_tdd_cfg_backoff( uint16 tdd_config_limit_dB10 );
/*--------------------------------------------------------------------------*/
uint16 rflte_core_txpl_get_sar_tdd_cfg_backoff(void);
/*--------------------------------------------------------------------------*/
int16 rflte_core_txpl_get_rc_proc_var(void);
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_set_rc_proc_var(int16 rc_proc_var);
/*--------------------------------------------------------------------------*/
void rflte_core_pwr_trking_loop_timer_update(uint32 carrier_idx, 
                                             uint32 loop_period);
/*--------------------------------------------------------------------------*/
void rflte_core_def_tx_pwr_trking_loop_timer(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
void rflte_core_undef_tx_pwr_trking_loop_timer(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
uint32 rflte_core_get_pwr_trking_loop_timer(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
uint16 rflte_core_txpl_get_fbrx_gain_err_for_logging(uint32 carrier_idx);

typedef enum
{
  RFLTE_CORE_TXPL_MODE_ACQ,
  RFLTE_CORE_TXPL_MODE_TRK,
  RFLTE_CORE_TXPL_MODE_INIT,
  RFLTE_CORE_TXPL_MODE_DISABLED,
  RFLTE_CORE_TXPL_MODE_INVALID,
} rflte_core_txpl_mode_type;

typedef enum
{
  RFLTE_CORE_TXPL_HDET_LPM_MODE,
  RFLTE_CORE_TXPL_HDET_HPM_MODE,
}rflte_core_txpl_hdet_pwr_type;

typedef enum
{
  RFLTE_CORE_TXPL_MTPL_MODE,
  RFLTE_CORE_TXPL_LIN_UPDATE_MODE,
  RFLTE_CORE_TXPL_INVALID_MODE,
}rflte_core_txpl_pwr_trking_type;


typedef enum
{
  RFLTE_CORE_TXPL_NS_01 = 1,
  RFLTE_CORE_TXPL_NS_02 = 2,
  RFLTE_CORE_TXPL_NS_03 = 3,
  RFLTE_CORE_TXPL_NS_04 = 4,
  RFLTE_CORE_TXPL_NS_05 = 5,
  RFLTE_CORE_TXPL_NS_06 = 6,
  RFLTE_CORE_TXPL_NS_07 = 7,
  RFLTE_CORE_TXPL_NS_08 = 8,
  RFLTE_CORE_TXPL_NS_09 = 9,
  RFLTE_CORE_TXPL_NS_10 = 10,
  RFLTE_CORE_TXPL_NS_11 = 11,
  RFLTE_CORE_TXPL_NS_12 = 12,
  RFLTE_CORE_TXPL_NS_13 = 13,
  RFLTE_CORE_TXPL_NS_14 = 14,
  RFLTE_CORE_TXPL_NS_15 = 15,
  RFLTE_CORE_TXPL_NS_16 = 16,
  RFLTE_CORE_TXPL_NS_17 = 17,
  RFLTE_CORE_TXPL_NS_18 = 18,
  RFLTE_CORE_TXPL_NS_19 = 19,
  RFLTE_CORE_TXPL_NS_20 = 20,
  RFLTE_CORE_TXPL_NS_21 = 21,
  RFLTE_CORE_TXPL_NS_22 = 22,
  RFLTE_CORE_TXPL_NS_23 = 23,
  RFLTE_CORE_TXPL_NS_24 = 24,
  RFLTE_CORE_TXPL_NS_25 = 25,
  RFLTE_CORE_TXPL_NS_26 = 26,
  RFLTE_CORE_TXPL_NS_27 = 27,
  RFLTE_CORE_TXPL_NS_28 = 28,
  RFLTE_CORE_TXPL_NS_29 = 29,
  RFLTE_CORE_TXPL_NS_30 = 30,
  RFLTE_CORE_TXPL_NS_31 = 31,
  RFLTE_CORE_TXPL_NS_32 = 32,
  RFLTE_CORE_TXPL_NS_INVALID = 0xFF,
} rflte_core_txpl_ns_type;

/* Structure to hold the power backoff info */
typedef struct rflte_core_power_backoff_db10_info
{
/* To hold the slot1 backoff value */
  uint8 slot1_backoffvalue;

/* To hold the slot2 backoff value */
  uint8 slot2_backoffvalue;

/* To hold the final backoff value that will be applied per subframe */
  uint8 backoffvalue;
  
}rflte_core_power_backoff_db10_info_type;

void rflte_core_txpl_ul_power_update_per_sf(uint8 sub_frame, int16 tx_power, uint8 num_rbs);

void rflte_core_txpl_pwr_trking_threshold(uint32 carrier_idx,
                                          boolean mtpl_enabled,
                                          int16 *pwr_trking_threshold);

typedef struct 
{
    uint8 sub_frame;
    int16 tx_power;
    uint8 num_rbs;
    uint8 start_rb_index;
    int32 rx_freq_error[RFLTE_MC_MAX_CELL_IDX_SUPPORTED];
    rflte_core_nlic_wb_nb_type wb_nb;
} nlic_input_params_per_sf;


typedef struct 
{
    boolean nlic_en[3];
    uint8 victim_id;
    rflte_mc_nlic_clock_mode_type clock_mode;
} nlic_return_params_per_sf;

void rflte_core_txpl_ul_power_update_per_sf(uint8 sub_frame, int16 tx_power, uint8 num_rbs);

void rflte_core_txpl_update_nlic_per_sf(nlic_input_params_per_sf *nlic_input_info, nlic_return_params_per_sf *nlic_return_info);

#endif /*FEATURE_LTE*/
#endif /* RFLTE_CORE_TXPL_H */

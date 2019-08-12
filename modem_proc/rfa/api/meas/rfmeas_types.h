#ifndef RFMEAS_TYPES_H
#define RFMEAS_TYPES_H

/*
   @file
   rfm_meas.h

   @brief
   RF Driver's Inter Freq Meas external interface file.

   @details
   
*/

/*===========================================================================
Copyright (c) 2010 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/meas/rfmeas_types.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/20/14   php     ML1-RF interface changes for IRAT to support HORxD/other fixes
10/21/14   sw      Introduce enums for DL paths
09/26/14   gvn     Support for 3DLCA gaps
07/31/14   ndb     Added support for extended EARFCN for LTE
05/13/14   rmb     Additional fields for WCDMA SBDC and SB3C support.
05/02/14   sc      Add subscription ID to rfm_meas_irat_info_type
12/11/13   rmb     Added pretune offset time for home and neighbour.
10/21/13   pl      Adding trigger type
10/15/13   pl      Adding preload type
10/15/13   pl      Update LTE PLL settling time
10/11/13   dw      Add pretune(preload) offset time in rfm_meas_irat_info_type
03/06/13   pl      Add struct tag to help with non-LTE builds
02/05/13   pl      Remove irat buffer from common header as per review comments
02/04/13   pl      Adding missed fields to IRAT common header structure
01/31/13   pl      IRAT common header restructure
01/22/13   pl      Implement code view comments
01/20/13   pl      FTM LTE-CA IRAT support
01/10/13   cj      Added source and target device id to rfm_meas_header_type structure
11/19/12   sc      RF-Only mDSP update shared variables cleanup
11/07/12   sc      Add common iRAT info params structure for types cleanup
10/30/12   sc      Cleanup common member ordering for casting
10/05/12   aa      Added wtow measurement parameter
04/04/12   av      Add is_measurement_gap_aborted flag to rfm_meas_exit_param_type to support Abort sequences.
01/17/12   av      Fix call flow for XtoG
10/21/11   zg      Added rfm_meas_ttog_irat_info_param_type 
09/19/11   dw      Added rfm_meas_ltow_irat_info_param_type
07/26/11   ckl     Added W2W AGC and SSBI time in rfm_meas_wtow_irat_info_param_type 
06/25/11   dw      Change W2W Pll settling time to uint16 type 
05/27/11   dw      Add more IRAT info timing parameters support
04/08/11   sar     Removed condition #def's for CMI-4.
04/05/11   dw      GtoW/LtoW RxlM support
03/08/11   tnt     LM integration for IRAT
02/09/11   jyu     Added Level 1&2 RF FTM IRAT test  
01/21/11   jfc     Include "rfc_card_gsmctl.h" to get defn of mdsp_burst_type
01/18/11   jfc     Featurize include of mdsp_intf.h under FEATURE_GSM
10/07/10   jyu     Added new type for rfm_meas_generic_params_type
09/29/10   av/jyu  Created new API rfm_meas_common_get_irat_info_param  
05/20/10   tws     IRAT for MDM9K.
03/19/10   can     Initial common measurement APIs and support.
03/02/10   lcl     un-commented out startup and cleanup buffers
03/01/10   tws     Initial version
============================================================================*/


#include "rfm_mode_types.h"
#include "rfcom.h"

#include "mdsp_intf.h"
#include "lm_types.h"

// Initial values for startup_time and cleanup_time calculation
#define NUM_OF_MAX_SBI_BEFORE_WRITES 120
#define NUM_OF_MAX_SBI_AFTER_WRITES  60
#define SBI_WRITE_TIME_IN_NS 2200
// GENERIC_PLL_LOCK_TIME_IN_US used for GSM/WCDMA/1x/DO/LTE
#define GENERIC_PLL_LOCK_TIME_IN_US 200
#define LTE_PLL_LOCK_TIME_IN_US  GENERIC_PLL_LOCK_TIME_IN_US
// Values for Neighbour and Home Pretune offset time
#define NEIGHBOUR_PRETUNE_OFFSET_TIME_IN_US 1200
#define HOME_PRETUNE_OFFSET_TIME_IN_US 0

/* Define DL paths for irat testing */
typedef enum
{
  RFM_MEAS_DL_PATH,
  RFM_MEAS_DL_PATH_LTE_CA_1,
#if !defined FEATURE_THOR_MODEM && !defined FEATURE_JOLOKIA_MODEM
  RFM_MEAS_DL_PATH_LTE_CA_2,
#endif
  RFM_MEAS_DL_PATH_MAX
} rfm_meas_src_params;

#define MAX_SOURCE_ITEM RFM_MEAS_DL_PATH_MAX

typedef enum
{
   RFM_MEAS_COMMON_FAILURE,
   RFM_MEAS_COMMON_SUCCESS,

} rfm_meas_result_type;

typedef enum
{
   RFM_MEAS_PROGRAM_STARTUP,
   RFM_MEAS_PROGRAM_BURSTS,
   RFM_MEAS_PROGRAM_CLEANUP,
   RFM_MEAS_PROGRAM_PRELOAD,
   RFM_MEAS_PROGRAM_TRIGGER,
} rfm_meas_program_script_type;

typedef struct 
{
    uint32 start_up_time;
    uint32 clean_up_time;
} rfm_meas_time_const_type;

typedef struct
{
  rfm_device_enum_type device;
  rfm_device_enum_type div_device;
  rfm_device_enum_type div2_device;
  rfm_device_enum_type div3_device;
  boolean div_en;
  uint32 band;
} rfm_meas_param_type;

typedef struct rfm_meas_header_struct
{
   rfcom_mode_enum_type   source_tech;
   uint8                  num_source_item;
   rfm_meas_param_type    source_param[MAX_SOURCE_ITEM];
   rfcom_mode_enum_type   target_tech;
   rfm_meas_param_type    target_param;
} rfm_meas_header_type;

typedef struct 
{
    rfm_meas_header_type header;
    void* source_param;
    void* target_param;
    rfm_meas_program_script_type script_type;
} rfm_meas_setup_param_type;

typedef struct 
{
    rfm_meas_header_type header;
    void* source_param;
    void* target_param;

} rfm_meas_enter_param_type;

typedef struct 
{
    rfm_meas_header_type header;
    void* source_param;
    void* target_param;
    boolean is_measurement_gap_aborted;
} rfm_meas_exit_param_type;




/*! @brief  This structure will be common to all technology  */
typedef struct 
{
   /*! @brief  Serving RAT  */
   rfcom_mode_enum_type  serving_rat;

   /*! @brief  neighbor RAT  */
   rfcom_mode_enum_type  neighbor_rat;
} rfm_meas_cell_infor_type;


/*! @brief  This structure will be used for L<->G and W<->G  */
typedef struct 
{

   rfcom_device_enum_type device; 
   /*!<  Device on which WCDMA will be tuned  */

   /*! @brief  Serving band  */
   rf_card_band_type     serving_band;

   /*! @brief  Serving channel  */
   uint16                serving_chan;

   /*! @brief  neighbor band  */
   rf_card_band_type     neighbor_band;

   /*! @brief  neighbor channel  */
   uint16                neighbor_chan;

   /*! @brief  startup buffer indicator for L<->G and W<->G */
   mdsp_burst_type       startup_buffer;

   /*! @brief cleanup buffer indicator for L<->G and W<->G */
   mdsp_burst_type       cleanup_buffer;

   /*! @brief  The rest of the element only needed for L2G and W2G.  Not applicable for G2L and G2W. */

   /*! @brief  RSSI to set the LNA  */
   dBx16_T               rx_lvl_dbm;

   /*! @brief  Information to be reported back to GL1  */
   rf_gain_T             *rf_gain;

   /*! @brief  Needed for RF driver optimization */
   rf_burst_type         rf_burst;

   /*! @brief power measurement buffer indicator for L->G and W->G */
   mdsp_burst_type       meas_buffer;

   /*! @brief  Serving cell RxLM buffer index  */
   lm_handle_type        serving_rxlm_buf_idx;

   /*! @brief  Neighbor cell RxLM buffer index  */ 
   lm_handle_type        neighbor_rxlm_buf_idx;

} rfm_meas_params_channel_type;


/*! @brief  This structure will be passed to rfm_meas_enter() and rfm_meas_setup() */
typedef struct 
{
   /*! @brief  The number of neighbor cells to be measured--Mainly for L2G and W2G  */
   uint16                   num_neighbor;

   /* Pointer to neighbour cell list */
   rfm_meas_params_channel_type * channel_info;

} rfm_meas_tech_params_type;

typedef struct 
{
    rfm_meas_header_type header;
    uint32 startup_time;
    uint32 cleanup_time;

   /*! @brief Specifies the neighbor pre-tune offset time in us units  */
    uint32 nbr_pretune_offset_time;

   /*! @brief Specifies the home pre-tune offset time in us units  */
    uint32 home_pretune_offset_time;

    void* irat_info_param;
    void* tech_specific_info_param;
    uint8 sub_id;
} rfm_meas_irat_info_type;


typedef struct 
{
   /*! @brief Specifies the start duration for XtoY in Target Tech Units */
   uint16 start_duration;
 
   /*! @brief Power measurement setup time for XtoY in Target Tech Units */
   uint16 power_meas_setup_duration;
 
   /*! @brief Specifies the cleanup duration for XtoY in Serving Tech Units */
   uint16 cleanup_duration;

   /*! @brief XtoY pll settling time only */
   uint16 pll_settling_time;

   /*! @brief XtoY SBI write time */
   uint16 sbi_write_time;

   /*! @brief XtoY AGC settling time */
   uint16 agc_settling_time;

} rfm_meas_xtoy_irat_info_param_type;


typedef struct 
{
   /*! @brief  Specifies the start duration for WtoG=num_sbis*2.4Qs  */
   uint16 start_duration;
 
   /*! @brief  GSM power measurement setup time: num_sbis*2.4+tunetime (QS)  */
   uint16 power_meas_setup_duration;
 
   /*! @brief  WtoG cleanup duration = num_sbi*2.4+pll settling+agc settling */
   uint16 cleanup_duration;

   /*! @brief  WtoG tune back WCDMA pll settling time only (us) */ 
   uint16 w_pll_settling_time;
            
} rfm_meas_wtog_irat_info_param_type;


typedef struct 
{
   /*! @brief  Specifies the start duration for LtoG=num_sbis*2.4Qs  */
   uint16 start_duration;
 
   /*! @brief  GSM power measurement setup time: num_sbis*2.4+tunetime (QS)  */
   uint16 power_meas_setup_duration;
 
   /*! @brief  LtoG cleanup duration = num_sbi*2.4+pll settling*/
   uint16 cleanup_duration;

            
} rfm_meas_ltog_irat_info_param_type;


typedef struct 
{
   /*! @brief  WCDMA pll settling time only (us) */ 
   uint16 w_pll_settling_time;
            
} rfm_meas_gtow_irat_info_param_type;

/*! @brief  This structure will be used for L->W, W->L, DO->L, L->DO, L->1x, L->L, 1x->L  */
typedef struct 
{
   /*! @brief  source tech  */
   rfm_mode_enum_type    src_tech;

   /*! @brief  source band  */
   uint16                src_band;

   /*! @brief  source channel  */
   rfcom_lte_earfcn_type                src_chan;

   /*! @brief  source bandwidth  */
   /*! For technologies except LTE, _bw will be set by default as RFCOM_BW_LTE_1P4MHz*/
   rfcom_lte_bw_type     src_bw;

   /*! @brief  source diversity enable/disable indicator  */
   uint16                src_div_en;

   /*! @brief  source buffer index  */
   uint16                src_buffer_index;
 
   lm_handle_type        src_rxlm_buf_idx_ant0;
   /*!<  LM buffer index for prx*/

   lm_handle_type        src_rxlm_buf_idx_ant1;
   /*!<  LM buffer index for drx*/
 
   /*! @brief  target tech  */
   rfm_mode_enum_type    tgt_tech;

   /*! @brief  target band  */
   uint16                tgt_band;

   /*! @brief  target channel  */
   rfcom_lte_earfcn_type                tgt_chan;

   /*! @brief  target bandwidth  */
   /*! For technologies except LTE, _bw will be set by default as RFCOM_BW_LTE_1P4MHz*/
   rfcom_lte_bw_type     tgt_bw;

   /*! @brief  target diversity enable/disable indicator  */
   uint16                tgt_div_en;

   /*! @brief  target buffer index  */
   uint16                tgt_buffer_index;

   lm_handle_type        tgt_rxlm_buf_idx_ant0;
   /*!<  LM buffer index for prx*/

   lm_handle_type        tgt_rxlm_buf_idx_ant1;
   /*!<  LM buffer index for drx*/

   /*! @brief  test level  */
   uint16                test_level;

   /*! @brief  source tech has a second carrier */
   uint16                src_extra_carrier_flag;

   /*! @brief  source band on second carrier */
   uint16                src_band_carrier_1;

   /*! @brief  source channel on second carrier */
   rfcom_lte_earfcn_type                src_chan_carrier_1;

   /*! @brief  source bandwidth on second carrier */
   /*! For technologies except LTE, _bw will be set by default as RFCOM_BW_LTE_1P4MHz*/
   rfcom_lte_bw_type     src_bw_carrier_1;

   /*! @brief  source diversity enable/disable indicator on second carrier */
   uint16                src_div_en_carrier_1;

   /*!<  LM buffer index for prx on second carrier */
   lm_handle_type        src_rxlm_buf_idx_ant2;
   
   /*!<  LM buffer index for drx on second carrier */
   lm_handle_type        src_rxlm_buf_idx_ant3;
   
   /*!<  Specify SC, DC or 3C mode for first carrier  */
   rfcom_multi_carrier_hspa_id_type carrier_type_carr0;

   /*!<  Specify SC, DC or 3C mode for second carrier  */
   rfcom_multi_carrier_hspa_id_type carrier_type_carr1;
   
   /*! @brief  source channel 1 on first carrier */
   rfcom_lte_earfcn_type src_chan1_carr0;

   /*! @brief  source channel 2 on first carrier */
   rfcom_lte_earfcn_type src_chan2_carr0;
   
   /*! @brief  source channel 1 on second carrier */
   rfcom_lte_earfcn_type src_chan1_carr1;

   /*! @brief  source channel 2 on second carrier */
   rfcom_lte_earfcn_type src_chan2_carr1;

} rfm_meas_generic_params_type;

typedef struct
{
  uint16 pll_settling_time;
  uint16 agc_settling_time;  /* AGC settling Time in usec */
  uint16 ssbi_write_time;    /* ssbi_write_time in usec */
} rfm_meas_wtow_irat_info_param_type;

typedef struct
{
  uint16 pll_settling_time;
  uint16 agc_settling_time;  /* AGC settling Time in usec */
  uint16 ssbi_write_time;    /* ssbi_write_time in usec */
} rfm_meas_ltow_irat_info_param_type;

typedef struct 
{
   /*! @brief  Specifies the start duration for TtoG=num_sbis*2.4Qs  */
   uint16 start_duration;
   /*! @brief  GSM power measurement setup time: num_sbis*2.4+tunetime (QS)  */
   uint16 power_meas_setup_duration;
   /*! @brief  TtoG cleanup duration = num_sbi*2.4+pll settling*/
   uint16 cleanup_duration;       
} rfm_meas_ttog_irat_info_param_type;


typedef struct 
{

     rfcom_device_enum_type device; 
     /*!<  Device on which WCDMA will be tuned  */

     uint16 home_arfcn[RFCOM_MAX_CARRIERS];
     /*!<  DL Channel on which WCDMA will be tuned  */

     uint16 nbr_arfcn;
     /*!<  Neighbor Channel on which WCDMA will be tuned  */

     rfcom_multi_carrier_hspa_id_type carrier_type;
     /*!<  Specify SC or DC mode  */

     uint16 home_buffer_index;
     /*!<  Home RFLM handle for startup/cleanup scripts.*/ 

     uint16 nbr_buffer_index;
     /*!<  Neighbor RFLM handle for startup/cleanup scripts.*/ 


} rfm_meas_w2w_setup_and_build_scripts_param_type;

#endif /* RFMEAS_TYPES_H */


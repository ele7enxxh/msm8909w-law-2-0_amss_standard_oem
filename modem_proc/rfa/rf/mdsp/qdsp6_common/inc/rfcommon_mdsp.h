

#ifndef RFCOMMON_MDSP_H
#define RFCOMMON_MDSP_H

/*! 
  @file
  rfcommon_mdsp.h
 
  @brief
  Thie module defines the MDSP interfaces that are common across all technologies. 
*/

/*==============================================================================

  Copyright (c) 2010 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/mdsp/qdsp6_common/inc/rfcommon_mdsp.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/01/15   vs      Add support for RGI based phase compensation 
02/17/15   cd      TxAGC updates for max power override support
02/10/15   cd      TxAGC updates for max power override support
12/04/14   jmf     [selftest] Update FW processing parameters for VSWR to fix LTE20
11/25/14   jmf     [selftest] New API to program VSWR split capture specific params to FW
11/18/14   pk      Added rfcommon_mdsp_asdiv_check_aborted_event()
11/04/14   dbz     [selftest] Add support for VSWR V2 processing
08/24/14   dbz     [selftest] Support for EVM calculation and TX upsampling config
08/05/14   jmf     [SelfTest] Remove RFCMD FW message to set self-test flag
07/24/14   jmf     [SelfTest] Addition of rfcommon_mdsp_set_selftest_flag for FW
05/07/14   ka      Common tx agc / tx lin fix for fbrx gain state setting
04/21/14   cd      Provide variable ICQ address information to RFLM
04/15/14   cd      FBRx gain state interface update
04/08/14   cd      Support for ICQ address per PA range
03/31/14   cri     Remove deprecated TxOverride commands
03/20/14   cri     Remove deprecated DPD MSGR commands
03/05/14   cd      Support for loading freq comp index and full bias lin size to 
                   RFLM
02/21/14   hdz     Added AsDiv support
12/12/13   kg      Removed rfcommon_mdsp_extract_sapt_iq_cap_data
11/17/13   zhw     Tuner AOL. Check if an event is aborted when FW respond.
11/22/13   cd      Load XPT table info to RFLM
11/22/13   cri     Support for SW allocation of DPD sample capture buffers
11/21/13   cd      Optimization for Tx override linearizer population
11/18/13   cd      Hook-up RFLM Tx AGC override interface
10/16/13   cd      IQ gain offset must be provided to RFLM
10/16/13   cd      Add support for querying IREF LUT index
10/11/13   cd      Support to load front end address configuration
10/02/13   cd      Added API to finalize sequences in RFLM
10/02/13   cd      Load linearizer functionality
10/01/13   ra      Add support to for new CCS event
09/30/13   cd      Check in skeleton for load linearizer
09/30/13   cd      Add support to load common TxAGC event information to RFLM DM
09/10/13   aka     Added rfcommon_iq_efs_save API prototype 
07/30/13   aka     Self test API updates
07/19/13   aro     Featurized ASDIV
07/13/13   aro/pl  Added handling of fw response for ASDiv
07/13/13   aro/pl  Added interface to queue ASDIv script to SMEM
07/13/13   aro/pl  ASDiv Event ID
06/25/13   zhw     1x CL settling time/Tx threshold/time out update
06/07/13   ndb     Added support for Antenna Tuner Closed Loop common module
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/01/13   jmf     Implement xPT IQ Capture SWI change
04/22/13   jmf     Implement xPT processing Enum Type
04/12/13   Saul    [Common] Use RF dispatch for fw responses.
04/10/13   ndb     Code Clean: Removed the unused NikeL function
03/21/13   zhw     Added support for Antenna Tuner Open Loop common module 
03/27/13   vss     add support for getting iq capture
03/18/13   rsr     Fixed compiler warning.
03/14/13   cd      Add common interface for delay value programming calculation
03/11/13   sar     Updated file for APQ, GNSS only target.
03/05/13   cri     Initial check in for data module
02/08/13   cri     Get Tx chain info for Tx override from RFC
12/06/12   jmf     Added API to update SMEM with FW DPD processing params
11/16/12   Saul    Sample Capture. Save XPT DPD data
11/16/12   Saul    Sample Capture. Warning fixes.
11/16/12   Saul    Sample Capture. Use common response.
11/08/12   Saul    Common. Warning fixes.
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
09/21/12   vb      Added API to write tuner script to FW memory 
09/21/12   vb      Added API to convert tuner tech to FW technology 
09/21/12   vb      Added APIs to set and read status flag for tuner script in FW 
10/19/12   Saul    COMMON XPT. Removed maintenance of mask from common data.
09/11/12   zhw     COMMON. Common start_push_and_commit API support for
                   CCS events
08/27/12   Saul    CDMA. Added call to log xpt samples to EFS.
08/02/12   Saul    COMMON. Added support to allocate/write/free DPD tables.
07/23/12   Saul    RF COMMON. MDSP/MSGR framework.
05/16/12   vb      Do DAC cal for a device path only if a tech is enabled for it 
03/07/12   cri     Added initial DPD support
12/28/11   adk     Implemented DAC_1 calibration in the 1x mode
09/25/11   vb      Support for DAC cal using different Irefs
09/08/11   vb      Support for DAC Cal
07/28/10   pl      Remove all FW dependencies
07/01/10   pl      Remove FW dependency from external modules
07/01/10   pl      Added MDSP abstraction for writing IRT script to FW
06/29/10   pl      Initial Revision.

==============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "txlm_intf.h"
#endif
#include "rfdevice_cmn_intf.h"
#include "msgr_umid.h"
#include "rfcommon_mdsp_types.h"
#include "rfcommon_mdsp_event_intf.h"
#include "rf_buffer_intf.h"
#include "rf_hal_buffer.h"
#include "rf_dispatch.h"
#include "rf_dispatch_snum.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_core_txlin_types.h"
#include "rflm_txagc_api.h"

#include "fw_rf_common_intf.h"


#ifdef __cplusplus
extern "C" {
#endif

/*================================== DEFINITIONS =============================*/
/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Max supported size for APT Tx linearizer
*/
#define RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE             RFLM_TXAGC_TX_LIN_APT_SIZE

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Max supported size for EPT Tx linearizer
*/
#define RFCOMMON_MDSP_TXLIN_EPT_TBL_SIZE             RFLM_TXAGC_TX_LIN_EPT_SIZE

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Max supported size for ET Tx linearizer
*/
#define RFCOMMON_MDSP_TXLIN_ET_TBL_SIZE               RFLM_TXAGC_TX_LIN_ET_SIZE

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Max number of elements allowed for non-trigger data in a TxAGC transaction
*/
#define RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE \
                                               RFLM_TXAGC_NON_TRIGGER_DATA_SIZE

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Max number of elements for non-trigger data in a PAPM TxAGC transaction
*/
#define RFCOMMON_MDSP_TXLIN_PAPM_NON_TRIG_DATA_SIZE \
                                         RFLM_TXAGC_QPOET_NON_TRIGGER_DATA_SIZE

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Max number of elements for non-trigger data in WTR TxAGC transaction
*/
#define RFCOMMON_MDSP_TXLIN_WTR_NON_TRIG_DATA_SIZE \
                                          RFLM_TXAGC_WTR_NON_TRIGGER_DATA_SIZE

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Max number of elements allowed for trigger data in a TxAGC transaction
*/
#define RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE                                    1

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    TxAGC event definition for RFLM
*/
#define RFCOMMON_MDSP_CCS_EVENT_TXAGC_ID \
                             ( RFCOMMON_MDSP_EVENT_ID_BASE_RFCMD | 0x00000003 )


/*=============================================================================
                       Common Antenna Tuner Update Module Definitions
=============================================================================*/
/*----------------------------------------------------------------------------*/
/*! Antenna tuner update ccs Event ID.*/
#define RFCOMMON_MDSP_CCS_EVENT_TUNER_UPDATE_ID   ( RFCOMMON_MDSP_EVENT_ID_BASE_RFCMD | 0x00000001 )

/*! Antenna switch diversity ccs Event ID.*/
#define RFCOMMON_MDSP_CCS_EVENT_ASDIV_ID   ( RFCOMMON_MDSP_EVENT_ID_BASE_RFCMD | 0x00000002 )

/*----------------------------------------------------------------------------*/
extern const rfcommon_mdsp_event_settings_type rfcommon_mdsp_event_tuner_update;

/*===========================================================================
                       RF Common Event Settings DEFINITIONS
===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Event types that can be passed into RF COMMON MDSP API callback handlers.

  @details
  Generic event types passed into call back handler when call back
  handler is called during MDSP configuration.
*/
typedef enum
{
  /*! Event indicating general execution failure */
  RFCOMMON_MDSP_EXECUTION_FAILURE = -1,

  /*! Event indicating that the RF-MDSP action is completed successfully */
  RFCOMMON_MDSP_EXECUTION_SUCCESS = 0,
  
} rfcommon_mdsp_cb_event_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF COMMON MDSP interface callback type.

  @details
  Function signature of the callback expected by RF COMMON MDSP
  interface functions.  Callback functions are required to accept
  two parameters, to indiciate what event this callback invocation is
  responding to.
*/
typedef void (*rfcommon_mdsp_cb_handler_type)( rfcommon_mdsp_cb_event_type,
                                              void* );

#define RFCOMMON_MSDP_SNUM_COUNTER_MASK 0xFFFFFF
#define RFCOMMON_MSDP_SNUM_COUNTER_MAX 16777216

/* the VSWR split capture split number */
#define RFCOMMON_MDSP_VSWR_SPLIT_SAMP 2048
#define RFCOMMON_MDSP_VSWR_SPLIT_MARGIN 4096

/*----------------------------------------------------------------------------*/
/*! Macro to extract the technology out of a Sequence number */
#define RFCOMMON_MDSP_SNUM_TECH(snum) ((uint8) ( ( ((uint32)(snum)) >> 24) & 0xFF ) )

/*----------------------------------------------------------------------------*/
/*! Macro to extract the technology out of a Sequence number */
#define RFCOMMON_MDSP_SNUM_COUNT(snum) ((uint32) ( snum & RFCOMMON_MSDP_SNUM_COUNTER_MASK ) )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to store firmware response handler and the opaque data related
  to the handler
*/
typedef struct
{
  rf_dispatch_snum_handler_fp_type rsp_handler;
  /*!< Call back function to be used when RF receives the response back 
  from firmware */

  void *handler_data;
  /*!< Pointer to opaque data to be used by response handler. */

} rfcommon_mdsp_response_cb_param_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold common tx linearizer configuration params
*/
typedef struct
{
  struct
  {
    struct
    {
      uint8 non_trig[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
      /*!< PAPM write non trigger data, array of size: number of RFFE writes */
      uint8 trig[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
      /*!< PAPM write trigger data, array of size: number of RFFE writes */
    }data[RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE];
  }papm_config;

  struct
  {
    union
    {
      uint8 ssbi_rffe[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
      /*!< SSBI/RFFE PA write data */
      uint32 grfc[2];
      /*!< GRFC PA write data */
    }data[RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE];
  }pa_config;

  struct
  {
    struct
    {
      uint8 non_trig[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
      /*!< WTR write non trigger data, array of size: number of RFFE writes */
      uint8 trig[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
      /*!< WTR write trigger data, array of size: number of RFFE writes */
      uint16 iref_index;
      /*!< Iref LUT index */
      uint8 fbrx_gain_state;
      /*!< FBRx gain state */
      int16 phase;
      /*!< Phase value */

      struct 
      {
        uint16 rsb_coeff_alpha  ;
        /*!< RSB alpha correction coefficient from WTR for the MODEM */
        uint16 rsb_coeff_beta   ;
        /*!< RSB beta correction coefficient from WTR for the MODEM */
      } wtr_iq_adj_params ;

        uint16 rgi_val;
      /*!< RGI value corresponding to above data, from calibration */
    } data[RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE];

    uint8 fbrx_non_trig[4];
    /*!< WTR FBRx non trigger data, independent of RGI */
  } wtr_config;

  uint32 iq_gain_offset[RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE];                
  /*!< IQ gain Offset in dB10 */

  int16 add_env_scale_cap[RFCOMMON_MDSP_TXLIN_EPT_TBL_SIZE];
  /*!< Additional Env Scale cap for capped/boosted linearizer entries */

  int16 txpwr_dbm10_fullbias[RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE];  
  /*!< constant biased power table */

  int16 txpwr_dbm10_smpsbias[RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE]; 
  /*!< smps biased power table */

  /* index_to_const_bias_tbl[x] is the row index in txpwr_dbm10_fullbias[] 
  where txpwr_dbm10_smpsbias[x] and 
  txpwr_dbm10_fullbias[index_to_const_bias_tbl[x]] has the same RGI */
  uint8 index_to_const_bias_tbl[RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE];
  /*!< RGI mapping information */

  uint16 fcomp_index[RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE];
  /*!< Frequency compensation index per RGI */

  uint8 full_bias_lin_size;
  /*!< Store number of valid full bias entries for this PA state */

  uint8 calibrated_lin_size;
  /*!< Store number of valid calibrated entries for this PA state */

  rfcommon_mdsp_dpd_table_info_t amam_addr[RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE];
  /*!< Address of AMAM table to load. This address info is provided by 
    load DPD functionality */

  rfcommon_mdsp_dpd_table_info_t ampm_addr[RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE];
  /*!< Address of AMPM table to load. This address info is provided by 
    load DPD functionality */

  rfcommon_mdsp_dpd_table_info_t 
                             detrough_addr[RFCOMMON_MDSP_TXLIN_APT_TBL_SIZE];
  /*!< Address of detrough table to load. This address info is provided by 
    load DPD functionality */

  uint8 max_lin_cap_boost_algo;
  /*!< Provides interface to store into FED the max lin cap/boost algorithm 
    RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_DEFAULT = 0
    RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_BOOST = 1
    RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_QPA_BACKOFF = 2 */

}rfcommon_mdsp_tx_linearizer_config_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold common tx linearizer front end device address configuration
*/
typedef struct
{
  struct
  {
    int8   non_trig_numb_bytes[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
    int8   trigger_num_bytes[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
    uint16 non_trig[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
    uint16 trig[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
    uint16 dummy;
    /*!< PAPM write address */
  }papm_config_addr;

  struct
  {
    int8   non_trig_numb_bytes[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
    int8   trigger_num_bytes[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
    uint16 non_trig[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
    uint16 trig[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
    uint16 dummy;
    /*!< PA write address */
  }pa_config_addr[RFM_PA_GAIN_STATE_NUM];

  struct
  {
    int8   non_trig_numb_bytes[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
    int8   trigger_num_bytes[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
    uint16 non_trig[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
    uint16 trig[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
    uint16 dummy;
    /*!< WTR write address */
  }wtr_config_addr;

  struct
  {
    int8   non_trig_numb_bytes[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
    int8   trigger_num_bytes[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
    uint16 non_trig[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
    uint16 trig[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
    uint16 dummy;
    /*!< fbrx write address */
  }fbrx_config_addr;

  struct
  {
    int8   non_trig_numb_bytes[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
    int8   trigger_num_bytes[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
    uint16 non_trig[RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE];
    uint16 trig[RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE];
    uint16 dummy;
    /*!< WXE write address */
  }wxe_config_addr;

  /* Flag indicating whether PA ICQ address is fixed or not */
  boolean pa_icq_fixed_addr;

}rfcommon_mdsp_tx_front_addr_config_type;

typedef struct
{
  //! @brief
  // Forward correlated Rx power (dB x 100).
  //
  int16 fwd_pwr;

  //! @brief
  // Forward correlated Rx phase (degrees x 100).
  //
  int16 fwd_phase;

  //! @brief
  // Reflected correlated Rx power (dB x 100).
  //
  int16 refl_pwr;

  //! @brief
  // Forward correlated Rx phase (degrees x 100).
  //
  int16 refl_phase;

  //! @brief
  // Forward path delay (relative to search center) in sample/2^10 units.
  //
  int32 fwd_delay;

  //! @brief
  // Reflected Rx DC I.
  //
  int32 refl_rx_dc_i;

  //! @brief
  // Reflected Rx DC Q.
  //
  int32 refl_rx_dc_q;

  //! @brief
  // Reflected path delay (relative to search center) in sample/2^10 units.
  //
  int32 refl_delay;

  //! @brief
  // Reflected Tx RMS magnitude (x 100).
  //
  uint32 refl_tx_rms_mag;

  //! @brief
  // Reflected Rx RMS magnitude (after DC cancellation.)
  //
  uint32 refl_rx_rms_mag;

  //! @brief
  // Reflected correlation gain (dB x 100).
  //
  int16 refl_corr_gain;

}rfcommon_mdsp_selftest_vswr_meas_type;

typedef struct
{
  //! @brief
  // this many samples will be skipped around the split center
  //
  uint16 vswr_split_margin;

}rfcommon_mdsp_self_test_vswr_params_type;

/*----------------------------------------------------------------------------*/
boolean rfcommon_mdsp_dac_cal_enter( void );

/*----------------------------------------------------------------------------*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
boolean rfcommon_mdsp_extract_dac_cal_results( txlm_chain_type txlm_chain,
                                               rfdevice_dac_iref_val iref_val,
                                               uint8 iref_count );
#endif

/*----------------------------------------------------------------------------*/
boolean rfcommon_mdsp_dac_cal_exit( boolean dac_cal_status );

/*----------------------------------------------------------------------------*/
void rfcommon_mdsp_clear_dac_cal_results( void );

/*----------------------------------------------------------------------------*/
void rfcommon_mdsp_ant_tuner_spi_init(void);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_mdsp_init
(
  void
);

/*----------------------------------------------------------------------------*/
uint32
rfcommon_mdsp_get_sequence_number
(
  msgr_umid_type msgr_umid
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_allocate_dpd_buffer
(
   void
);

/*----------------------------------------------------------------------------*/
void
rfcommon_mdsp_free_dpd_buffer
(
   void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_event_start_push_and_commit
(
  uint32 event_id,
  rfcommon_mdsp_event_handle** event_handle,
  const rfcommon_mdsp_event_settings_type* event_settings,
  rfcommon_mdsp_transaction_settings_type* transaction_params,
  void * shared_mem_buf_ptr,
  uint32 shared_mem_max_size_bytes,
  rf_buffer_intf *buf_obj_ptr,
  boolean force_new_task
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_ccs_cleanup_event
(
  rfcommon_mdsp_event_handle** event_handle
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_ccs_abort_event
(
  rfcommon_mdsp_event_handle** event_handle
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_setup_capture_params
( 
  uint8 tx_chain,
  uint8 rx_chain,
  uint8 rx_lm_handle,
  uint8 capture_type,
  boolean rx_compact,
  uint16 num_samples,
  uint16 split_samp, /* Not used currently, keep for future mods */
  rfcommon_xpt_processing_type do_processing
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_setup_dpd_proc_params
( 
  int16 search_center,
  uint8 search_width,
  uint8 first_bin,
  uint8 extrap_bin,
  uint8 min_bin_count,
  uint8 min_first_bin_count,
  uint8 min_last_bin_count,
  uint8 rx_spec_inv,
  rfcommon_xpt_processing_type dpd_construct_flag
);

boolean
rfcommon_mdsp_fetch_selftest_iq_data
( 
   uint8 buffer_index,
   uint32 num_samples,
   int32 **rx_sample_buffer,
   int32 **tx_sample_buffer
);

boolean
rfcommon_mdsp_fetch_selftest_meas
(
uint8 buffer_index,
uint16 *evm,
uint32 *rx_rms_mag
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_save_xpt_iq_data
( 
   uint8 buffer_index,
   uint32 num_samples,
   char* iq_file_name
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_et_path_delay
( 
   uint8 buffer_index,
   int32 * et_path_delay
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_save_xpt_dpd_data
( 
   uint8 buffer_index,
   char* dpd_file_name
);

/*----------------------------------------------------------------------------*/
//#ifdef FEATURE_RF_HAS_XPT_SUPPORT
fw_dpd_cal_result_t *
rfcommon_mdsp_get_xpt_dpd_data
( 
   uint8 buffer_index
);
//#endif

/*----------------------------------------------------------------------------*/
int32 rfcommon_mdsp_convert_ns_et_delay
( 
  int32 delay_ns_10
);
/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_get_xpt_iq_data
( 
   uint8 buffer_index,
   uint32 num_samples,
   uint32 *tx_samples,
   uint32 *rx_samples
);

/*----------------------------------------------------------------------------*/
void
rfcommon_mdsp_txagc_override_fwrsp_handler
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void rfcommon_mdsp_tuner_update_fwrsp_handler
( 
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/

void rfcommon_mdsp_tuner_cl_meas_update_fwrsp_handler
( 
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_tuner_check_aborted_update
(
   uint16 tuner_tech,
   uint16 tuner_chain
);

/*----------------------------------------------------------------------------*/
boolean rfcommon_mdsp_tuner_update_queue_script
(
  uint16 tuner_tech,
  uint16 tuner_chain,
  rf_buffer_intf *tuner_script,
  rfcommon_mdsp_event_handle** tuner_event_handle_ptr,
  rf_dispatch_snum_handler_fp_type fwrsp_callback,
  void *tuner_cb_data
);

/*----------------------------------------------------------------------------*/
boolean rfcommon_mdsp_tuner_update_cl_meas_script
(
  uint16 tuner_tech,
  uint16 tuner_chain,
  rf_buffer_intf *tuner_script,
  rfcommon_mdsp_event_handle** tuner_event_handle_ptr,
  rf_dispatch_snum_handler_fp_type fwrsp_callback,
  void *tuner_cb_data,
  uint32 time_out_value_ms,
  int32 tx_agc_threshold,
  uint32 settling_time_us
);

/*----------------------------------------------------------------------------*/

uint16 rfcommon_mdsp_tuner_get_cl_meas_results 
(
  rfcommon_mdsp_event_handle* tuner_event_handle_ptr,
  rf_hal_bus_rffe_type *tuner_script,
  uint16 max_size
);

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
void 
rfcommon_mdsp_asdiv_fwrsp_handler
( 
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_asdiv_queue_script
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rfm_mode,
  rf_buffer_intf *asdiv_script,
  rfcommon_mdsp_event_handle** asdiv_event_handle,
  rf_dispatch_snum_handler_fp_type fwrsp_callback,
  void* asdiv_cb_data,
  uint32 rxlm_handle
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_asdiv_check_aborted_event
(
  void* update_buffer_ptr
);

#endif /* FEATURE_RF_ASDIV */

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_txagc_config_event_start
(
  rfm_mode_enum_type rfm_mode,
  uint32 event_id,
  int32 txlm_handle_id,
  uint32 script_buffer_id,
  rfcommon_mdsp_event_handle** event_handle
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_txagc_config_event_push
(
  uint32 event_id,
  int32 txlm_handle_id,
  uint32 script_buffer_id,
  uint16 *total_num_tx_agc_tasks,
  rfcommon_core_txlin_agc_sequence_type task_type,
  rfcommon_mdsp_event_handle** event_handle,
  rf_buffer_intf *buf_obj_ptr,
  boolean force_new_task
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_txagc_config_event_commit
(
  uint32 event_id,
  rfcommon_mdsp_event_handle** event_handle
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_txagc_config_finalize_seq
(
  int32 txlm_handle_id
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_txagc_config_event_update_timing
(
  uint32 event_id,
  int32 txlm_handle_id,
  uint32 script_buffer_id,
  int32 *trig_times,
  uint16 num_txagc_tasks_pushed,
  rfcommon_mdsp_event_handle** event_handle
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_txagc_config_static_linearizer
(
  int32 txlm_handle_id,
  rfm_pa_gain_state_type pa_state,
  uint8 num_lin_entries,
  rfcommon_mdsp_tx_linearizer_config_type *tx_lin_data
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_txagc_config_dynamic_linearizer
(
  int32 txlm_handle_id,
  rfm_pa_gain_state_type pa_state,
  uint8 num_lin_entries,
  rfcommon_mdsp_tx_linearizer_config_type *tx_lin_data,
  boolean skip_pout_comp
);

/*----------------------------------------------------------------------------*/
void rfcommon_iq_efs_save(int32 *iq_buf_tx, int32 *iq_buf_rx, int32 num_samples);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_txagc_config_front_end_addr
(
  int32 txlm_handle_id,
  rfm_pa_gain_state_type pa_state,
  rfcommon_mdsp_tx_front_addr_config_type *tx_front_end_addr
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_txagc_config_dummy_front_end_addr
(
  int32 txlm_handle_id,
  rfm_pa_gain_state_type pa_state,
  rfcommon_mdsp_tx_front_addr_config_type *tx_front_end_addr
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_mdsp_xpt_txagc_override_v2
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcommon_core_xpt_tx_override_type* tx_ovr_params
);

boolean rfcommon_mdsp_calc_xpt_cap_rx_dc
(
   uint8 cap_buf_id, 
   uint16 num_iq_samples, 
   uint16 discard_samps, 
   int32 * dc_val_i, 
   int32 * dc_val_q
);

boolean
rfcommon_mdsp_setup_self_test_params
( 
  boolean freq_adj_comp_en,
  uint8 rx_bw_filt_len,
  int8 rx_bw_filt_qfactor,
  int16 *rx_bw_filt_coeff,
  uint8 eq_filt_mask
);

boolean
rfcommon_mdsp_setup_self_test_upsampling_params
( 
  uint8 tx_upsamp,
  uint8 tx_up_fir_len,
  int8 tx_up_fir_shift,
  int16 *tx_up_fir
);

boolean
rfcommon_mdsp_selftest_vswr_meas
(
   uint8 buffer_index,
   rfcommon_mdsp_selftest_vswr_meas_type * vswr_meas
);

boolean
rfcommon_mdsp_setup_self_test_vswr_params
( 
  rfcommon_mdsp_self_test_vswr_params_type * vswr_params
);
#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_MDSP_H */


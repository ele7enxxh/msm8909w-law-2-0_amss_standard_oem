#ifndef RFCOMMON_MC_H
#define RFCOMMON_MC_H

/*!
   @file
   rfcommon_mc.h

   @brief

   @details
   
*/

/*===========================================================================
Copyright (c) 2008 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/inc/rfcommon_mc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/06/15   ag      Adding support for enabling/disabling band capability from bc config
08/05/14   jmf     [SelfTest] Remove RFCMD FW message to set self-test flag
07/24/14   jmf     [SelfTest] Addition of rfcommon_mc_set_selftest_flag for FW
03/31/14   cri     Remove deprecated TxOverride commands
03/20/14   cri     Remove deprecated DPD MSGR commands
11/18/13   cri     Support for SW allocation of DPD sample capture buffers
11/07/13   yb/ndb  Added the support for dynamic tune code size
11/14/13   cd      Support for RFLM-based Tx AGC override
07/30/13   aka     Self test API updates
06/16/13   zhw     Tuner AOL call flow update
06/14/13   ndb     Define the new call back tuner data for CL operation
06/16/13   zhw     Tuner AOL call flow update
06/10/13   ndb     Added rfcommon_mc_tuner_update_cl_meas_response_handler()
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/20/13   spa/zhw Move script pointer out of tuner callback data
05/02/13   vb      Tuner CA changes 
03/21/13   zhw     Added support for Antenna Tuner Open Loop common module 
03/19/20   aro     Added Data module hookup
03/11/13   sar     Updated file for APQ, GNSS only target.
11/19/12   aki     Added rfcommon_mc_deinit function
11/16/12   Saul    Sample Capture. Save XPT DPD data
11/16/12   Saul    Sample Capture. Warning fixes.
11/16/12   Saul    Sample Capture. Use common response.
11/13/12   Saul    Common XPT. Reverted changes made to commit_dpd_data.
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
10/19/12   Saul    COMMON. Warning Fix.
10/19/12   Saul    COMMON XPT. Separated allocate and write DPD into two APIs.
08/02/12   Saul    COMMON. Added support to allocate/write/free DPD tables.
07/23/12   Saul    RF COMMON. MDSP/MSGR framework.
08/12/11   jhe     Added support for TDS BC config
03/21/11   sty     Added C++ protection
02/23/11   av      Disable GP_CLK once again 
02/17/11   av      Reverting GP_CLK changes
02/02/11   av      Disable GP_CLK
01/15/11   aro     Removed direct access to rfm_mode[] variable
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
06/30/10    ap     Added support for Power Collapse & Early Clock Enable
06/28/10   can     Removing Power collapse changes that cause a crash.
06/24/10   can     Support for LTE BC Config.
06/22/10    ap     Added support for Power Collapse
05/21/10   lcl     Back out clock changes for GSM to work.
04/13/10   adk     Merged branch scmm_mdm9k_merge_final
03/01/10   kma     Added power collapse status API
01/29/10   kma     Removed uncessary vote
01/28/10   kma     Removed power collapse API from RFM_COMMON
01/28/10   lcl     Merge MDM9K changes
01/26/10   sr      Added doxygen comments
01/21/10   sr      Added RFCOMMON_MC_GET_RX_WARMUP_TIME
11/30/09   bmg     Changed init function return type to boolean
10/27/09   sr      Added extern variables
10/16/09   jhe     Added _power_clock_on/off
09/16/09   bmg     Added rfm_mode externalization
10/14/08   sr      Initial version.

============================================================================*/

#include "rfa_variation.h"
#include "task.h"
#include "rfcom.h"
#include "pm.h"
#include "rf_fwrsp_task.h"
#include "rfcommon_mdsp_types.h"
#include "rf_dispatch_snum.h"
#include "rfcommon_atuner_intf.h"
#include "rfcommon_mdsp.h"
#include "rfm.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "txlm_intf.h"
#endif
#include "rfcommon_data.h"
 
#ifdef __cplusplus
extern "C" {
#endif

/*!
  @brief
  Max size of each tune code value. 

  This is the max size (in bytes)of each tune code value that is currently supported.
*/
#define RFCOMMON_MC_MAX_TUNE_CODE_SIZE 8

/*----------------------------------------------------------------------------*/

typedef enum 
{
  RFCOMMON_IS_SLEEPING,
  RFCOMMON_GET_BAND_MASK,
  RFCOMMON_MC_GET_NUM_ANTENNA,
  RFCOMMON_MC_IS_TUNABLE_ANTENNA,
  RFCOMMON_MC_GET_ANTENNA_DIFF,
  RFCOMMON_MC_ADC_READ,
  RFCOMMON_MC_GET_RX_WARMUP_TIME,   /*! returns the worst-case time needed 
                                        for HW to be configured and ready 
                                        to do Rx */
  RFCOMMON_MC_MAX_CMD
} rfcommon_mc_cmd_type;

/*----------------------------------------------------------------------------*/

typedef struct {
  int32 antenna_index;
  rf_card_band_type band;
} rfcommon_antenna_type;

/*----------------------------------------------------------------------------*/

typedef struct {
  rf_card_band_type band;
  uint16 chan;
  int32 ant_x; 
  int32 ant_ref;
} rfcommon_antenna_diff_type;

/*----------------------------------------------------------------------------*/

#ifdef FEATURE_CDMA1X
typedef struct {
  rfcom_adc_logical_channel_type adc_addr;
  uint16 *adc_val;
} adc_read_type;
#endif /*FEATURE_CDMA1X*/

typedef struct
{
  /*!< Pointer to antenna tuner manager device object */
  rfcommon_atuner_intf *ant_tuner;

  /*!< Pointer to tuner script token object*/
  script_token_type tuner_token;

}rfcommon_mc_tuner_device_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold antenna tuner code update call back data
*/
typedef struct
{
  /*!< Pointer to CCS event handle for tuner CCS event*/
  rfcommon_mdsp_event_handle* tuner_ccs_event_handle;

  /*!< An array that holds tuner_dev pointer and token for each chain*/
  rfcommon_mc_tuner_device_data_type tuner_data[RF_TUNER_CHAIN_NUM];

  /*!< RF tech registered for this update*/
  uint16 tuner_tech;

  /*!< Tuner chain registered for this update*/
  uint16 tuner_chain;

} rfcommon_mc_tuner_cb_data_type;


typedef struct
{
  /*!< Pointer to rl object */
  void *rl_tuner;

  /*!< tuner current state token*/
  script_token_type tuner_token;

  /*!< scenario in use*/
  uint8 scenario_in_use;

  /*!< nv tuner code  in use*/
  uint8 nv_tune_code_in_use[RFCOMMON_MC_MAX_TUNE_CODE_SIZE]; 

  /*!< size of each tune code*/
  uint8 tune_code_size;

}rfcommon_mc_cl_tuner_device_data_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold antenna cl tuner 
*/
typedef struct
{
  /*!< Pointer to CCS event handle for CL tuner CCS event*/
  rfcommon_mdsp_event_handle* tuner_ccs_event_handle;

  /*!< An array that holds cl measurement object and its associated  info for each chain*/
  rfcommon_mc_cl_tuner_device_data_type tuner_data[RF_TUNER_CHAIN_NUM];

} rfcommon_mc_cl_tuner_cb_data_type;

/*----------------------------------------------------------------------------*/

extern void rfcommon_mc_deinit(void);

/*----------------------------------------------------------------------------*/

extern boolean rfcommon_mc_init(rfcom_device_enum_type device, 
                                rex_tcb_type *caller_tcb_ptr,
                                rex_sigs_type task_nv_wait_sig,
                                void (*task_wait_func_ptr)( rex_sigs_type ));

/*----------------------------------------------------------------------------*/

extern int32 rfcommon_mc_cmd_dispatch(rfcom_device_enum_type device, int32 cmd, 
                                      void *data);

/*----------------------------------------------------------------------------*/

extern uint64 rfcommon_mc_get_band_mask(rfcom_device_enum_type device );
extern uint64 rfcommon_mc_get_lte_band_mask(void);
extern uint64 rfcommon_mc_get_tds_band_mask(rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_mc_reload_nv(rfcom_device_enum_type device, 
                                     rex_tcb_type *caller_tcb_ptr,
                                     rex_sigs_type task_nv_wait_sig,
                                     void (*task_wait_func_ptr)( rex_sigs_type ));


/*----------------------------------------------------------------------------*/
boolean
rfcommon_mc_allocate_dpd_buffer
(
   void
);

/*----------------------------------------------------------------------------*/
void
rfcommon_mc_free_dpd_buffer
(
   void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mc_xpt_txagc_override
(
   rfcom_device_enum_type device,
   rfcommon_mdsp_txagc_oride_params_type* oride_data
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_mc_xpt_txagc_override_v2
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  rfcommon_core_xpt_tx_override_type* tx_ovr_params
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_xpt_is_capture_done
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_xpt_wait_for_capture_done
(
  uint32 time_interval_us,
  uint32 timeout_us,
  uint8 *buffer_index
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_xpt_is_processing_done
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_xpt_wait_for_processing_done
(
  uint32 time_interval_us,
  uint32 timeout_us,
  uint8 *buffer_index
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_xpt_set_capture_in_progress
(
   void
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_xpt_set_processing_in_progress
(
   void
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_xpt_store_iq_capture_data
(
  rfcommon_xpt_cal_iq_capture_data_type *iq_cap_data
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_xpt_fetch_iq_capture_data
(
  rfcommon_xpt_cal_iq_capture_data_type *iq_cap_data
);

/*----------------------------------------------------------------------------*/
void
rfcommon_mc_handle_capture_done_response
(
  rf_fwrsp_cmd_type* req_ptr
);

/*----------------------------------------------------------------------------*/
void
rfcommon_mc_handle_processing_done_response
(
  rf_fwrsp_cmd_type* req_ptr
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mc_save_xpt_iq_data
( 
   uint8 buffer_index,
   uint32 num_samples,
   char* iq_file_name
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mc_et_path_delay
( 
   uint8 buffer_index,
   int32 * et_path_delay
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mc_fetch_selftest_iq_data
( 
   uint8 buffer_index,
   uint32 num_samples,
  int32 **rx_sample_buffer,
  int32 **tx_sample_buffer
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mc_fetch_selftest_meas
( 
   uint8 buffer_index,
   uint16 *evm,
   uint32 *rx_rms_mag
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mc_save_xpt_dpd_data
( 
   uint8 buffer_index,
   char* dpd_file_name
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mc_tuner_update_response_handler
( 
  void *cmd_ptr,
  const rf_dispatch_snum_info_type *cb_data
);

/*----------------------------------------------------------------------------*/
void
rfcommon_mc_tuner_update_cleanup
( 
  void *registered_cb_data
);

/*----------------------------------------------------------------------------*/
void
rfcommon_mc_cl_tuner_update_cleanup
( 
  void *registered_cb_data
);
/*----------------------------------------------------------------------------*/
void 
rfcommon_mc_config_trm_band_support
(
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_MC_H */



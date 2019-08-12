#ifndef FTMCOMMON_FBRX_H
#define FTMCOMMON_FBRX_H
/*!
  @file
  ftm_fcommon_fbrx.h

  @brief
  This module contains interface of common FTM FBRx.
  
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_fbrx.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/08/14    ka     Replace rfc with device api for retrieving device pointer
09/08/14   jmf     [SelfTest] Implement Tx Power measurement using TxAGC loop
08/26/14    aa     Added support for FBRX QRCT logging
08/11/14    aa     Update run continuous cmd to control fbrx enable/disable
06/12/14    ka     Add enable parameter to run continuous cmd
05/26/14    ka     Return results from single trigger cmd
04/07/14    ka     Update single trigger. Remove obsolete code.
03/20/14    ka     Added support for storing captures during cal
02/13/14    ka     Added continuous run api, updated capture params
02/06/14    aa     Added api to set ftm cal state in fbrx dm
01/30/14    aa     Added API to get valid Tx handle for FBRx
01/22/14    ka     Added fbrx set params API
12/23/13    ka     Added API for calibration
11/25/13    ka     Added store results. Updated response packet.
11/13/13    ka     Added FTM fbrx common test framework
09/23/13    ka     Initial version

==============================================================================*/

#include "comdef.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "rfcommon_fbrx_api.h"
#include "rfcommon_mdsp_types.h"
#include "ftm_log.h"
#include "math.h"

#define FTM_COMMON_FBRX_RET_MAG_LSE(lse_r, lse_i) ( abs(lse_r)>abs(lse_i) ? abs(lse_r) : abs(lse_i))

/*----------------------------------------------------------------------------*/
/*! Structure used to hold FTM_FBRX_ENTER_MODE input parameters */
typedef PACK(struct)
{
  rfm_mode_enum_type tech;
} ftm_fbrx_enter_mode_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to hold FTM_FBRX_INIT_TX input parameters */
typedef PACK(struct)
{
  rfcommon_fbrx_tx_cfg_type cfg;
} ftm_fbrx_init_tx_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to hold FTM_FBRX_ENABLE input parameters */
typedef PACK(struct)
{
  int8 reserved;
} ftm_fbrx_enable_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to hold FTM_FBRX_EXIT input parameters */
typedef PACK(struct)
{
  rfm_mode_enum_type tech;
} ftm_fbrx_exit_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to hold FTM_FBRX_ACTIVATE_CHAIN input parameters */
typedef PACK(struct)
{
  int8 reserved;
} ftm_fbrx_activate_chain_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to hold FTM_FBRX_BRIDGE_CONFIG input parameters */
typedef PACK(struct)
{
  boolean enable;
} ftm_fbrx_bridge_config_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to hold FTM_FBRX_SINGLE_TRIGGER input parameters */
typedef PACK(struct)
{
  uint8  tx_device;
  uint8  fbrx_gain_state;
  uint8  ubias;
  uint16 scale_rx;
} ftm_fbrx_single_trigger_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to hold FTM_FBRX_RUN_CONTINUOUS input parameters */
typedef PACK(struct)
{
  boolean enable;
} ftm_fbrx_run_control_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to hold FTM_FBRX_ENABLE_RF input parameters */
typedef PACK(struct)
{
  boolean enable;
  uint8   gain_state;
} ftm_fbrx_enable_rf_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to hold FTM_FBRX_STORE_RESULTS input parameters */
typedef PACK(struct)
{
  uint32 num_tx;
  uint32 num_rx;
  uint8  num_buffers;
} ftm_fbrx_store_results_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to hold output parameters */
typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
  uint32  handle_id;
} ftm_fbrx_handle_response_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to hold Single Trigger output parameters */
typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
  int16   gain;
  uint32  ls_err;
  uint16  xcorr_log[RFLM_FBRX_XCORR_LOG_SIZE];
} ftm_fbrx_single_trigger_response_type;

/*----------------------------------------------------------------------------*/
/*! ftm_common_fbrx_capture input parameters */
typedef struct
{
  uint8 tx_device_id;
  rfcommon_mdsp_xpt_mode_enum_type xpt_mode;
  /* Parameter used for equaling (1tau, tau) TA interpolator gain droop */
  int16 gain_comp;
  uint8 fbrx_gain_state;
  uint8   ubias;
  uint16  scale_rx;
  boolean store_capture_data;
  rfm_mode_enum_type tech;
  rfcom_band_type_u band;
} ftm_common_fbrx_capture_input_type;

/*----------------------------------------------------------------------------*/
/*! ftm_common_fbrx_capture output parameters */
typedef struct
{
  rfcommon_fbrx_error_type status;
  int16          gain;
  uint32         loft; /* complex */
  uint32         rsb;  /* complex */
  int16          gain_imbalance;
  int16          phase_imbalance;
  uint32 ls_err;
  uint16 xcorr_log[RFLM_FBRX_XCORR_LOG_SIZE];
} ftm_common_fbrx_capture_result_type;

/*----------------------------------------------------------------------------*/
/*! This type specifies the data packet for the FBRX results log */
typedef PACK(struct)
{
 uint8  version;       /*!< Log packet version. As additional paramters are added the version number can increase */
 int16  txagc;         /*!< Current TxAGC value in dB10 */
 uint8  pa;            /*!< flag indicating the PA State */ 
 uint8  gs;            /*!< flag indicating the fbrx gain state (0,1,2 or 3) */
 int16  gn_est;        /*!< Current Gain estimate result */
 int16  gn_err;        /*!< The FBRx Gain Error in dB10 uint for current txagc */
 int16  mag_lse;       /*!< magnitude of the least squared error estimate result */
 int16  temp_comp;     /*!< Temp Comp value for current tech/band/chan/mode/gs */
 uint16 ubias;         /*!< Updated ubias value for current tech/band/chan/mode/gs */
 uint16 rxscale;       /*!< Updated Rx Scale value for current tech/band/chan/mode/gs */
 uint16 txscale;       /*!< Updated Tx Scale value for current tech/band/chan/mode/gs */
 int16  cal_pwr;       /*!< FBRx Cal Power for current gain state */
 int16  cal_gn;        /*!< FBRx Expected Cal Gain value */
 uint8  tech;          /*!< rflm tech id ( 0=1X, 1=HDR, 2=LTE, 3=WCDMA, 4=TDSCDMA, 5=GSM ) */
 uint8  band;          /*!< flag indicating the current Band of type (rfcom_band_type_u) */ 
 uint8  bandwidth;     /*!< rfcommon_fbrx_tx_cfg_type tech bandwidth */
 uint8  fbrx_mode;     /*!< flag indicating the fbrx mode (0,1,2 or 3) */
 uint8  slot;          /*!< flag indicating the slot index (0,1, or 2) per SF for current fbrx run */ 
 uint8  nv;            /*!< flag indicating the FBRX SYS common NV control value for current tech */ 
} ftm_common_fbrx_log_results_data_type;

/*----------------------------------------------------------------------------*/
/*! This type specifies the header/data packet for the FBRX results log */
typedef PACK(struct)
{
  ftm_log_gen_type      header;
  ftm_common_fbrx_log_results_data_type data;
} ftm_common_fbrx_log_results_t;

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif


/*----------------------------------------------------------------------------*/
extern ftm_rsp_pkt_type ftm_common_fbrx_enter_mode(ftm_common_pkt_type *cmd_ptr);

/*----------------------------------------------------------------------------*/
extern ftm_rsp_pkt_type ftm_common_fbrx_init_tx(ftm_common_pkt_type *cmd_ptr);

/*----------------------------------------------------------------------------*/
extern ftm_rsp_pkt_type ftm_common_fbrx_enable(ftm_common_pkt_type *cmd_ptr);

/*----------------------------------------------------------------------------*/
extern ftm_rsp_pkt_type ftm_common_fbrx_exit(ftm_common_pkt_type *cmd_ptr);

/*----------------------------------------------------------------------------*/
extern ftm_rsp_pkt_type ftm_common_fbrx_activate_chain(ftm_common_pkt_type *cmd_ptr);

/*----------------------------------------------------------------------------*/
extern ftm_rsp_pkt_type ftm_common_fbrx_bridge_config(ftm_common_pkt_type *cmd_ptr);

/*----------------------------------------------------------------------------*/
extern ftm_rsp_pkt_type ftm_common_fbrx_single_trigger(ftm_common_pkt_type *cmd_ptr);

/*----------------------------------------------------------------------------*/
extern ftm_rsp_pkt_type ftm_common_fbrx_run_control(ftm_common_pkt_type *cmd_ptr);

/*----------------------------------------------------------------------------*/
extern ftm_rsp_pkt_type ftm_common_fbrx_enable_rf(ftm_common_pkt_type *cmd_ptr);

/*----------------------------------------------------------------------------*/
extern ftm_rsp_pkt_type ftm_common_fbrx_store_results(ftm_common_pkt_type *cmd_ptr);

/*----------------------------------------------------------------------------*/
extern ftm_common_fbrx_capture_result_type ftm_common_fbrx_capture_and_process
(
  ftm_common_fbrx_capture_input_type input
);

/*----------------------------------------------------------------------------*/
extern void ftm_common_fbrx_lm_test( void );

/*----------------------------------------------------------------------------*/
extern uint32 ftm_common_fbrx_get_valid_tx_handle(void);

/*----------------------------------------------------------------------------*/
extern void ftm_common_fbrx_set_cal_state( boolean ftm_cal_mode );

/*----------------------------------------------------------------------------*/
extern uint8 ftm_common_fbrx_get_mode( void );

/*----------------------------------------------------------------------------*/
extern void ftm_common_fbrx_log_results_init_func( void);

/*----------------------------------------------------------------------------*/
extern void ftm_common_fbrx_log_results_exit_func( void);

/*----------------------------------------------------------------------------*/
extern void ftm_common_fbrx_log_results_cb(int4 interval);

/*----------------------------------------------------------------------------*/
extern boolean ftm_common_fbrx_meas_ant_pwr( 
   int16 * pwr_est_db10, 
   int16 * pwr_est_db100, 
   uint8 num_averages, 
   uint8 * fbrx_gs );

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /* FTMCOMMON_FBRX_H */

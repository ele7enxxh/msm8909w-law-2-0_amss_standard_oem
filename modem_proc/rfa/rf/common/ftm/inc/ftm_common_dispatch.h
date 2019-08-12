#ifndef FTM_COMMON_DISPATCH_H
#define FTM_COMMON_DISPATCH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    F T M  C O M M O N  D I S P A T C H

GENERAL DESCRIPTION
  This is the header file for the embedded FTM Commands that are common
  between 1x and UMTS.


Copyright (c) 2005 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_dispatch.h#1 $ $DateTime:

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
11/03/14   jps     Add support for GET_STATIC_INFO
07/08/14   kg      CL Optimization
10/29/13   cvd     New FTM commands for DTR-Rx 
08/05/13   ars/rmb New FTM command to exit currently selected FTM RF mode. 
07/30/13   aka     Self test API updates
07/12/13   aca     RL cmd defn update
07/08/13   aca     RL Logging
06/12/13   aca     RL FTM API v2
06/12/13   aca     RL FTM API
05/01/13   aki     Added support for RX WAIT V2
03/21/13  yzw      Add FTM common cmd to query RFFE device information {mfg_id, prd_id, rev_id} 
11/15/12   dj      Added new xo calibration command FTM_XO_CALIBRATION
                   Removed support for legacy XO Cal commands 
07/10/12   kai     Add support for Qtuner FTM command
05/16/12   sa      Added FTM_GET_ENH_XO_THERM Command Support
05/08/12   aki     Status field to correct place in ftm_rx_wait_dbm_response_type,
                   mainline FTM_RX_WAIT_DBM code
03/18/12   aro     Created main common function to dispatch radio commands
                   to specific techs
03/18/12   aro     Created common radio dispatch
01/28/12   sar     Feature reduction changes.
11/29/11   aki     Added support for FTM_RX_WAIT_DBM message structures
11/21/11  tnt      Support for Ant Tuner Overwrite cmd thru FTM
05/16/11   vb      Added support for XO Cal V2
10/29/10   ka      Updates for initial WCDMA APT functionality
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1
06/15/09   ns      XO calibration changes for CDMA SCMM bringup
02-09-09   anr     Added includes for XO Calibration
08-19-05   ka      Created
===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_RF_HAS_QTUNER 
#include "rfdevice_antenna_tuner_intf.h"
#endif /* FEATURE_RF_HAS_QTUNER */


#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmdiag.h"
#include "ftm.h"

#include "rfc_class.h"
#include "rfcommon_nv.h"

typedef struct
{
  int16   min_power;
  uint16  resolution;
  uint16  dynamic_range;
} ftm_agc_response_type;

typedef  union
{
  uint16 error_code;
  ftm_agc_response_type agc_result;
} ftm_common_result_type;

#if defined (FEATURE_XO_FACTORY_CAL)
// Enh thermistor reading type (enh xo therm read)
typedef enum
{
    FTM_ENH_XO_THERM_READ_XO,
    FTM_ENH_XO_THERM_READ_PMIC,
    FTM_ENH_XO_THERM_READ_PA,
    FTM_ENH_XO_THERM_MAX
}ftm_enh_xo_therm_read_type;
#endif

//Load Linearizer Cal Request Packet
#define MAX_PA_RANGES               MAX_APT_NUM_PA_RANGES
#define MAX_TX_LIN_PWR_LIST_SIZE  FTM_MAX_APT_PWR_LIST_SIZE
#define MAX_CHAN_LIST_SIZE          MAX_APT_CHAN_LIST_SIZE

//Define max num of devices per band for common targets
#define RFC_COMMON_MAX_DEVICE_INSTANCES_PER_BAND   5

typedef struct {
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  uint2   ref_chan;
  uint1   pa_ranges[MAX_PA_RANGES];
  uint1   num_pwr_list[MAX_PA_RANGES];
  uint1   tx_lin_pdm_list[MAX_PA_RANGES][MAX_TX_LIN_PWR_LIST_SIZE];
  uint2   tx_lin_pwr_list[MAX_PA_RANGES][MAX_TX_LIN_PWR_LIST_SIZE];
} ftm_load_tx_lin_cal_data_request;

typedef struct {
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  uint1   pa_ranges[MAX_PA_RANGES];
  uint2   chan_list[MAX_CHAN_LIST_SIZE];
  uint1   tx_fcomp_pdm_list[MAX_PA_RANGES];
  uint2   tx_fcomp_pwr_list[MAX_PA_RANGES][MAX_CHAN_LIST_SIZE];
  uint1   num_freqcomp_cal_chan;
} ftm_load_tx_fcomp_cal_data_request;

typedef struct {
 uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   ftm_error_code;
} ftm_load_tx_cal_data_response;

typedef PACK(struct) {
  uint8   core_id;
  uint8   cs_n;
  uint8   length;
  uint32  codeword[4];
} ftm_ant_tuner_overwrite_type;

typedef enum
{
    ENH_TX_CAL_ACTION_GST,
    ENH_TX_CAL_ACTION_APT,
    ENH_TX_CAL_ACTION_MAX
}enh_cal_action_type;

typedef enum
{
	RFFE_DEVICE_PA = 1,	
	RFFE_DEVICE_ASM,
	RFFE_DEVICE_PAPM
}rffe_device_enum_type;

typedef enum
{
   GET_PA_SWPT,
   GET_STATIC_INFO_MAX
}ftm_get_static_info_action_type;

typedef PACK(enum) {
    FTM_RX_WAIT_DBM_SUCCESS                 = 0,
    FTM_RX_WAIT_DBM_TIMEOUT                 = 1,
    FTM_RX_WAIT_DBM_UNSUPPORTED_TECHNOLOGY  = 2,
    FTM_RX_WAIT_DBM_RSSI_FAILED             = 3,
    FTM_RX_WAIT_DBM_MAX_ERROR               = 0xFFFF
} ftm_rx_wait_dbm_status_type;

typedef PACK(struct) {
  uint2 rf_tech;
  int2  rx_power_threshold;
  uint4 timeout;
} ftm_rx_wait_dbm_request_payload_type;

typedef PACK(struct) {
  uint1 cmd;
  uint1 sub_sys;
  uint2 mode;
  uint2 ftm_cmd;
  uint2 req_len;
  uint2 rsp_len;
  uint2 status;
  uint2 rf_tech;
  int2  rx_power;
} ftm_rx_wait_dbm_response_type;

typedef PACK(struct){
  uint16 rf_tech;
  uint16 device_id;
  uint16 signal_direction;
  int16  rx_power_threshold;
  uint32 timeout;
} ftm_rx_wait_dbm_v2_request_payload_type;

typedef PACK(struct) {
  uint8  cmd;
  uint8  sub_sys;
  uint16 mode;
  uint16 ftm_cmd;
  uint16 req_len;
  uint16 rsp_len;
  uint16 status;
  int16  rx_power;
} ftm_rx_wait_dbm_v2_response_type;

typedef PACK(struct){
	uint1 cmd;
	uint1 sub_sys;
	uint2 mode;
	uint2 ftm_cmd;
	uint8 rf_mode;
	uint8 rfm_device;
	rffe_device_enum_type rffe_device_type;
}ftm_get_rffe_device_info_request;

typedef PACK(struct) {
  uint1 cmd;
  uint1 sub_sys;
  uint2 mode;
  uint2 ftm_cmd;
  uint2 req_len;
  uint2 rsp_len;
  uint2 status;
} ftm_rf_mode_exit_response_type;

typedef PACK(struct){
	uint1 cmd;
	uint1 sub_sys;
	uint2 mode;
	uint2 ftm_cmd;
	uint16 status;
    uint16 mfg_id[RFC_MAX_DEVICE_INSTANCES_PER_BAND];
	uint16 prd_id[RFC_MAX_DEVICE_INSTANCES_PER_BAND];
	uint16 rev_id[RFC_MAX_DEVICE_INSTANCES_PER_BAND];
}ftm_get_rffe_device_info_response;

typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command ID */

  uint8 sub_sys;
  /*!< Sub-system ID. Ex - 11 for FTM */

  uint16 mode;
  /*!< RF Mode ID. Ex-28 for SVDO Cmd, FTM_1X_CAL_V2_C */

  uint16 ftm_cmd;
  /*!< FTM Sub command. Example, FTM_1X_CAL_DVGA */

  uint16 req_len;
  /*!< Request Length */

  uint16 rsp_len;
  /*!< Response Length */

  uint32 handle;
  /*!< LM Handle */
} ftm_rxlm_allocate_buffer_rsp_type;

typedef PACK(struct){
    uint8 tech;
}ftm_rxlm_allocate_buffer_params;

typedef PACK(struct){
    uint8 handle;
}ftm_rxlm_deallocate_buffer_params;

typedef PACK(struct){
    //cfg
    uint8  num_carr;
    uint8 bandwidth;
    uint8 mode;
    //cfg
}ftm_rxlm_cfg;

typedef PACK(struct){
    uint8 handle;
    ftm_rxlm_cfg cfg;
}ftm_rxlm_update_static_settings_params;

typedef PACK(struct){
    uint8 handle;
}ftm_rxlm_update_dynamic_settings_params;

typedef PACK(struct){
    uint8 rxchn_idx_0;
    uint8 adca_idx_0;
    uint8 wbw_idx_0;
    uint8 nbn_idx_0;
    uint8 nbn_idx_1;
    uint8 nbn_idx_2;
    uint8 nbn_idx_3;
}ftm_rxlm_indices_params;

typedef PACK(struct){
    uint8 handle;
    ftm_rxlm_update_dynamic_settings_params indices;
//  uint8 config_mask;
}ftm_rxlm_configure_chain_params;

typedef PACK(struct){
    uint32 tech;
    uint32 nb0;
    uint32 nb1;
    uint32 nb2;
    uint32 nb3;
}ftm_rxlm_configure_rxfe_bridge_params;

typedef PACK(struct){
    uint8 adc;
    uint8 wb;
    uint8 nb0;
    uint8 nb1;
    uint8 nb2;
    uint8 nb3;
}ftm_rxlm_stream_data_params;

#if 0
typedef struct
{
   uint8 device;
}ftm_get_pa_swpts_req_payload_type;

typedef struct
{
   int16 active_pa_states;
   int16 pa_rise_swpt[RFCOMMON_NV_MAX_PA_STATE];
   int16 pa_fall_swpt[RFCOMMON_NV_MAX_PA_STATE];
}ftm_get_pa_swpts_rsp_payload_type;

typedef struct
{
   ftm_get_pa_swpts_req_payload_type get_pa_swpt_req_payload;

}ftm_get_static_info_req_payload_type;

typedef struct
{
   ftm_get_pa_swpts_rsp_payload_type get_pa_swpt_rsp_payload;

}ftm_get_static_info_res_payload_type;
#endif

typedef PACK(struct){
  uint8 rf_tech;
  uint8 action;
  uint16 band;
  uint16 cmd_params;
}ftm_get_static_info_request_pkt_type;

typedef PACK(struct){
  uint8 rf_tech;
  uint8 action;
  uint16 band;
  uint8 tx_device;
}ftm_get_pa_swpt_request_pkt_type;

typedef PACK(struct) {
  uint8  cmd;
  uint8  sub_sys;
  uint16 mode;
  uint16 ftm_cmd;
  uint16 req_len;
  uint16 rsp_len;
  uint8 status;
  uint8 rf_tech;
  uint8 action;
  uint8 tx_device;
  uint16 band;
  int16 active_pa_states;
  int16  pa_rise_swpt[RFCOMMON_NV_MAX_PA_STATE];
  int16  pa_fall_swpt[RFCOMMON_NV_MAX_PA_STATE];
}ftm_get_pa_swpt_response_pkt_type;


#ifdef FEATURE_RF_HAS_QTUNER
// qtuner path type
typedef enum
{
    FTM_QFE_QTUNER_PATH0,
    FTM_QFE_QTUNER_PATH1,
    //FTM_QFE_QTUNER_PATH2,
    //FTM_QFE_QTUNER_PATH3,
    FTM_QFE_QTUNER_PATH_MAX
}ftm_qfe_qtuner_path_type;
#define MAX_TUNER_PID 6
typedef struct 
{  
  uint8 hdet_pid;    
  uint8 gain_state;
}ftm_set_tuner_gain_type;

typedef struct 
{
  uint8 num_hdet_pid;
  ftm_set_tuner_gain_type gain_params[MAX_TUNER_PID];  
}ftm_set_tuner_gain_req_type;

typedef struct 
{
  uint8 num_hdet_pid;
  uint8 num_avg;
  uint16 wait_time;
  uint8 hdet_pid[MAX_TUNER_PID];  
}ftm_get_tuner_power_req_type;
typedef struct 
{
  uint8 hdet_pid;    
  uint16 power;  
}ftm_get_tuner_power_type;

typedef struct 
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
  uint8 num_hdet_pid;
  ftm_get_tuner_power_type power_params[MAX_TUNER_PID];  
}ftm_get_tuner_power_resp_type;

/* RL */

typedef enum
{
  FTM_QFE_QTUNER_SETUP_RL,
  FTM_QFE_QTUNER_READ_RL,
  FTM_QFE_QTUNER_ENABLE_RL_LOG,
  FTM_QFE_QTUNER_DISABLE_RL_LOG,
  FTM_QFE_QTUNER_INVALID_CMD
}ftm_get_tuner_rl_cmd_enum;

typedef struct 
{
  /* Cmd */
  uint8 rl_cmd;
  
  /* Averages */
  uint8 num_average;
  
  /* Delay */
  uint16 delay_us;  
  
  
  /* Gain control */  
  uint8 incident_gain;
  uint8 reflected_gain;
  uint8 pass_through_gain;  

  /* TIA gain control- current not used */
  uint8 rl_gain;
  
}ftm_get_tuner_rl_req_type;

typedef struct 
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;

  uint8 rl_cmd;  
  
  /* Measurements in raw ADC counts */
  uint16 pass_through_raw;
  uint16 reflected_raw;
  uint16 incident_raw;
  
  /* Measurements in dBm */
  uint16 pass_through_dbm10;
  uint16 reflected_dbm10;
  uint16 incident_dbm10;

  /* RL , ML Measurements */
  uint16 return_loss_db10;
  float return_loss_raw;
  uint16 mismatched_loss_db10;
  
}ftm_get_tuner_rl_resp_type;

#endif /* FEATURE_RF_HAS_QTUNER */

/*===========================================================================

FUNCTION FTM_COMMON_DISPATCH

DESCRIPTION
   This function handles requests to run common tests and other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_common_dispatch(ftm_common_pkt_type *cmd_ptr);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_radio_dispatch
(
  void  * ftm_req_data
);

#endif  // FEATURE_FACTORY_TESTMODE
#endif  // FTM_COMMON_DISPATCH_H

/*!
  @file
  fw_rf_common_intf.h

  @brief
  Generic interface for common RF interface

*/

/*===========================================================================

  Copyright (c) 2011-14 Qualcomm Technologies Incorporated. All Rights Reserved

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

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/--/14   DG      Add SW specified Tx decimation and upsampling.
08/--/14   DG      Support sample processing of cache aligned Tx captures.
10/25/13   DG      Add Rx RMS capture -- faster Rx RMS measurement.
07/22/13   DG      Add EVM capture -- compute EVM from RefMode=1 (Tx I,Q).
06/14/13   BW	   Add settling_time for CL measurement.
05/22/13   BW      Add ASD command intf
04/29/13   DG      SWI changes for ET delay, FBRx cal -- SWI only, no code.
04/29/13   DG      SWI+API changes to separate sample capture and processing.
04/18/13   BW      Add QTF enum to support UMTS DBDC
02/01/13   BW      Add QTF antenna tuner intf
01/30/13   DG      Move RF cal sample buffers to overlay memory.
01/22/13   TZ      Remove DAC Cal struct from shared memory
12/05/12   NB      Interface for RSB calibration
11/07/12   NB      Updated DPD interface for multiple sample capture 
10/18/12   NB      XPT calibration updates for Dime
03/01/12   NG      Updates for SAPT
07/18/11   NB      Updated DAC calibration interface
05/06/11   NB      Increased buffer size (added word in header) file is 
                   backword compatible 
04/27/11   NB      Initial version
===========================================================================*/
#ifndef FW_RF_COMMON_INTF_H
#define FW_RF_COMMON_INTF_H

/* Include memory map file for interface location */
#include "modem_fw_memmap.h"
#include "ccs_rf_intf.h"
#include "fw_dpd_intf.h"
#include "rf_event_intf.h"
#include "fw_autopin_intf.h"

/*! @brief Bridge ID of AHB mempool bridge that RF SW should use to 
 * directly read out samples from mempool */
#define FW_RF_COMMON_SW_MEMPL_RD_AHB_BRDG_ID 1

/*! @brief size of debug log buffer. This buffer is used for logging 
  debug data or IQ data. The data is packed in 32 bit format. The
  Q_size of the actual data copied is provided in the header.

  GSM overlays a structure on log buffer. */
#define FW_RF_COMMON_DEBUG_BUF_SIZE  (18*1024)

/*! @brief Number of common captures */
#define FW_RF_COMMON_NUM_CAPTURES  4

/*! @brief Maximum number of sample buffers for DPD */
#define FW_DPD_MAX_SAMP_BUF 12

/*! @brief Maximum number of result buffers for DPD */
#define FW_DPD_MAX_RESULTS  12

/*! @brief Max number of CCS tasks in the QTF event */
#define FW_QTF_MAX_NUM_CCS_TASKS 4

/*! @brief Max number of RFFE' addr/ data pairs in the QTF event */
#define FW_QTF_MAX_NUM_OF_RFFE   30

/*! @brief Max nmber of RFFE_EXT tasks in the QTF event */
#define FW_QTF_MAX_NUM_OF_RFFE_EXT   10   

/*! @brief Maximum size of QTF script buffer */
#define FW_RF_QTF_SCRIPT_DATA_BUF_SIZE_BYTE (FW_QTF_MAX_NUM_OF_RFFE*2 + \
                                             FW_QTF_MAX_NUM_OF_RFFE_EXT*8)

#define FW_RF_QTF_SCRIPT_DATA_BUF_SIZE_WORDS ((FW_RF_QTF_SCRIPT_DATA_BUF_SIZE_BYTE + 3)>>2)

/*! @brief Max number of CCS tasks in the ASD event */
#define FW_ASD_MAX_NUM_CCS_TASKS 16

/*! @brief Maximum size of ASD script buffer size in word */
#define FW_RF_ASD_SCRIPT_DATA_BUF_SIZE_WORDS 48

/*! @brief Common Tech buffer enumeration.
    @details SW uses the enum to identify the common interface buffer associated with each Tech */
//XXX Later combine the QTF tech enum with this common enum
typedef enum
{
  FW_RF_TECH_BUF_ID_GSM,
  FW_RF_TECH_BUF_ID_GSM_G0 =
    FW_RF_TECH_BUF_ID_GSM,
  FW_RF_TECH_BUF_ID_GSM_G1,
  FW_RF_TECH_BUF_ID_1X,
  FW_RF_TECH_BUF_ID_DO,
  FW_RF_TECH_BUF_ID_UMTS,
  FW_RF_TECH_BUF_ID_UMTS_C0 =
    FW_RF_TECH_BUF_ID_UMTS,
  FW_RF_TECH_BUF_ID_UMTS_C1,
  FW_RF_TECH_BUF_ID_LTE,
  FW_RF_TECH_BUF_ID_LTE_C0 = 
    FW_RF_TECH_BUF_ID_LTE,
  FW_RF_TECH_BUF_ID_LTE_C1,
  FW_RF_TECH_BUF_ID_TDSCDMA,
  FW_RF_NUM_TECHS
} fw_rf_tech_type;

typedef enum
{
  /* Result is invalid */
  FW_RF_COMMON_RESULT_INVALID = 0,
  /* Result cannot be updated (some error) */
  FW_RF_COMMON_RESULT_FAIL,
  /* Result copy in progress */
  FW_RF_COMMON_RESULT_IN_PROGRESS,
  /* Result copy completed successfully */
  FW_RF_COMMON_RESULT_SUCC
} fw_rf_common_result_e;

/*! @brief Control structure for debug logging */
typedef struct 
{
  /*! @brief Number of items */
  uint32 length;
  /*! @brief Number of 32 bit words */
  uint32 size32;
  /*! @brief Index of first used sample (number of samples to skip) */
  uint8 first_samp_idx;
  /*! @brief item bit width */
  uint8 q_size;
  /*! @brief storage bit width */
  uint8 mem_q_size;
  /*! @brief Whether capture was success or not */
  fw_rf_common_result_e result;
} fw_rf_common_header_s;

/*! @brief Structure to capture debug samples or IQ samples logged 
    or SAPT samples*/
typedef struct 
{
  /*! @brief header for logging results */
  fw_rf_common_header_s rf_header;
  /*! @brief Data buffer */
  uint32 data[FW_RF_COMMON_DEBUG_BUF_SIZE] ALIGN(32);
} fw_rf_common_log_buffer_s;

/*! @brief Structure for DPD sample Tx capture */
typedef struct
{
  /*! @brief Header information */
  fw_rf_common_header_s header;
  /*! @brief data buffer */
  uint32 data[FW_DPD_MAX_TX_CAPTURE_SIZE32] ALIGN(32);
} fw_rf_dpd_tx_samp_buffer_s;

/*! @brief Structure for DPD sample Rx capture */
typedef struct
{
  /*! @brief Header information */
  fw_rf_common_header_s header;
  /*! @brief data buffer */
  uint32 data[FW_DPD_MAX_RX_CAPTURE_SIZE32] ALIGN(32);
} fw_rf_dpd_rx_samp_buffer_s;

/*! @brief Structure that contains captured RX and TX samples */
typedef struct
{
  /*! @brief TX samples */
  fw_rf_dpd_tx_samp_buffer_s tx;
  /*! @brief RX samples */
  fw_rf_dpd_rx_samp_buffer_s rx;
} fw_rf_dpd_samp_buffer_s;

/*! @brief DAC calibration results are of size 128 */
#define FW_RF_COMMON_DAC_CAL_BUF_SIZE  128

/*! @brief sample capture types */
typedef enum
{
  /*! @brief construct DPD */
  FW_RF_CAP_DPD,
  /*! @brief ET path delay (split capture) */
  FW_RF_CAP_ET_PATH_DELAY,
  /*! @brief feedback Rx (gain/phase imbalance, LO feed through, Tx gain */
  FW_RF_CAP_FBRX,
  /*! @brief EVM measurement */
  FW_RF_CAP_EVM,
  /*! @brief Rx RMS measurement */
  FW_RF_CAP_RX_RMS,
  /*! @brief VSWR measurement */
  FW_RF_CAP_VSWR,
  /*! @brief GSM power measurement */
  FW_RF_CAP_GSM_PWR_MES // AI: rename
} fw_rf_capture_type;

/*! @brief Structure for common sample capture parameters */
typedef struct
{
  /*! @brief Number of samples */
  uint16 num_samp;
  /*! @brief Capture type */
  fw_rf_capture_type capture_type;
  /*! @brief Number of samples before split capture config change,
      0 = no split capture */
  uint16 split_samp;
  /*! @brief Tx chain */
  uint8 tx_chain;
  /*! @brief Rx chain */
  uint8 rx_chain;
  /*! @brief Buffer for RxLM configuration */
  uint8 rxlm_buf_id;
  /*! @brief Flag to compressed Rx samples.
      0=no, 20 bit samples are expanded to 32 bits, upper 12 bits are ignored
      1=yes, 20 bit samples have no padding between samples */
  uint8 rx_compact;
  /*! @brief Tx decimation ratio -- 0,1 disables */
  uint8 tx_decimate;
  /*! @brief Tx upsample ratio -- 0,1 disables */
  uint8 tx_upsamp;
  /*! @brief Flag to enable FW processing */
  uint8 do_processing;
} fw_rf_capture_config_t; // in RF common SWI, shared for all captures

/*! @brief Structure for parameters common to all tech sample capture commands.
    The structure is included in each tech sample capture command. */
typedef struct
{
  /*! @brief Message Router tag for response */
  uint32 seq_num;
  /*! @brief Sample buffer index */
  uint32 samp_idx : 8;
  /*! @brief Result buffer index */
  uint32 result_idx : 8;
  /*! @brief Flag indicating first capture in sequence */
  uint32 first_trig : 1;
  /*! @brief Flag indicating last capture in sequence */
  uint32 last_trig : 1;
} fw_rf_capture_param_t;

/*! @brief Common FW/RF interface for SPI*/
typedef struct
{
  /*! @brief 1 = clk to slave is always on*/
  uint8 clock_always_on; 
  /*! @brief 8n, n = 1..4*/     
  uint8 num_bits_per_trans;
  /*! @brief clk cycles to deassert CS between transaction*/    
  uint8 deassert_wait_time;   
  /*! @brief 1= cs active hi, 0 = cs active low*/ 
  uint8 cs_active_hi;         
  /*! @brief 8GSBI core id*/ 
  uint8 core_index;              
  /*! @brief reserved byte*/ 
  uint8 reserved_byte;        
  /*! @brief unit = KHz*/ 
  uint16 clock_freq_khz;      
} fw_rf_spi_ant_tuner_protocol_para_type_s; 

/*! @brief Structure containing header and tasks for any event */
typedef struct 
{
  /*! @brief Header of event  */
  rf_ccs_event_description_buffer_header_type header;

  /*! @brief Tasks for the event */
  rf_ccs_event_task_description_type task_buf[FW_QTF_MAX_NUM_CCS_TASKS];
} fw_rf_qtf_ant_tuner_event_header_task_type;

/*! @brief Data structure for passing QTF event based script to FWs. */
typedef struct
{
  /*! @brief Header and tasks for the QTF ant tuner event */
  fw_rf_qtf_ant_tuner_event_header_task_type header_task;
  /*! @brief Data buffer for the tasks  */
  uint32 data_buf[FW_RF_QTF_SCRIPT_DATA_BUF_SIZE_WORDS];
} ALIGN(4) fw_rf_qtf_ant_tuner_event_infor_type;

/*! @brief QTF tuner information for close loop update */
typedef struct
{
  /*! @brief Close Loop TX AGC power threshold */
  int32 cl_tx_agc_threshold;
  /*! @brief Close Loop event timeout value. It is absolute time set by SW 
      (current time + timeout delta time). Tech FW need to do a wraparound
      comparison with half range of the USTMR counter */
  uint32 cl_timeout;
  /*! @brief Settling time for the length of the CL measurement */
  uint32 cl_settling_time;
} fw_rf_qtf_ant_tuner_cl_info_type;

/*! @brief QTF tuner status information enum. */
typedef enum
{
  /*! @brief This value indicates that the antenna chain has been disabled 
     before commiting the pending QTF transaction in FW */
  FW_RF_QTF_ANT_TUNER_UPDATE_ABORTED,
  /*! @brief This value indicates an outstanding QTF AOL update in FW */
  FW_RF_QTF_ANT_TUNER_UPDATE_PENDING,
  /*! @brief This value indicates that FW has commited the event for AOL */
  FW_RF_QTF_ANT_TUNER_UPDATE_COMMITTED,
  /*! @brief This value indicates an outstanding QTF CL update in FW */
  FW_RF_QTF_ANT_TUNER_UPDATE_CL_PENDING,
  /*! @brief This value indicates that Close Loop event has been occurred */
  FW_RF_QTF_ANT_TUNER_UPDATE_CL_OCCURRED,
  /*! @brief This value indicates that Close Loop event timeout */
  FW_RF_QTF_ANT_TUNER_UPDATE_CL_TIMEOUT
} fw_rf_qtf_ant_tuner_status_type; 

/*! @brief QTF tuner ID enum. */
typedef enum
{
  FW_RF_QTF_ANT_TUNER_PRIMARY,
  FW_RF_QTF_ANT_TUNER_SECONDARY,
  FW_RF_QTF_ANT_TUNER_NUM_PER_TECH
} fw_rf_qtf_ant_tuner_buf_type; 

/*! @brief QTF tuner information Data structure per antenna.
    @details SW updates this interface and checks the SSBI commit status flag before issuing
    next update. So there is no need for double buffering. */
typedef struct
{
  /*! @brief QTF AOL tuner update status. 
     0) SW sets this to FW_RF_QTF_ANT_TUNER_UPDATE_ABORTED to disable the antenna chain update
        before committing the pending QTF transaction in FW
     1) SW sets this to FW_RF_QTF_ANT_TUNER_UPDATE_PENDING after updating the structure
     2) When the event are committed, FW sets the flag to FW_RF_QTF_ANT_TUNER_UPDATE_COMMITTED
     3) SW sets this to FW_RF_QTF_ANT_TUNER_UPDATE_CL_PENDING after updating the structure
     4) When Close Loop event occurred, FW sets the flag to FW_RF_QTF_ANT_TUNER_UPDATE_CL_OCCURRED
     5) When Close Loop event timeout and did not occur, FW sets the flag to
     FW_RF_QTF_ANT_TUNER_UPDATE_CL_TIMEOUT */
  fw_rf_qtf_ant_tuner_status_type event_commit_status;
  fw_rf_qtf_ant_tuner_event_infor_type qtf_tuner_script;
  uint32 token;
  int32 scratch_mem; /*!< Scratch memory for FW to use per QTF event */
  /*! @brief QTF tuner close loop update info structure */
  fw_rf_qtf_ant_tuner_cl_info_type qtf_tuner_cl_info;
} fw_rf_qtf_ant_tuner_info_type; 

/*! @brief QTF tuner Tech buffer enumeration.
    @details SW uses the enum to identify the QTF interface buffer associated with a Tech */
typedef enum
{
  FW_RF_QTF_ANT_TUNER_BUF_ID_GSM,
  FW_RF_QTF_ANT_TUNER_BUF_ID_GSM_G0 =
    FW_RF_QTF_ANT_TUNER_BUF_ID_GSM,
  FW_RF_QTF_ANT_TUNER_BUF_ID_GSM_G1,
  FW_RF_QTF_ANT_TUNER_BUF_ID_1X,
  FW_RF_QTF_ANT_TUNER_BUF_ID_DO,
  FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS,
  FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS_C0 =
    FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS,
  FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS_C1,
  FW_RF_QTF_ANT_TUNER_BUF_ID_LTE,
  FW_RF_QTF_ANT_TUNER_BUF_ID_LTE_C0 = 
    FW_RF_QTF_ANT_TUNER_BUF_ID_LTE,
  FW_RF_QTF_ANT_TUNER_BUF_ID_LTE_C1,
  FW_RF_QTF_ANT_TUNER_BUF_ID_TDSCDMA,
  FW_RF_QTF_ANT_TUNER_NUM_TECHS
} fw_rf_qtf_ant_tuner_tech_type;

/*! @brief QTF tuner Interface buffer.
    @details QTF interface for all techs and antenna chains. */
typedef struct
{
  fw_rf_qtf_ant_tuner_info_type qtf_tuner_config[FW_RF_QTF_ANT_TUNER_NUM_TECHS][FW_RF_QTF_ANT_TUNER_NUM_PER_TECH];
  fw_rf_qtf_ant_tuner_info_type qtf_tuner_cl_config[FW_RF_QTF_ANT_TUNER_NUM_TECHS][FW_RF_QTF_ANT_TUNER_NUM_PER_TECH];
} fw_rf_qtf_ant_tuner_intf_type;

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
/*! @brief Structure containing header and tasks for ASD event */
typedef struct 
{
  /*! @brief Header of event  */
  rf_ccs_event_description_buffer_header_type header;

  /*! @brief Tasks for the event */
  rf_ccs_event_task_description_type task_buf[FW_ASD_MAX_NUM_CCS_TASKS];
} fw_rf_asd_event_header_task_type;

/*! @brief Data structure for passing ASD event based script to FWs. */
typedef struct
{
  /*! @brief Header and tasks for the ASD event */
  fw_rf_asd_event_header_task_type header_task;
  /*! @brief Data buffer for the tasks  */
  uint32 data_buf[FW_RF_ASD_SCRIPT_DATA_BUF_SIZE_WORDS];
} ALIGN(4) fw_rf_asd_event_infor_type;

/*! @brief ASD script status information enum. */
typedef enum
{
  /*! @brief This value indicates SW abort the script and no action from FW */
  FW_RF_ASD_CMD_ABORTED,
  /*! @brief This value indicates an outstanding QTF AOL update in FW */
  FW_RF_ASD_CMD_PENDING,
  /*! @brief This value indicates that FW has commited the event for AOL */
  FW_RF_ASD_CMD_COMMITTED
} fw_rf_asd_status_type; 

/*! @brief Antenna Switch Diversity Data structure.
    @details SW updates this interface and checks the ASD script status
    flag before issuing the next ASD command. */
typedef struct
{
  /*! @brief ASD script status. 
     0) SW sets this to FW_RF_ASD_CMD_ABORTED to disable the ASD command
     1) SW sets this to FW_RF_ASD_CMD_PENDING after updating the structure
     2) When the event are committed, FW sets the flag to FW_RF_ASD_CMD_COMMITTED. */
  fw_rf_asd_status_type asd_script_status;
  fw_rf_asd_event_infor_type asd_script;
  uint32 token;
} fw_rf_asd_info_type; 

/*! @brief Antenna Swtich Diversity Interface buffer.
    @details ASD interface for all techs. */
typedef struct
{
  fw_rf_asd_info_type asd_config[FW_RF_NUM_TECHS];
} fw_rf_asd_intf_type;
#endif
/*! @brief Common FW/RF interface. This contains 
  a) Debug log buffer
  b) XPT results
  c) DAC calibration results
  d) Antenna tuner SPI configuration.
  e) QTF antenna tuner common FW intf
  f) Antenna switch diversity intf
 */
typedef struct
{
  union {
    /*! @brief For legacy copy */
    fw_rf_common_log_buffer_s rf_log_buffer;
    /*! @brief Make an array for generic copies */
    fw_rf_common_log_buffer_s rf_common_log_buf[FW_RF_COMMON_NUM_CAPTURES];
  };
  /*! @brief Common parameters for sample capture */
  fw_rf_capture_config_t capture_config;
  /*! @brief Common paramenters for DPD calibration */
  fw_dpd_cal_config_t dpd_cal_config;
  /*! @brief XPT calibration results */
  fw_dpd_cal_result_t dpd_cal_result[FW_DPD_MAX_RESULTS];
  /*! @brief Common Ant tuner parameters using SPI (legacy) 
    XXX -- needs to be removed */
  fw_rf_spi_ant_tuner_protocol_para_type_s spi_config;
  /*! @brief Common FW interface for antenna tuner */
  fw_rf_qtf_ant_tuner_intf_type qtf_tuner_intf;
  /*! @brief Common FW interface for antenna switch diversity */
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  fw_rf_asd_intf_type asd_intf;
#endif
  /*! @brief Autopin processing function pointer */
  fw_autopin_proc fw_autopin_fn;

} fw_rf_common_intf_s;

/*! @brief Common FW/RF calibration sample buffers. The sample buffers for
    different types of calibration are overlayed with each other then
    overlayed with crash dump memory.
 */
typedef struct
{
  union
  {
    /*! @brief Make an array for generic copies */
    fw_rf_common_log_buffer_s rf_common_log_buf[FW_RF_COMMON_NUM_CAPTURES];
    /*! @brief Sample captures for DPD calibration */
    fw_rf_dpd_samp_buffer_s dpd_samp_buffer[FW_DPD_MAX_SAMP_BUF];
  };
} fw_rf_calibration_intf_s;

#endif // FW_RF_COMMON_INTF_H

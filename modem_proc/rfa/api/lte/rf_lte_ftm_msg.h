/*!
  @file
  rf_lte_ftm_msg.h

  @brief
  Message router UMIDs header for RF LTE FTM subsystem. This file defines
  interfaces for the RF LTE FTM Calibration commands to be processed by the
  RF Task.

  @note
  This file must be named with a _msg.h suffix for the build environment
  to correctly process the messages defined within.

*/

/*===========================================================================

  Copyright (c) 2009 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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
10/29/14   stw     Added structure ftm_lte_hw_path_query_data_s to ftm_lte_calibration_generic_rsp_pkt_type
09/15/14   ars     Fix for incorrect payload in FTM_SET_PATH_SEL_OVERRIDE_INDEX rsp pkt.
08/15/14    st     Alt Path Override Support
09/09/13   yzw     FTM LTE UL CW Tone Support: Command Interface
08/14/13   jf      Added: new msg for fb rx.
02/12/12   pl      remove redundant RFA_RF messages
07/12/12   pl      Moved FTM command processing to FTM task
                   removed obsolete RFA_RF_LTE_FTM messages
07/12/12   pl      Add FTM LTE CA support
05/22/12   aca     NPT KW Fix
05/22/12   aca     Added FTM command to extract RX AGC data
05/01/12   yzw     Modification and clean up of LTE FTM code for preparing diagnostic response packets
05/02/12   ars     Modifed FTM frequency adjust logic to apply correction using XO manager API
04/24/12   ars     Modifed FTM frequency adjust logic to apply correction using XO manager API
03/24/12   yzw     Added FTM command FTM_LTE_SET_FREQ_ADJUST
02/29/12   yzw     Added LTE FTM command FTM_LTE_SET_LNA_STATE_DBM
02/07/12   can     Added FTM set modulation command.
10/17/11   ars     Added FTM_RF_GET_RX_LEVEL_DBM command
10/17/11   ars     Added FTM_SET_PA_STATE and FTM_SET_TX_POWER_DBM FTM Layer
09/14/11   can     Merge MDM9K tips to NikeL.
06/27/11   ars     Added FTM_SET_TX_POWER_DBM 
06/23/11   pl      Remove obsolete interface : set_rx_on
04/20/11   pl      Remove obsolete interfaces
03/14/11   pl      Adding Sleep and Wakeup Command for LTE FTM
02/09/11   aca     FTM dac setpoint value override support
02/08/11   aca     FTM modulation change support
12/21/10   whc     FTM Command for SMPS Override
09/28/10   whc     Added txagc oride MSGR UMID defines and cmd_s's
08/09/10   qma     Updated LTE IM2 FTM cmd interface to match legacy
07/27/10   pl      Remove measurement FTM command handler to Measurement module
07/01/10   mkv     Adding support LTE LPM HDET Mode.   
06/07/10   pl      Added FTM support for LTE IM2 CAL 
06/04/10   pl      Adding x->L IRAT FTM command 
02/18/10   pl      Added Set Secondary Chain
02/18/10   pl      Added GET_LNA_OFFSET
11/21/09   mkv     Updating the payload in rfa_rf_lte_ftm_generic_message_rsp_s 
				   from boolean to uint8 support more return status conditions
 				   instead of just pass fail.Adding a unique RSP packet to each
				   command instead of a generic response tp all commands.
10/21/09   mkv     Added TX/RX IQ-Mismatch commands.
10/13/09   mkv     Added FTM MAX ID to set ID length to uint16.
10/04/09   mkv     Updating command and structure definitions for LTE calibration.
09/22/09   mkv     Updating Tx Waveform command structure to recieve number of rbs.
04/29/09   mkv     Initial Checkin

===========================================================================*/

#ifndef RF_LTE_FTM_MSG_H
#define RF_LTE_FTM_MSG_H

/*===========================================================================

				  INCLUDE FILES

===========================================================================*/
#include "appmgr_msg.h"
#include "msgr.h"
#include "msgr_types.h"
#include "rfa_msgr.h"
#include "rfcom.h"

/*===========================================================================

			 EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*-----------------------------------------------------------------------*/
/*     MSGR COMMAND ID'S                                             */
/*-----------------------------------------------------------------------*/
/*! @brief ID for FTM to configure the TX sweep.
 */
#define RFA_RF_LTE_FTM_MSG_ID_CONFIGURE_TX_SWEEP  13

/*! @breif ID for FTM to set RX IQ Mismatch coefficients
*/
#define RFA_RF_LTE_FTM_MSG_ID_SET_RX_IQ_MISMATCH_COMP_COEFFTS          16

/*! @breif ID for FTM to get HDET from Sweep
*/
#define RFA_RF_LTE_FTM_MSG_ID_GET_HDET_FROM_SWEEP           22

/*! @breif ID for FTM to get all HDET from sweep
*/
#define RFA_RF_LTE_FTM_MSG_ID_GET_ALL_HDET_FROM_TX_SWEEP           23

/*! @breif ID for FTM to get LPM HDET from Sweep
*/
#define RFA_RF_LTE_FTM_MSG_ID_GET_LPM_HDET_FROM_SWEEP           31
/*! @breif ID for FTM to get all LPM HDET from sweep
*/
#define RFA_RF_LTE_FTM_MSG_ID_GET_ALL_LPM_HDET_FROM_TX_SWEEP           32

/*! @brief ID for FTM to override TXAGC power.
 */
#define RFA_RF_LTE_FTM_MSG_ID_OVERRIDE_TXAGC_OUTPUT_POWER    33

/*! @brief ID for FTM to override TXAGC backoff.
 */
#define RFA_RF_LTE_FTM_MSG_ID_OVERRIDE_TXAGC_BACKOFF    34

/*! @brief ID for FTM to move tx coupler to fbrx path.
 */
#define RFA_RF_LTE_FTM_MSG_ID_FBRX_UPDATE     35

/*! @brief ID for MAX FTM
 */
#define RFA_RF_LTE_FTM_MSG_ID_MAX    50

/*-----------------------------------------------------------------------*/
/*     MSGR RESPONSE ID'S                                             */
/*-----------------------------------------------------------------------*/

/*! @brief ID to as a generic FTM command and RSP
 */
#define RFA_RF_LTE_FTM_MSG_ID_GENERIC_MESSAGE				0xFF

/*! @brief ID to as a generic FTM command and RSP
 */
#define RFA_RF_LTE_FTM_MSG_MAX_ID				0xFFFF

/****************************************************************************
				RF Task LTE messages
****************************************************************************/
/*! @brief LTE subsystem message IDs the RF task receives. 
   This enum is never called by name. It just defines symbols.
*/
enum
{
  /*! Command to configure the LTE Tx Sweep, sent by the FTM task to the RF task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, CMD, CONFIGURE_TX_SWEEP, RFA_RF_LTE_FTM_MSG_ID_CONFIGURE_TX_SWEEP, rfa_rf_lte_ftm_configure_tx_sweep_cmd_s),

  /*! Command to set the Rx IQ Mismatch Coefficients, sent by the FTM task to the RF task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, CMD, SET_RX_IQ_MISMATCH_COMP_COEFFTS, RFA_RF_LTE_FTM_MSG_ID_SET_RX_IQ_MISMATCH_COMP_COEFFTS, rfa_rf_lte_ftm_set_rx_iq_mismatch_coeft_s),

  /*! Command to get the HDET value from the sweep for the index provided, sent by the FTM task to the RF task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, CMD, GET_HDET_FROM_TX_SWEEP, RFA_RF_LTE_FTM_MSG_ID_GET_HDET_FROM_SWEEP, rfa_rf_lte_ftm_get_hdet_from_sweep_s),

  /*! Command to get all the HDET values from sweep, sent by the FTM task to the RF task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, CMD, GET_ALL_HDET_FROM_TX_SWEEP, RFA_RF_LTE_FTM_MSG_ID_GET_ALL_HDET_FROM_TX_SWEEP, rfa_rf_lte_ftm_get_all_hdet_from_sweep_s),

  /*! Command to get the LPM HDET value from the sweep for the index provided, sent by the FTM task to the RF task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, CMD, GET_LPM_HDET_FROM_TX_SWEEP, RFA_RF_LTE_FTM_MSG_ID_GET_LPM_HDET_FROM_SWEEP, rfa_rf_lte_ftm_get_hdet_from_sweep_s),

  /*! Command to get all the LPM HDET values from sweep, sent by the FTM task to the RF task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, CMD, GET_ALL_LPM_HDET_FROM_TX_SWEEP, RFA_RF_LTE_FTM_MSG_ID_GET_ALL_LPM_HDET_FROM_TX_SWEEP, rfa_rf_lte_ftm_get_all_hdet_from_sweep_s),

  /*! Command to enable TXAGC output power override, sent by the FTM task to the RF task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, CMD, OVERRIDE_TXAGC_OUTPUT_POWER, RFA_RF_LTE_FTM_MSG_ID_OVERRIDE_TXAGC_OUTPUT_POWER, rfa_rf_lte_ftm_oride_txagc_output_power_cmd_s),

 /*! Command to enable TXAGC backoff override, sent by the FTM task to the RF task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, CMD, OVERRIDE_TXAGC_BACKOFF, RFA_RF_LTE_FTM_MSG_ID_OVERRIDE_TXAGC_BACKOFF, rfa_rf_lte_ftm_oride_txagc_backoff_cmd_s),

  /*! Command to setup or release fbrx, sent by the FTM task to the RF task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, CMD, FBRX_UPDATE, RFA_RF_LTE_FTM_MSG_ID_FBRX_UPDATE, rfa_rf_lte_ftm_fbrx_update_cmd_s),

};

/*! @brief LTE subsystem message IDs the RF task sends as a response. 
   This enum is never called by name. It just defines symbols.
*/
enum
{
  /*! Generic FTM Command. Allows testability.*/
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, RSP, GENERIC_MESSAGE, RFA_RF_LTE_FTM_MSG_ID_GENERIC_MESSAGE, rfa_rf_lte_ftm_generic_message_cmd_s),

  /*! Response to configure the LTE Tx Sweep, sent by the RF task to the FTM task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, RSP, CONFIGURE_TX_SWEEP, RFA_RF_LTE_FTM_MSG_ID_CONFIGURE_TX_SWEEP, rfa_rf_lte_ftm_generic_message_cmd_s),

  /*! Response to set the Rx IQ Mismatch Coefficients, sent by the RF task to the FTM task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, RSP, SET_RX_IQ_MISMATCH_COMP_COEFFTS, RFA_RF_LTE_FTM_MSG_ID_SET_RX_IQ_MISMATCH_COMP_COEFFTS, rfa_rf_lte_ftm_generic_message_cmd_s),

  /*! Command to stop the Tx LTE Waveform transmission, sent by the RF task to the FTM task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, RSP, GET_HDET_FROM_TX_SWEEP, RFA_RF_LTE_FTM_MSG_ID_GET_HDET_FROM_SWEEP, rfa_rf_lte_ftm_get_hdet_from_sweep_s),

  /*! Command to stop the Tx LTE Waveform transmission, sent by the RF task to the FTM task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, RSP, GET_ALL_HDET_FROM_TX_SWEEP, RFA_RF_LTE_FTM_MSG_ID_GET_ALL_HDET_FROM_TX_SWEEP, rfa_rf_lte_ftm_get_all_hdet_from_sweep_s),

  /*! Command to stop the Tx LTE Waveform transmission, sent by the RF task to the FTM task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, RSP, GET_LPM_HDET_FROM_TX_SWEEP, RFA_RF_LTE_FTM_MSG_ID_GET_LPM_HDET_FROM_SWEEP, rfa_rf_lte_ftm_get_hdet_from_sweep_s),

  /*! Command to stop the Tx LTE Waveform transmission, sent by the RF task to the FTM task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, RSP, GET_ALL_LPM_HDET_FROM_TX_SWEEP, RFA_RF_LTE_FTM_MSG_ID_GET_ALL_LPM_HDET_FROM_TX_SWEEP, rfa_rf_lte_ftm_get_all_hdet_from_sweep_s),

  /*! Command to enable TXAGC output power override, sent by the RF task to the FTM task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, RSP, OVERRIDE_TXAGC_OUTPUT_POWER, RFA_RF_LTE_FTM_MSG_ID_OVERRIDE_TXAGC_OUTPUT_POWER, rfa_rf_lte_ftm_generic_message_cmd_s),
  
  /*! Command to enable TXAGC backoff override, sent by the RF task to the FTM task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, RSP, OVERRIDE_TXAGC_BACKOFF, RFA_RF_LTE_FTM_MSG_ID_OVERRIDE_TXAGC_BACKOFF, rfa_rf_lte_ftm_generic_message_cmd_s),

  /*! Command to setup or release fbrx, sent by the RF task to the FTM task. */
  MSGR_DEFINE_UMID(RFA, RF_LTE_FTM, RSP, FBRX_UPDATE, RFA_RF_LTE_FTM_MSG_ID_FBRX_UPDATE, rfa_rf_lte_ftm_generic_message_cmd_s),

};

/*===========================================================================

				   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef enum
{
  RFA_RF_LTE_FTM_SUCCESS=1,
  RFA_RF_LTE_FTM_STATE_MACHINE_ERROR=2,
  RFA_RF_LTE_FTM_EXECUTION_ERROR=3,
  RFA_RF_LTE_FTM_CMD_OUTCOME_MAX=4,
} rfa_rf_lte_ftm_cmd_outcome_t;

/*Structures for the commands that FTM task sends to the RF task.*/

typedef struct
{
  msgr_hdr_struct_type   hdr;
  boolean success;  
} rfa_rf_lte_ftm_generic_message_cmd_s;

typedef enum
{
	RFA_RF_LTE_FTM_UL_WAVEFORM_CW,
	RFA_RF_LTE_FTM_UL_WAVEFORM_PUSCH,
	RFA_RF_LTE_FTM_UL_WAVEFORM_PUCCH,
	RFA_RF_LTE_FTM_UL_WAVEFORM_PRACH,
	RFA_RF_LTE_FTM_UL_WAVEFORM_SRS,
	RFA_RF_LTE_FTM_UL_WAVEFORM_UPPTS
}rfa_rf_lte_ftm_ul_waveform_type;


typedef struct
{
  byte tx_waveform;
  byte num_rbs_pusch;
  byte num_rbs_pucch;
  byte start_pusch_rb_index;
  int16 cw_offset_kHz;    
} rfa_rf_lte_ftm_set_tx_waveform_data_s;
typedef PACK(struct)
{
  uint8 rx_chain;
  int16 rx_agc;
  uint8 rx_lna_state;
} rfa_rf_lte_ftm_get_rx_agc_chain_data;
 
typedef PACK(struct)
{
  uint8                           num_chains;
  rfa_rf_lte_ftm_get_rx_agc_chain_data   rfa_rf_lte_ftm_get_rx_agc_rec[RF_PATH_MAX]; /* actual size = 1..8 elements*/
} rfa_rf_lte_ftm_get_rx_agc_data_rsp_type;

typedef PACK(struct)
{
  rfcom_lte_band_type band;
  rfcom_lte_sub_band_type sub_band;
  uint8 path_override_index;
  uint8 alternate_path;
} ftm_lte_hw_path_query_data_s;

typedef enum
{
  rfa_rf_lte_ftm_UL_QPSK,
  rfa_rf_lte_ftm_UL_16QAM,
  rfa_rf_lte_ftm_UL_64QAM
} rfa_rf_lte_ftm_ul_modulation_type;
typedef PACK(struct)
{
  int32 user_freq_adjust_ppb;
  uint32 center_frequency_hz;
  uint8  enable_xo_freq_adjust;
  int32 total_freq_adjust_hz;
  uint16 status;  
}rfa_rf_lte_ftm_set_freq_adjust_cmd_data_s;

typedef struct
{
  uint16 rsvd;
  uint16 sweep_step_interval;
  uint8 sweep_gain_index_step_size;
  uint8 num_hdet_avgs;    
} rfa_rf_lte_ftm_configure_tx_sweep_data_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_ftm_configure_tx_sweep_data_s config_tx_sweep;
} rfa_rf_lte_ftm_configure_tx_sweep_cmd_s;
typedef struct
{
  uint8    device;
  uint32   iq_mismatch_coeft;
} rfa_rf_lte_ftm_set_rx_iq_mismatch_coeft_data_s;


typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_ftm_set_rx_iq_mismatch_coeft_data_s   rx_iq_mismatch_data;
} rfa_rf_lte_ftm_set_rx_iq_mismatch_coeft_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
} rfa_rf_lte_ftm_get_rx_agc_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint16 index;
} rfa_rf_lte_ftm_get_hdet_from_sweep_s;
typedef struct
{
  msgr_hdr_struct_type   hdr;
} rfa_rf_lte_ftm_get_all_hdet_from_sweep_s;
typedef struct
{
  uint16                 gain_index;
  int16                  expected_agc_val;
} rfa_rf_lte_ftm_get_lna_offset_data_s;
typedef struct
{
  msgr_hdr_struct_type   hdr;
  boolean pwr_oride_enable;
  int32 counts_pwr_oride_val;
} rfa_rf_lte_ftm_oride_txagc_output_power_cmd_s;


typedef struct
{
  msgr_hdr_struct_type   hdr;
  boolean backoff_oride_enable;
  int32 backoff_oride_val;
} rfa_rf_lte_ftm_oride_txagc_backoff_cmd_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  boolean move_tx_coupler_to_fbrx_path;
}rfa_rf_lte_ftm_fbrx_update_cmd_s;

typedef PACK(struct)
{
  uint8 ftm_lte_set_tx_power_enable;
  int16 ftm_lte_set_tx_power_dbmby10;
}rfa_rf_lte_ftm_set_tx_power_dbm_cmd_data_s;

typedef struct
{
  uint8 ftm_lte_set_net_sig_value;
}rfa_rf_lte_ftm_set_net_sig_value_cmd_data_s;

/*The structures for the responses that RF sends back to the FTM task.*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint8 status;  
} rfa_rf_lte_ftm_generic_message_rsp_s;

typedef struct
{
  uint16 dac_setpoint_override_enable;
  uint16 dac_setpoint_value;
} rfa_rf_lte_ftm_dac_setpoint_cmd_data_s;

typedef PACK(struct)
{
  uint8 sys_id;
  uint8 subsys_id;
  uint16 cmd_id;
  uint16 cmd_code;
} ftm_lte_calibration_rsp_pkt_hdr_type;

typedef PACK(struct)
{
  uint16 index;
  uint16 val;
} ftm_lte_id_val_type;

typedef PACK(struct)
{
  uint16 gain_index;
  int16 lna_offset;
} ftm_lte_get_lna_offset_rsp_type;

typedef PACK(struct)
{
  uint8 i_code;
  uint8 q_code;
  uint8 trans_conductance;
} ftm_lte_im2_cal_rsp_type;

/*This structure defines the response type for RFA_RF_LTE_FTM_SET_LNA_STATE_DBM_RSP*/
typedef PACK(struct)
{ 
  int8 selected_lna_state;
  uint16 status;
}ftm_lte_set_lna_state_dbm_rsp_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for RFA_RF_LTE_FTM_SET_MODE_RSP
 */
typedef PACK(struct)
{
  ftm_lte_calibration_rsp_pkt_hdr_type hdr;

  union
  {
    uint8 status;
    uint16 status_path_sel_override;
    ftm_lte_id_val_type id_val;
    uint16 hdet_array[32];
    uint16 rx_agc;
    uint16 adc_val;
    ftm_lte_get_lna_offset_rsp_type lna_offset_val;
    ftm_lte_im2_cal_rsp_type  im2_code;
    uint16 dac_setpoint;
    uint16 status_set_tx_power_dbmby10;
    uint16 status_set_net_sig_value;
    uint16 status_get_rx_level_dbm;
    int16 rsp_get_rx_level_response[2];
    ftm_lte_set_lna_state_dbm_rsp_type set_lna_state_dbm;
    rfa_rf_lte_ftm_set_freq_adjust_cmd_data_s set_freq_adjust;	
    rfa_rf_lte_ftm_get_rx_agc_data_rsp_type rx_agc_data;
    ftm_lte_hw_path_query_data_s path_query_data[3];
  };
} ftm_lte_calibration_generic_rsp_pkt_type;

/*-----------------------------------------------------------------------*/
/* This structure is used to support overriding Alternate Path Index for PCELL or SCELL devices */
typedef PACK(struct)
{
  /*! Defines the Master Logical Device Index */
  uint8 master_dev_id;  
  /*! Defines the Master Logical Device Alt Path Index */
  uint8 master_dev_alt_path_idx;
  /*! Defines the Slave Logical Device Index */
  uint8 slave_dev_id;
  /*! Defines the Slave Logical Device Alt Path Index */
  uint8 slave_dev_alt_path_idx;
} rfa_rf_lte_ftm_alt_path_oride_params_s;

/*-----------------------------------------------------------------------*/
typedef PACK(struct)
{ 
  msgr_hdr_struct_type hdr;
  rfa_rf_lte_ftm_alt_path_oride_params_s lte_ftm_alt_path_oride_params;
} rfa_rf_lte_ftm_alt_path_oride_ind_s; 

/*-----------------------------------------------------------------------*/


#endif /* RF_LTE_FTM_MSG_H */




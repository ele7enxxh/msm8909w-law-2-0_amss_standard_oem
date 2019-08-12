#ifndef FTM_COMMON_H
#define FTM_COMMON_H

/*==============================================================================

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/15    br     Added support for DL power level input by user
02/04/15   sd      API change support for FTM  Sensitivity Estimation
01/13/15   bar     Added embedded support for Sensitivity/CNR calculation
12/10/13   aj      Added FTM_FEEDBACK_SETUP command
09/10/13   aa      Enable/Disable NBEE RXCAL on the fly
07/29/13   aka     Added FTM_SET_FB_PATH_STATE cmd
07/10/13   aa      RX calibration optimization using NBEE
04/18/13   kai     Reduced NUM_OF_AVG_RX_AGC_READ
07/02/12   cvd     Add FTM_TX_RX_INVALID_STATE to ftm_tx_rx_error_code_type.
01/16/12   aro     Added common interface to get therm read from any device
                   for a given mode
10/21/09   mkv     Removed LTE specific commands
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
09/11/09   aro     Typo 
09/09/09   aro     Removed DateTime and Author + Extra Space Removal
07/22/09   aro     Added Composite Cal Error Return value
11/16/07   adk     Merged changes from FTM tips.
02/28/07   ycl     Fix compiler warnings.
07/23/06   vm      Created for initial version of FTM for MSM7600
 
==============================================================================*/

#include "rfa_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ftmicap.h"

#ifdef FEATURE_FTM_HWTC
#include "ftm_log.h"
#endif

/*----------------------------------------------------------------------------*/
#define FTM_NUM_OF_AVG_NBEE_READ      6
#define FTM_NUM_OF_AVG_RX_AGC_READ    5
#define FTM_NBEE_DELAY_READ           70
/*----------------------------------------------------------------------------*/
#define FTM_NUM_OF_AVG_HDET_READ      10

/*----------------------------------------------------------------------------*/
#define FTM_UPLINK_SWEEP_CAL_MIN      0

/*----------------------------------------------------------------------------*/
#define FTM_UPLINK_SWEEP_CAL_MAX      511

/*----------------------------------------------------------------------------*/
#define FTM_UPLINK_SWEEP_CAL_MIN_STEP 1

/*----------------------------------------------------------------------------*/
#define FTM_UPLINK_SWEEP_CAL_SIZE     (((FTM_UPLINK_SWEEP_CAL_MAX - FTM_UPLINK_SWEEP_CAL_MIN)/FTM_UPLINK_SWEEP_CAL_MIN_STEP) + 1)

/*----------------------------------------------------------------------------*/
#define FTM_TX_RX_SWEEP_NO_ACTION 0xFF

/*----------------------------------------------------------------------------*/
#define FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ 21

/*----------------------------------------------------------------------------*/
#define FTM_FREQ_CAL_SWEEP_NUM_HDET_AVG 5

/*----------------------------------------------------------------------------*/
#define FTM_TX_RX_FREQ_CAL_MAX_FREQS 16

/*----------------------------------------------------------------------------*/
#define FTM_TX_RX_MIN_STEP_LENGTH 01 /* 655 sleep clock cycles ~= 20 ms */

/*----------------------------------------------------------------------------*/
typedef enum {
  FTM_RX_LNA_OFFSET0,
  FTM_RX_LNA_OFFSET1,
  FTM_RX_LNA_OFFSET2,
  FTM_RX_LNA_OFFSET3,
  FTM_RX_LNA_OFFSET4,
  FTM_RX_LNA_OFFSET5,
  FTM_RX_DVGA_OFFSET = 100,
  FTM_RX_NO_ACTION = 0xFF
} ftm_rx_action_type;

/*----------------------------------------------------------------------------*/
typedef enum
{
  FTM_TX_RX_SUCCESS          = 0,
  FTM_TX_RX_STEPS_INVALID    = 1,
  FTM_TX_RX_FREQS_INVALID    = 2,
  FTM_TX_RX_STEP_LEN_INVALID = 4,
  FTM_TX_RX_TRIG_FAILURE     = 5,
  FTM_TX_RX_INVALID_STATE    = 6,
  FTM_TX_RX_SYSTEM_MODE_INVALID    = 0xFF
} ftm_tx_rx_error_code_type;

#ifdef FEATURE_FTM_HWTC
/*----------------------------------------------------------------------------*/
typedef PACK(struct)
{
  ftm_log_gen_type  header;
  int16             hdet;
  int16             filt_hdet;
  int16             tx_agc_pdm;
} ftm_hdet_track_debug_log_type;
#endif /* FEATURE_FTM_HWTC */

/*----------------------------------------------------------------------------*/
/* This structure is required for IM2 functions.*/
typedef PACK(struct)
{
  byte i_dac;
  byte q_dac;
  byte transconductor;
  int16  suppression_level;   /* in dB, value will be 100 times bigger */
} ftm_im2_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM common dispatch request packet to be used to query the THERM read for
  a given device
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 20 for FTM_COMMON_C */

  uint16  ftm_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8 device;
  /*!< The device on which THERM read is requested */

  uint8 rf_mode;
  /*!< FTM RF Mode for which the THERM read is requested */

} ftm_common_get_therm_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM common dispatch request packet to be used to enable and set gain state for FB path 
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 20 for FTM_COMMON_C */

  uint16  ftm_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8 tx_device;
  /*!< Primary device ID for which FB device need to be enabled */

  boolean fb_path_enable;
  /*!< Enable/disable FB path */

  uint8 fb_gain_state;
  /*!< Gain state to be set on FB path*/

} ftm_common_set_fb_path_state_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM common dispatch response packet to be used to return THERM read for
  a given device
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 mode;
  /*!< RF Mode Identification. For Instance - 20 for FTM_COMMON_C */

  uint16 ftm_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16 req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16 rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint32 therm_read;
  /*!< Thermistor reading for the given device */

  uint8 therm_status;
  /*!< Status of the thermistor read */

} ftm_common_get_therm_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM common dispatch request packet to be used to enable and set gain state for FB path 
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 20 for FTM_COMMON_C */

  uint16  ftm_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8 version;
  /*!< Version of the OpCode */

  uint8 debug_mode_en;
  /*!< Flag indicating debug mode for feedback setup opcode */

  uint16 flag;
  /*!< 16-bit flag indicating a specific type of action to be perform the 
  given payload. */

  uint8 primary_device;
  /*!< Primary Device associated with the feedback path */

  uint8 feedback_device;
  /*!< Feedback device which needs to be configured */

  uint16 feedback_setup_action;
  /*!< 16-bit action Mask indicating the type of Feedback config action to be done
  Bit 0 = Configure WTR | Bit 1 = Configure RxLM */

  uint8 enable_mode_configure_wtr;
  /*!< Flag indicating, if the WTR feedback device should be configrued in 
    feedback mode or not */

  uint8 fb_state;
  /*! Gain State to which the feedback device is to be configured */

  uint8 enable_mode_configure_rxlm_xpt;
  /*!< Flag indicating, if the RxLM device should be configrued in 
    feedback mode or not */

  uint8 enable_mode_configure_rxlm_fbagc;
  /*!< Flag indicating, if the RxLM device should be configrued in 
    feedback mode or not */

} ftm_feedback_setup_req_packet_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM common dispatch response packet to be used to enable and set gain state for FB path 
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 20 for FTM_COMMON_C */

  uint16  ftm_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8 status;
  /*!< Status of the Feedback Setup Command*/

}ftm_feedback_setup_resp_packet_type;


typedef struct
{
  int32 cnr_db100;
  int32 sens_db100;
  int32 peak_freq;
}ftm_common_sens_est_result_type;


void ftm_common_analyze_iq_samples(ftm_common_sens_est_result_type *sens_est_result_ptr, 
                                       uint16 *iq_buf_ptr,
                                       uint32 sample_size,
                                       uint32 samp_freq,
                                       ftm_rf_technology_type rf_tech,
                                       int16 dl_pwr_lvl);


#endif /* FTM_COMMOM_H */

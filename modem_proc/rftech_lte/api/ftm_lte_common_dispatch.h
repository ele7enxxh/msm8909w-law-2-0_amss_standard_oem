/*!
  @file ftm_lte_common_dispatch.h

  @brief
  This files contains the definition of the LTE disaptcher shared 
  for RF CAL and Non-signaling commands, in addition to some common 
  ultilities

*/

/*===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/ftm_lte_common_dispatch.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/14   bar     Added FTM interface for FTM_RF_IQ_CAPTURE_ANALYZE_SAMPLES() Command.
10/14/14   jmf     [selftest] Implement FBRx Noise Floor Cal
10/07/14   dbz     [selftest] Add spectrum flatness measurement feature
09/19/14   jmf     [SelfTest] Add definitions for VSWR v2 Meas
08/05/14   jmf     [SelfTest] Port Dime Defs for VSWR result return
05/16/14   cri     IQ capture segment reduction changes
08/21/13   dj      Added FTM_INT_CALV3_FEEDBACK_SETUP command
07/30/13   aka     Updating Rx sample size to 32 bit I/Q
07/15/13   ars     Added FTM interface for self-test APIs
05/01/13   gvn     LTE RSB Cal Support
11/14/12   cri     LTE radio setup opcode support 
02/03/12   ems     Added support for LTE RFM mode switch
10/01/09   pl      Adding support for diag response packet in LTE 
                   Non-signaling commands processing
05/14/08   mkv	  Moving the file from rfa/non_signaling/common to ftm/common.
05/08/09   pl     Initial Check-in
===========================================================================*/

#ifndef FTM_LTE_COMMON_DISPATCH_H
#define FTM_LTE_COMMON_DISPATCH_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_common_calibration_v3.h"

/* *************************************************************/
/* ***************FTM FBRx IQ capture macros*****************/
/* *************************************************************/

// The LTE FBRx IQ capture structure which stores all the relevent data when acquiring samples.

/*! Maximum FBRx LTE IQ Samples that can be captured  */
#define FTM_LTE_FBRX_IQ_SAMPLES_MAX 9216

/*! FBRx LTE Sample server IQ format - Tx*/
#define FTM_LTE_FBRX_IQ_SAMPLE_DATA_FORMAT_TX FTM_IQ_8_BIT_FORMAT 

/*! FBRx LTE Sample server IQ format - Rx*/
#define FTM_LTE_FBRX_IQ_SAMPLE_DATA_FORMAT_RX FTM_IQ_32_BIT_FORMAT

/*! LTE FBRx IQ capture sampling frequency */
#define FTM_LTE_FBRX_IQ_SAMPLE_FREQ_1P4MHZ (30.72e6)
#define FTM_LTE_FBRX_IQ_SAMPLE_FREQ_3MHZ   (30.72e6)
#define FTM_LTE_FBRX_IQ_SAMPLE_FREQ_5MHZ   (30.72e6)
#define FTM_LTE_FBRX_IQ_SAMPLE_FREQ_10MHZ  (30.72e6)
#define FTM_LTE_FBRX_IQ_SAMPLE_FREQ_15MHZ  (61.44e6)
#define FTM_LTE_FBRX_IQ_SAMPLE_FREQ_20MHZ  (61.44e6)

/*! This is the maximum number of LTE FBRx IQ samples that can be sent through the diag. */
/*! If diag requires more, it has to get it through multiple iterations */
#define FTM_LTE_FBRX_IQ_DIAG_MAX_SAMPLES_TX (FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)
#define FTM_LTE_FBRX_IQ_DIAG_MAX_SAMPLES_RX ((FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)/4)


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for RF Mode configuration Instruction.

  @details
  Contains the RF Mode configuration instruction (Payload)
*/
typedef PACK(struct)
{

  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_RFM_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  ftm_cal_instr_payload_config_radio_type  ftm_cal_instr_payload_config_radio;
  /*!< Structure template for RF Mode configuration Instruction */


} ftm_cal_instr_payload_config_radio_req_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Tx Waveform Tx Measurement Instruction.

  @details
  Contains the RF Mode configuration instruction (Payload)
*/
typedef PACK(struct)
{

  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_RFM_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  ftm_cal_instr_payload_config_tx_wf_type  ftm_cal_instr_payload_config_tx_wf;
  /*!< Structure template for Resource Block for Tx Waveform configuration Instruction */


} ftm_cal_instr_payload_cfg_tx_wf_req_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retune Radio for Calibration Sweep
 
  @details
  Contains the retune instruction (Payload)
 
  @param rf_retune
  Pointer to the data structure containing RF retune parameter:
 
  primary chain
  ul channel
*/
typedef PACK(struct)
{

  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_RFM_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  ftm_cal_instr_payload_retune_radio_type  ftm_cal_instr_payload_retune_radio;
  /*!< Structure template for RF retune Instruction. */

} ftm_cal_instr_payload_cfg_retune_radio_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  tearing down radio
 
  @details
  Contains the tear down instruction (Payload)
 
  @param
  none
 
  @return
  boolean
*/
typedef PACK(struct)
{

  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_RFM_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

} ftm_cal_instr_payload_cfg_tear_down_radio_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Measure Tx params payload.

  @details
*/
typedef struct
{
  int16 tx_power;
  /*!< Tx Power in dB10 units */

  uint16 evm_value;
  /*!< EVM in % * 100 units*/

  int16 aclr_value[6];
  /*!< Array of ACLR values */  

  uint16 flatness_coef;
  /*!< Flatness coefficient x1000*/

  uint16 range1_ripple;
  /*!< Center of carrier band ripple in dB x1000*/

  uint16 range2_ripple;
  /*!< Edge of carrier band ripple in dB x1000*/

  int16 vswr_ratio_dB10;
  /*!< value of VSWR in dB10 */

  int16 vswr_v2_return_loss;
  /*!< value of VSWR Return Loss in  dB100 */

  int16 vswr_v2_mag;
  /*!< value of VSWR Magnitude */

  int16 vswr_v2_phase;
  /*!< value of VSWR phase  */

  int16 vswr_v2_fwd_pwr;
  /*!< Value of forward power in dbm100 */

  int16 vswr_v2_rev_pwr;
  /*!< Value of reverse power in dbm100 */

} ftm_rf_lte_payload_tx_params_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template to store the information regarding acquired IQ samples.
 
  @details
*/
typedef struct
{
 rfm_device_enum_type device; 
 /*!< The device for which IQ samples have been acquired */

 uint16 req_samples;
 /*!< The number of Tx and Rx samples requested to be acquired */

 int32 *iq_buf_tx;
 /*!< The buffer which stores LTE Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

 int32 *iq_buf_rx;         
 /*!< The buffer which stores LTE IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

 int32 *iq_buf_tx_rsp_data;
 /*!< The buffer which stores LTE Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

 int32 *iq_buf_rx_rsp_data;         
 /*!< The buffer which stores LTE IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

 uint16 tx_iq_data_siz_bytes;
 /*!< The buffer which stores LTE Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

 uint16 rx_iq_data_siz_bytes;
 /*!< The buffer which stores LTE IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

 boolean data_available;      
 /*!< Flag to indicate that data capture has been successful */

 uint32 sampling_frequency;
 /*!< Sampling frequency that was used to acquire IQ samples */

 ftm_rf_lte_payload_tx_params_type meas_results;
 /*!< Structure to contain self-test tx meas results */

 ftm_rf_fbrx_noise_binsum_type noise_meas_results[4];
 /*!< Structure to contain self-test fbrx noise cal results */

}ftm_rf_lte_fbrx_iq_acquired_samples_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template to store the information regarding acquired
  FTM RF Rx IQ samples.
 
  @details
*/
typedef struct
{
  uint32 acquired_iq_data_sample_size;
  /*!< The number of Rx IQ samples successfully acquired. */
  int8 acquired_iq_data_state;
  /*!< Represent if the Rx IQ samples have been captured or not. 
    0 - IQ samples not acquired.
    1 - IQ samples have been previously captured using FTM_RF_ACQUIRE_IQ_SAMPLES command*/
}ftm_rf_lte_rx_iq_samples_state_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
void ftm_lte_msg_dispatch(ftm_msgr_u* msg_ptr, uint32 msg_size, ftm_rsp_pkt_type *rsp_pkt_ptr);
ftm_rsp_pkt_type ftm_lte_rex_dispatch        (void  *ftm_req_data);
ftm_rsp_pkt_type ftm_lte_radio_enter         (void  *ftm_req_data);
ftm_rsp_pkt_type ftm_lte_radio_setup         (void  *ftm_req_data);
ftm_rsp_pkt_type ftm_lte_feedback_setup      (void  *ftm_req_data);
ftm_rsp_pkt_type ftm_lte_calv3_xpt_iq_capture(void  *ftm_req_data);
ftm_rsp_pkt_type ftm_lte_cfg_retune_radio    (void  *ftm_req_data);
ftm_rsp_pkt_type ftm_lte_cfg_tear_down_radio (void  *ftm_req_data);
rfcom_lte_band_type rflte_ftm_mc_convert_mode_id_to_band(ftm_mode_id_type mode_id);

#endif /*  FTM_LTE_COMMON_DISPATCH_H */

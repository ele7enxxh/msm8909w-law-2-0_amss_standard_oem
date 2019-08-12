#ifndef FTM_CALIBRATION_V3_RADIO_H
#define FTM_CALIBRATION_V3_RADIO_H
/*!
  @file
  ftm_calibration_v3_radio.h

  @brief
  This module contains prototypes and definitions used by common RF calibration
  code.
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_calibration_v3_radio.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/22/14   shk     Added support for ver 1.0 of RADIO SETUP opcode
09/19/13   aa      Added support to enable NBEE RXCAL for WCDMA
08/21/13    dj     Added ftm_cal_feedback_setup_req_type to enable fb setup to 
                   be called from the tech's dispatch
08/19/13    kb     [XPT] Support for new TDS DPD waveform.
08/02/13    dj     Updated feedback setup to use fb_state instead of pa_state
                   Added new FB Setup Action to configure rxlm for fbagc
                   updated the existing rxlm configuration to be used for xpt
04/24/13    dj     Added Version field and updated the data types in the 
                   feedback setup instr parameter header structure
04/19/13    dj     Support for Feedback Setup Opcode
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/29/13   aro     Radio Config/Retune optimized opcode proc functions
03/29/13   aro     Tx Config/Meas optimized opcode proc functions
03/29/13   aro     Rx Config/Meas optimized opcode proc functions
11/14/12   cri     Move radio setup request to common 
11/06/12   aro     Added 1x handler function to perform cal v3 radio setup
11/05/12   aro     Removed instruction payload pointer from process
                   instruction function
11/05/12   aro     Added RFM Mode validity checking
11/05/12   aro     Removed OpCode init from opcode modules to common cal v3
                   init function
11/02/12   aro     Renamed function pointer type name
11/02/12   aro     Converted payload computation function to perform
                   opcode payload unpacking
10/31/12   aro     Added Tx waveform config support in Radio Setup
10/30/12   aro     Documentation Update
10/29/12   aro     Interface to process radio setup opcode
10/29/12   aro     Interface to query the payload size for Radoio setup opcode
10/29/12   aro     Added interface to initialize Radio OpCode
10/29/12   aro     Added interface to deinitialize Radio OpCode
10/29/12   aro     Defined payload for Radio setup
10/24/12   aro     Initial version of Radio Config code

==============================================================================*/

#include "comdef.h"
#include "ftm_calibration_v3_types.h"
#include "ftm_calibration_v3_opcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Forward declaration of Cal v3 data  */
struct ftm_calibration_data_type_t;

/*----------------------------------------------------------------------------*/
/*! Enumeration indicating the maximum number of channel list supported. Tools 
may send any number of channel list; however embedded side is limiting the MAX 
for ease in interface */
#define FTM_RADIO_SETUP_MAX_CHAN 3

/*----------------------------------------------------------------------------*/
/*! Enumeration indicating that the the salve device is invalid */
#define FTM_RADIO_SETUP_NO_SLAVE_DEV 0xFF


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration to indicate NBEE RX CAL is enabled
*/
#define FTM_CALIBRATION_NBEE_RXCAL_ENABLED  1

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type of Device Mask for FTM Calibration Command
 
  @details
  Contains the Master device information. Each bit corresponds to a RFM Device
  and indicates 
*/
typedef uint8 ftm_calibration_master_device_mask_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type of Slave Device Mask for FTM Calibration Command.
 
  @details
  Contains the Slave device information for each of the master device in the
  Master Device Mask.
 
  Bit[3:0] = SlaveDev of RFM_DEVICE_0 indicated by Bit[0] of MasterDev mask
  Bit[7:4] = SlaveDev of RFM_DEVICE_1 indicated by Bit[1] of MasterDev mask
  Bit[11:8] = SlaveDev of RFM_DEVICE_2 indicated by Bit[2] of MasterDev mask
  Bit[15:12] = SlaveDev of RFM_DEVICE_3 indicated by Bit[3] of MasterDev mask
  Bit[19:16] = SlaveDev of RFM_DEVICE_4 indicated by Bit[4] of MasterDev mask
  Bit[23:20] = SlaveDev of RFM_DEVICE_5 indicated by Bit[5] of MasterDev mask
  Bit[27:24] = SlaveDev of RFM_DEVICE_6 indicated by Bit[6] of MasterDev mask
  Bit[31:28] = SlaveDev of RFM_DEVICE_7 indicated by Bit[7] of MasterDev mask
*/
typedef uint32 ftm_calibration_slave_device_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for RF Mode configuration Instruction.

  @details
  Contains the RF Mode configuration instruction (Payload)
*/
typedef PACK(struct)
{
  ftm_calibration_master_device_mask_type device_master;
  /*!< Master RF Device for which the RF is to be configured to. This is a bit 
  mask, where each bit defines a master device. For instance. Bit 0 corresponds
  to RFM_DEVICE_0 */

  ftm_calibration_slave_device_type device_slave;
  /*!< Slave RF Device for which the RF is to be configured to. Each 4 bit 
  define the slave device for a master device. If the 4 bit value is set to 0xF, 
  that means, there is no slave device. */ 
  
  uint8 rf_mode;
  /*!< The RF Mode for which the RF Calibration is to be done. Supporting the
  Legacy definition, this variable will indicate the RF Tech and band on
  which the calibraiton is done. This will be later typecasted to 
  ftm_mode_id_type. */

  uint16 channel;
  /*!< The RF uplink channel to which the the Radio should be tuned to peform
  RF Calibration. For LTE, since downlink channel is different, software should 
  calculate the downlink channel based on this uplink channel. */ 

  uint16 rx_bandwidth;
  /*!< Bandwidth of the Rx Channel. This will be used by LTE only */

  uint16 tx_bandwidth;
  /*!< Bandwidth of the Rx Channel. This will be used by LTE only */

  uint16 dc_channel_offset;
  /*!< Channel/Frequency offset for Dual Carrier Calibration */

} ftm_cal_instr_payload_config_radio_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for RF retune Instruction.

  @details
  Contains the RF retune instruction (Payload)
*/
typedef PACK(struct)
{
  ftm_calibration_master_device_mask_type device_master;
  /*!< Master RF Device for which the RF is to be retuned. This is a bit 
  mask, where each bit defines a master device. For instance. Bit 0 corresponds
  to RFM_DEVICE_0 */

  uint16 channel;
  /*!< The RF uplink channel to which the the Radio should be tuned to peform
  RF Calibration. For LTE, since downlink channel is different, software should 
  calculate the downlink channel based on this uplink channel. */ 
  
  uint16 dc_channel_offset;
  /*!< Channel offset for Dual Carrier Calibration */

} ftm_cal_instr_payload_retune_radio_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Tx Waveform configuration Instruction.

  @details
  Contains the Tx waveform configuration instruction (Payload)
*/
typedef PACK(struct)
{
  uint8 waveform;
  /*!< The type of Tx Waveform to be used for Tx Measurement */

  uint8 num_of_rb;
  /*!< Number of LTE Resource Block for Tx Waveform. */

  uint8 start_rb;
  /*!< Starting Location of LTE Resource Block for Tx Waveform. */
 
} ftm_cal_instr_payload_config_tx_wf_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration listing the radio state for master device
*/
typedef enum
{
  MASTER_DEVICE_NO_CHANGE = 0,
  /*!< Indicates that master device state should not be altered */

  MASTER_DEVICE_DISABLED = 1,
  /*!< Indicates that the master device should be moved to disabled state */

  MASTER_DEVICE_RX = 2,
  /*!< Indicates that the master device should be moved to Rx State */

  MASTER_DEVICE_RXTX = 3,
  /*!< Indicates that the master device should be moved to RxTx State */

  MASTER_DEVICE_RETUNE = 4,
  /*!< Indicates that the master device and its all associated devices should 
  be retuned to new band and channel */

  MASTER_DEVICE_FEEDBACK = 5,
  /*!< Indicates that the master device should be moved to feedback mode */

  MASTER_DEVICE_STATE_MAX
  /*!< Indicates mmaximum valid state */ 

} ftm_cal_radio_setup_mdevice_state_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration listing the radio state for master device
*/
typedef enum
{
  SLAVE_DEVICE_NO_CHANGE = 0,
  /*!< Indicates that slave device state should not be altered */

  SLAVE_DEVICE_DISABLED = 1,
  /*!< Indicates that the slave device should be moved to disabled state */

  SLAVE_DEVICE_RXDIV = 2,
  /*!< Indicates that the slave device should be moved to RxDiv State */

  SLAVE_DEVICE_STATE_MAX
  /*!< Indicates mmaximum valid state */ 

} ftm_cal_radio_setup_sdevice_state_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration listing the Tx Waveforms
*/
typedef enum
{
  TX_WFM_DEFAULT = 0,
  /*!< Indicates that the default waveform is to be used */

  TX_WFM_CW = 1,
  /*!< Indicates that the CW waveform is to be used */

  TX_WFM_OFFSET_CW = 2,
  /*!< Indicates that the offset CW waveform is to be used */

  TX_WFM_OFFSET_LTE_PUSCH = 3,
  /*!< Indicates that the LTE PUSCH waveform is to be used */

  TX_WFM_OFFSET_EUL = 4,
  /*!< Indicates that the EUL waveform is to be used */

  TX_WFM_DPD = 5,
  /*!< Indicates that the DPD waveform is to be used */

} ftm_cal_radio_setup_wfm_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration listing Radio setup action types
*/
typedef enum
{
  FTM_SPLIT_BAND_PATH_OVERRIDE = 0,
  /*!< Selects h/w path based on override index */

  FTM_ALT_PATH_OVERRIDE = 1,
  /*!< Allows RF front end setup in case of multiple physical path 
    like we have in CA case */

  FTM_RADIO_SETUP_ACTION_MAX = 0xFFFF
  /*!< Max enum value. INVALID */

} ftm_cal_radio_setup_action_enum_type;

/*----------------------------------------------------------------------------*/
/*! Definition for radio setup action type */
typedef uint32 ftm_cal_radio_setup_action_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the payload for path override params */
typedef PACK(struct)
{
  uint8 master_path_idx;
  /*!< Master path override index value */

  uint8 slave_path_idx;
  /*!< Slave path override index value */

} ftm_cal_radio_setup_device_idx_type;

/*----------------------------------------------------------------------------*/
/*! Definition for waveform type */
typedef uint8 ftm_cal_radio_setup_wfm_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold channel information for Radio Setup Instruction.
*/
typedef PACK(struct)
{
  uint32 channel;
  /*!< WAN channel number to which the radio is to be tuned to. */

  uint32 channel_rx_bw;
  /*!< Rx bandwidth for the channel to which the radio is to be tuned to */

  uint32 channel_tx_bw;
  /*!< Tx bandwidth for the channel to which the radio is to be tuned to */

} ftm_cal_radio_setup_chan_info_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the payload for offset CW waveform */
typedef PACK(struct)
{
  int32 offset_hz;
  /*!< Offset in hertz for CW */

} ftm_cal_radio_setup_wfm_ocw_info_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the payload for LTE PUSCH waveform */
typedef PACK(struct)
{
  uint8 num_of_rbs;
  /*!< Number of Resource blocks */

  uint8 start_rb;
  /*!< Start Resource blocks */

} ftm_cal_radio_setup_wfm_pusch_info_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the payload for EUL waveform */
typedef PACK(struct)
{
  uint16 sub_test;
  /*!< Predetermined Subtest for configuring EUL waveform parameters 
  (like betaC, betaD, etc.) */
} ftm_cal_radio_setup_wfm_eul_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Radio Setup Instruction.
*/
typedef PACK(struct)
{
  uint8 version;
  /*!< Version of the OpCode */

  uint8 debug_mode_en;
  /*!< Flag indicating debug mode for sync opcode */

  uint16 flag;
  /*!< 16-bit flag indicating a specific type of action to be perform the 
  given payload. */

  uint8 master_device;
  /*!< Master Device which will be tuned to a given Master Radio State */

  uint8 slave_device;
  /*!< Slave Device which will be tuned to RxDiversity state. For the case, 
  when no diversity device is to be setup, this will be passed as 0xFF. */

  uint8 master_radio_state;
  /*!< The radio state which will be used to configure the master device. The 
  radio state is defined by #ftm_cal_radio_setup_mdevice_state_type */

  uint8 slave_radio_state;
  /*!< The radio state which will be used to configure the slave device. The 
  radio state is defined by #ftm_cal_radio_setup_sdevice_state_type */

  uint8 technology;
  /*!< WAN technology for which the radio is to be setup.This will be 
  defined by #ftm_rf_technology_type */

  uint16 band;
  /*!< WAN band to which the the radio should be tuned to. This will 
  be intrepeted as per tech specific band enum.*/

  uint8 num_of_channel;
  /*!< Number of channel for the given band. */

  ftm_cal_radio_setup_chan_info_type channel_list;
  /*!< Variable to be used to extract the address of the memory which stores 
  list of channnel information. */

} ftm_cal_radio_setup_instr_payload_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to hold device params and valid flag for path override type
*/
typedef struct
{
   boolean valid;
   /*!< Flag to indicate if we have a valid case for path override */

   uint8 master_path_idx;
   /*!< Master path override index value */

   uint8 slave_path_idx;
   /*!< Slave path override index value */

} ftm_path_override_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to hold the unpacked radio config parameter used to send
  to tech specific functions
*/
typedef PACK(struct)
{
  ftm_cal_opcode_version_type version;
  /*!< Flag indicating version of the radio setup opcode. This is used 
    to determine the size of payload based on radio setup action*/

  boolean debug_mode_en;
  /*!< Flag indicating debug mode for sync opcode */

  uint16 flag;
  /*!< 16-bit flag indicating a specific type of action to be perform the 
  given payload. */

  rfm_device_enum_type mdevice;
  /*!< Master radio device on which radio setup is requested */

  rfm_device_enum_type sdevice;
  /*!< Slave radio device on which radio setup is requested */

  ftm_cal_radio_setup_mdevice_state_type mdevice_state;
  /*!< Radio state of master device to be acheived */

  ftm_cal_radio_setup_sdevice_state_type sdevice_state;
  /*!< Radio state of Slave device to be acheived */

  ftm_rf_technology_type tech;
  /*!< WAN technology for which the radio setup is to be done */

  uint16 band;
  /*!< Tech specific band */

  uint8 num_of_chan;
  /*!< Number of channels sent requested to be tuned. Based on this the 
  chan_list will be filled. */

  struct
  {
    uint32 channel;
    /*!< Channel for tuning */

    uint32 rx_bw;
    /*!< Rx radio bandwidth */

    uint32 tx_bw;
    /*!< Tx radio bandwidth */

  } chan_list[FTM_RADIO_SETUP_MAX_CHAN];
  /*!< Structure array to hold list of channel param. This will be populated 
  according to num_of_chan */

  ftm_cal_radio_setup_wfm_enum_type wfm_type;
  /*!< Tx waveform type to be used during Tx tunig */

  union
  {
    struct
    {
      int32 offset_hz;
      /*!< ofsset in hertz */

    } ocw_wfm;
    /*!< Structure to hold offset CW param */

    struct
    {
      uint8 num_of_rbs;
      /*!< Number of resource blocks */

      uint8 start_rbs;
      /*!< Start resource blocks */

    } pusch_wfm;
    /*!< Structure to hold LTE PUSCH waveform param */

    struct
    {
      uint16 subtest;
    } eul_wfm;
    /*!< Structure to hold EUL waveform param */

  } wfm_param;
  /*!< Union of structures holding the waveform parameter for each waveform 
  type*/

  ftm_path_override_type split_band_path_override;
  /*!< Structure to hold Split band Path override params */

  ftm_path_override_type alt_path_override;
  /*!< Structure to Alternate Path override params */

} ftm_cal_radio_setup_parameter_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of all Feedback Setup Actions.
*/
typedef enum
{

  FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_WTR            = 0x01,
  /*!< Feedback Action: Configure WTR */

  FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_XPT   = 0x02,
  /*!< Feedback Action: Configure RxLM for XPT */

  FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_FBAGC = 0x04
  /*!< Feedback Action: Configure RxLM for FBAGC */
} ftm_calibration_feedback_setup_action_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for wtr feedback setup params 
*/
typedef PACK(struct)
{
  uint8 enable_mode;
  /*!< Flag indicating, if the WTR feedback device should be configrued in 
    feedback mode or not */
  uint8 fb_state;
  /*! Gain State to which the feedback device is to be configured */

}ftm_cal_feedback_setup_instr_payload_configure_wtr_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold feedback setup RxLM for XPT params
*/
typedef PACK(struct)
{
  uint8 enable_mode;
  /*!< Flag indicating, if the RxLM device should be configrued in 
    feedback mode or not */
}ftm_cal_feedback_setup_instr_payload_configure_rxlm_for_xpt_type;
  
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold feedback setup RxLM for FB AGC params
*/
typedef PACK(struct)
{
  uint8 enable_mode;
  /*!< Flag indicating, if the RxLM device should be configrued in 
    feedback mode or not */
}ftm_cal_feedback_setup_instr_payload_configure_rxlm_for_fbagc_type;  

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of the header for feedback setup parameter
*/
typedef PACK(struct)
{
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

} ftm_cal_feedback_setup_instr_parameter_header_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to hold the feedback setup request packet
*/
typedef PACK(struct)
{
  ftm_cal_feedback_setup_instr_parameter_header_type feedback_setup_header;

  uint8 feedback_setup_payload;

} ftm_cal_feedback_setup_instr_payload_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to hold the unpacked feedback setup parameters
  used to send to tech specific functions
*/
typedef struct
{
  boolean debug_mode_en;
  /*!< Flag indicating debug mode for feedback setup opcode */

  uint16 flag;
  /*!< 16-bit flag indicating a specific type of action to be perform the 
  given payload. */

  rfm_device_enum_type primary_device;
  /*!< Primary Device associated with the feedback path */

  rfm_device_enum_type feedback_device;
  /*!< Feedback device which needs to be configured */

  ftm_calibration_feedback_setup_action_type feedback_setup_action;
  /*!< 16-bit action Mask indicating the type of Feedback config action to be 
    done: Bit 0 = Configure WTR | Bit 1 = Configure RxLM */

  struct
  {
    uint8 enable_mode;
    /*!< Flag indicating, if the WTR feedback device should be configrued in 
      feedback mode or not */
    uint8 fb_state;
    /*! Gain State to which the feedback device is to be configured */

  }configure_wtr;
  /*!< Parameters to configure wtr */

  struct
  {
    uint8 enable_mode;
    /*!< Flag indicating, if the RxLM device should be configured in 
      feedback mode or not */

  }configure_rxlm_for_xpt;
  /*!< Parameters to configure rxlm for xpt*/

  struct
  {
    uint8 enable_mode;
    /*!< Flag indicating, if the RxLM device should be configured in 
      feedback mode or not */

  }configure_rxlm_for_fbagc;
  /*!< Parameters to configure rxlm for fb agc */
}ftm_cal_feedback_setup_parameter_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Request Packet to be used to setup radio for
  calibration v3 sweep
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

  ftm_cal_radio_setup_parameter_type rs_param;
  /*!< Structure containing the Radio Setup parameter. PACKED structure is not 
  used here as the this command is sent only internally */

} ftm_cal_radio_setup_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM Dispatch Request Packet to be used to feedback setup for
  calibration v3 sweep
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

  ftm_cal_feedback_setup_parameter_type fb_param;
  /*!< Structure containing the FeedBack Setup parameter. PACKED structure is not 
  used here as the this command is sent only internally */

} ftm_cal_feedback_setup_req_type;

/*----------------------------------------------------------------------------*/
/*! 
  @details
  Function Pointer definition for Callback function to rear down radio
 
  @details
  Thus function will tear down the radio to Sleep state from whatever is the
  current RF State. This function will be called at the end of Calibration
  Sweep to put Radio back to sleep state.
*/
typedef boolean (*ftm_calibration_cb_radio_tear_down_fp_type)
(
  void
);


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition for Callback function to configure Radio
 
  @details
  This function will configure the radio for calibration. This function
  should enable radio to be in RxTx State with lowest Tx Power. Lowest Tx
  Power is a MUST requirement to prevent the Measurement Box from False trigger.
*/
typedef boolean (*ftm_calibration_cb_config_radio_fp_type)
(
  ftm_cal_instr_payload_config_radio_type const *config_radio
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition for Callback function to retune Radio
 
  @details
  This function will retune the radio to new channel. If a device has diversity
  devices as slave devices, then this function should also retune the slave
  devices.
*/
typedef boolean (*ftm_calibration_cb_retune_radio_fp_type)
(
  ftm_cal_instr_payload_retune_radio_type const *retune_radio
);

/*----------------------------------------------------------------------------*/
/*!<
  @brief
  Function Pointer definition for Callback function to configure Tx waveform
 
  @details
  This funtion will configure Tx Waveform for the given mode. Radio should
  already be in Tx State when this function is called.
*/
typedef boolean (*ftm_calibration_cb_config_tx_wf_fp_fp_type)
(
  ftm_cal_instr_payload_config_tx_wf_type const *config_tx_wf
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition to perform Radio setup
 
  @details
  This function will tech specific function to peform radio setup
 
  @param radio_setup
  Pointer to the data containing payload for Radio Config OpCode
 
  @retval TRUE
  Indicates that the radio setup is successful
 
  @retval FALSE
  Indicates that the radio setup encountered failure.
*/
typedef boolean (*ftm_cal_radio_setup_fp_type)
(
  ftm_cal_radio_setup_parameter_type const *radio_setup
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition to perform Feedback setup
 
  @details
  This function will call tech specific functions to peform Feedback setup
*/

typedef boolean (*ftm_cal_feedback_setup_fp_type)
(
  ftm_cal_feedback_setup_parameter_type const *feedback_setup
);

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Radio Tear down */
typedef struct
{
  ftm_calibration_cb_radio_tear_down_fp_type 
                           oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< Function Pointer definition for Callback function */
} ftm_cal_tear_down_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Radio Config OpCode */
typedef struct
{
  ftm_calibration_cb_config_radio_fp_type 
                           oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function to configure Radio
 
  @details
  This function will configure the radio for calibration. This function
  should enable radio to be in RxTx State with lowest Tx Power. Lowest Tx
  Power is a MUST requirement to prevent the Measurement Box from False trigger.
  */
} ftm_cal_radio_config_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Radio Retune OpCode */
typedef struct
{
  ftm_calibration_cb_retune_radio_fp_type 
                           oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function to retune Radio
 
  @details
  This function will configure the radio for calibration. This function
  should enable radio to be in RxTx State with lowest Tx Power. Lowest Tx
  Power is a MUST requirement to prevent the Measurement Box from False trigger.
  */
} ftm_cal_radio_retune_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Tx waveform OpCode */
typedef struct
{
  ftm_calibration_cb_config_tx_wf_fp_fp_type 
                           oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function to configure waveform
  */
} ftm_cal_tx_waveform_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Radio Retune OpCode */
typedef struct
{
  ftm_cal_radio_setup_fp_type oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function to configure waveform
  */

  ftm_cal_radio_setup_parameter_type up_param;
  /*!< Structure holding all the unpacked payload for radio config opcode */

} ftm_cal_radio_setup_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Feedback Setup OpCode */
typedef struct
{
  ftm_cal_feedback_setup_fp_type oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function to setup the feedback path
  */

  ftm_cal_feedback_setup_parameter_type up_param;
  /*!< Structure holding all the unpacked payload for feedback setup opcode */
} ftm_cal_feedback_setup_data_type;

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tear_down_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tear_down_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tear_down_process
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_radio_config_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_radio_config_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_radio_config_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_radio_config_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_radio_retune_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_radio_retune_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_radio_retune_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_radio_retune_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tx_waveform_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tx_waveform_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_tx_waveform_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tx_waveform_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_radio_setup_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_radio_setup_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_radio_setup_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_radio_setup_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_feedback_setup_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_feedback_setup_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_feedback_setup_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_feedback_setup_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);


#ifdef __cplusplus
}
#endif

#endif /* FTM_CALIBRATION_V3_RADIO_H */


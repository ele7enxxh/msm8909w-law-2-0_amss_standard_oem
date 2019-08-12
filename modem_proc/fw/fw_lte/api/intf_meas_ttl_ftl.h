/*!
  @file
  intf_meas_ttl_ftl.h

  @brief
  This file containd lte LL-ML command id's and parameter structures for 
  serving and neighbor cell measurements, time and frequecy tracking


 
*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_meas_ttl_ftl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/12   wm      Add support of one-shot TTL
===========================================================================*/
#ifndef INTF_MEAS_TTL_FTL_H
#define INTF_MEAS_TTL_FTL_H

#include "intf_common.h"
#include "lte_LL1_ue_types.h"
#include "lte_as.h"
#include "intf_common.h"
/*===========================================================================

      Constants

===========================================================================*/
/*! @brief Number of neighbor cells in the neighbor list maintained by SW. 
*/
#define LTE_LL1_MAX_NCELLS_PER_REQ  16


/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/

/*! @brief Measurements configuration message for serving cell

    @detail This message should be used to customize serving cell measurements 
    as opposed to using the default serving cell measurement configuration.
    This message can also be used to reconfigure serving cell measurements.
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;            ///< Common request parameters

  uint8 alpha_rx_combine;                       ///<  Alpha factor to combine RSRP, RSRQ serving cell measurements across Rx antennas.
                                                ///<  <EM> Range: [0, 1] </EM>
                                                ///<  <EM> Q-factor: </EM>
                                                ///<  <EM> Default value: 0.5 in 5U4 = 8 </EM>

  uint8   threshold_offset_rx[2];               ///<  Linear factor by which threshold (per Rx) is offset. SNE taps within window 
                                                ///<  length are averaged to get a threshold. This threshold is weighted by
                                                ///<  threshold_offset as (threshold_offset * threshold) to obtain the measurement
                                                ///<  threshold.
                                                ///<  <EM> Q-factor: 7U4 </EM>
                                                ///<  <EM> Default value: 3dB = 2 in linear scale = 32 in 7U4 </EM>

  uint16  win_len;                              ///<  Threshold computation windowing length. Same for both Rx antennas.
                                                ///<  <EM> Range: [1, (# SNE taps)] </EM>
                                                ///<  <EM> Default: (Number of SNE taps as a function of BW - CP length in CIR domain) </EM>
} lte_LL1_meas_scell_config_req_struct;

LTE_LL1_INTF_MSG( meas_scell_config_req );


/*---------------------------------------------------------------------------
            CONFIGURATION FOR SERVING CELL TIME TRACKING
---------------------------------------------------------------------------*/
/*! @brief Enum listing transmission mode for TTL calculation. 
    CONN MODE will use different method of TTL calculation

    @detail
*/
typedef enum {
   TTL_FDD_CONN_MODE,
   TTL_FDD_DRX_MODE,
   TTL_TDD_DRX_MODE,
   TTL_TDD_CONN_MODE
}lte_LL1_TTL_conn_mode_enum;
/*! @brief Enum listing configuration parameters for TTL configuration 
    message for serving cell, enabling serving TTL and for all neighbor 
    cells

    @detail
*/
typedef enum {
  LTE_LL1_TTL_SCELL_CONFIG_ENABLE = 0,                    ///<  Enable switch for time tracking module                                                         
                                                          
  LTE_LL1_TTL_SCELL_CONFIG_INNER_LOOP_GAIN,               ///<  Inner loop gain 
                                                          ///<   <EM> Range: 0 ~ 1 </EM>
                                                          ///<   <16U16>
                                                          
  LTE_LL1_TTL_SCELL_CONFIG_INNER_LOOP_STEP_SIZE,          ///<  Inner loop step size 
                                                          ///<   <EM> Range: 1 ~ 128 </EM>
                                                          ///<   <8U0>
                                                          
  LTE_LL1_TTL_SCELL_CONFIG_OUTER_LOOP_GAIN,               ///<  Outer loop gain 
                                                          ///<   <EM> Range: 0 ~ 1 </EM>
                                                          ///<   <16U16>     
                                                          
  LTE_LL1_TTL_SCELL_CONFIG_OUTER_LOOP_STEP_SIZE,          ///<  Outer loop step size 
                                                          ///<   <EM> Range: 1 ~ 128 </EM>
                                                          ///<   <16U16>          

  LTE_LL1_TTL_SCELL_CONFIG_THRESHOLD_FOR_MEAS,            ///<  if the TTL timing offset is greater than
                                                          ///<   this threshold, measurements are 
                                                          ///<   deemed invalid
                                                          ///<   <EM> Range is TBD </EM>

  LTE_LL1_TTL_SCELL_CONFIG_SHIFT_ORIGIN,                  ///<  constant right shift applied to avoid 
                                                          ///<   aliasing of CER due to erroneous FFT 
                                                          ///<   window placement
                                                          ///<   <EM> Range: 0 ~ 512 </EM>

  LTE_LL1_TTL_SCELL_CONFIG_DISC_SHAPE,                    ///<  shape of tracking weights within CP 
                                                          ///<   0: Step function
                                                          ///<   1: Linear (default)

  LTE_LL1_TTL_SCELL_CONFIG_EPSILON,                       ///<  magnitude of tracking weights within CP 
                                                          ///<   <EM> Range: 0 ~ 1 </EM>
                                                          ///<   <EM> Q-factor: 16U15 </EM>

  LTE_LL1_TTL_SCELL_CONFIG_SINGLE_PATH_POSITION,          ///<  measure of asymmetry about the current 
                                                          ///<   window position.
                                                          ///<   x = 0: symmetric
                                                          ///<   x > 0: shift to the right by x
                                                          ///<   x < 0: shift to the left by x
                                                          ///<   <EM> Range: -256 ~ 255 </EM>

  LTE_LL1_TTL_SCELL_CONFIG_ALPHA_THRESHOLD_P1,            ///<  Threshold used to determine combining 
                                                          ///<   weight alpha. The magnitude of the outer 
                                                          ///<  loop discriminant is compared against 
                                                          ///<  this threshold to determine the loop 
                                                          ///<  combining factor alpha. Details in 
                                                          ///<  80-VB236-4 Section 14.1.9
                                                          ///<  <EM> Range: -128 ~ -1 </EM>

  LTE_LL1_TTL_SCELL_CONFIG_ALPHA_THRESHOLD_P2,            ///<  Threshold used to determine combining     
                                                          ///<   weight alpha. The magnitude of the outer 
                                                          ///<  loop discriminant is compared against     
                                                          ///<  this threshold to determine the loop      
                                                          ///<  combining factor alpha. Details in        
                                                          ///<  80-VB236-4 Section 14.1.9
                                                          ///<  <EM> Range: -128 ~ -1 </EM>

  LTE_LL1_TTL_SCELL_CONFIG_ALPHA_THRESHOLD_P3,            ///<  Threshold used to determine combining     
                                                          ///<   weight alpha. The magnitude of the outer 
                                                          ///<  loop discriminant is compared against     
                                                          ///<  this threshold to determine the loop      
                                                          ///<  combining factor alpha. Details in        
                                                          ///<  80-VB236-4 Section 14.1.9  
                                                          ///<  <EM> Range: 1 to 128 </EM>

  LTE_LL1_TTL_SCELL_CONFIG_ALPHA_THRESHOLD_P4,            ///<  Threshold used to determine combining     
                                                          ///<   weight alpha. The magnitude of the outer 
                                                          ///<  loop discriminant is compared against     
                                                          ///<  this threshold to determine the loop      
                                                          ///<  combining factor alpha. Details in        
                                                          ///<  80-VB236-4 Section 14.1.9
                                                          ///<  <EM> Range: 1 to 128 </EM>

  LTE_LL1_TTL_CIR_SHIFT_BIAS,                             ///< Bias dependent on system BW; used to 
                                                          ///< determine starting FFT window position
  LTE_LL1_TTL_CONN_MODE,                                  ///< connet mode for TTL algorithm 
                                                          ///0 => TTL_FDD_CONN_MODE
                                                          ///1 => TTL_FDD_DRX_MODE
                                                          ///2 => TTL_TDD_DRX_MODE
                                                          ///3 => TTL_TDD_CONN_MODE
  LTE_LL1_TTL_SCELL_CONFIG_PARAM_COUNT

}lte_LL1_TTL_SCELL_CONFIG_REQ_PARAM_enum;


/*! @brief TTL configuration message for serving cell 

    @detail This message should be sent before serving cell time tracking 
    module is enabled or for reconfiguration. It can be used as a variable 
    length message containing exclusively, the parameters that need to be 
    modified (different from default value). For example, serving cell TTL can  
    be enabled with default values for all parameters by sending this message 
    with LTE_LL1_TTL_SCELL_CONFIG_SCELL_ENABLE = 1.  
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;            ///< Common request parameters

  uint32 num_params;
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
  lte_LL1_config_element_struct config_params[LTE_LL1_TTL_SCELL_CONFIG_PARAM_COUNT];
} lte_LL1_ttl_scell_config_req_struct;

LTE_LL1_INTF_MSG( ttl_scell_config_req );


/*---------------------------------------------------------------------------
          CONFIGURATION FOR SERVING CELL FREQUENCY TRACKING, RV-AFC
---------------------------------------------------------------------------*/
/*! 
    @brief
    Enum listing the different modes in which RV-AFC module can operate 
 
    @detail
    Example:
    To start RV-AFC module in normal mode,
    Set the LTE_LL1_RVAFC_ENABLE_MASK field in FTL scell config message to 
    value 0x3.
        - bit 0 to enable RV-AFC
        - bit 1 to start in normal mode
 
    The configuration parameters are defined in the FTL scell config message
*/
typedef enum
{
  LTE_LL1_RVAFC_VCO_ONLY = 0,   ///< RV-AFC module disabled. Entire frequency error is applied to VCTCXO
  LTE_LL1_RVAFC_NORMAL_MODE,    ///< Normal mode
  LTE_LL1_RVAFC_ROT_ONLY_MODE,  ///< Rotator only mode
  LTE_LL1_RVAFC_SRL_MODE,       ///< SRL mode
  LTE_LL1_RVAFC_XO_MODE,        ///< XO mode
  LTE_LL1_NUM_RVAFC_MODE        ///< Number of the RVAFC modes 

} lte_LL1_rvafc_mode_enum;

/*! @brief Enum listing configuration parameters for FTL configuration  
    message for serving cell FTL

    @detail 
*/
typedef enum {
  LTE_LL1_FTL_SCELL_CONFIG_ENABLE = 0,                    ///<  Enable switch for frequency tracking module 
  LTE_LL1_FTL_SCELL_CONFIG_INNER_LOOP_GAIN,               ///<   Inner loop gain 
                                                          ///<   <EM> Range: 0 ~ 1 </EM>
                                                          ///<   <Q-factor>   
  LTE_LL1_FTL_SCELL_CONFIG_OUTER_LOOP_GAIN,               ///<   Outer loop gain 
                                                          ///<   <EM> Range: 0 ~ 1 </EM>
                                                          ///<   <Q-factor>       
  LTE_LL1_FTL_SCELL_CONFIG_OUTER_LOOP_STEP_SIZE,          ///<  Outer loop step size 
                                                          ///<   <EM> Range: 1 ~ 10 KHz </EM>
                                                          ///<   <Q-factor>                                             
                                                          ///<  <EM> Range: 1 ~ 5 </EM>                     
  LTE_LL1_FTL_SCELL_CONFIG_ALPHA_THRESHOLD_LOW,           ///<  lower threshold used to determine combining 
                                                          ///<   weight alpha                          
                                                          ///<   <EM> Range: 400 ~ 2000 Hz </EM>                               
  LTE_LL1_FTL_SCELL_CONFIG_ALPHA_THRESHOLD_HIGH,          ///<  higher threshold used to determine combining 
                                                          ///<   weight alpha                          
                                                          ///<   <EM> Range: 800 ~ 2000 Hz </EM>                
  LTE_LL1_RVAFC_MODE,                                     ///<  RV-AFC mode. Specified in lte_LL1_rvafc_mode_enum
  LTE_LL1_RVAFC_INNER_LOOP_GAIN,                          ///<  Inner loop gain of RV-AFC loop
                                                          ///<  <EM> Range: 0 ~ 1 </EM>
                                                          ///<  <EM> Q-factor: </EM>
  LTE_LL1_RVAFC_OUTER_LOOP_GAIN,                          ///<  Outer loop gain of RV-AFC loop
                                                          ///<  <EM> Range: 0 ~ 1 </EM>       
                                                          ///<  <EM> Q-factor: </EM>   
  LTE_LL1_RVAFC_OUTER_LOOP_GAIN_TDD,                      ///<  Outer loop gain of RV-AFC loop
                                                          ///<  <EM> Range: 0 ~ 1 </EM>       
                                                          ///<  <EM> Q-factor: </EM>                                                                    
  LTE_LL1_FTL_SCELL_CONFIG_VCO_SLOPE,                     ///<   VCO slope 
                                                          ///<   <EM> Range: non_zero </EM>
                                                          ///<   <Q-factor>   Q0     
  LTE_LL1_FTL_SCELL_CONFIG_PARAM_COUNT

} lte_LL1_FTL_SCELL_CONFIG_REQ_PARAM_enum;


/*! @brief FTL configuration message for serving cell 

    @detail This message should be sent before serving cell frequency tracking 
    module is enabled or for reconfiguration. It can be used as a variable 
    length message containing exclusively, the parameters that need to be 
    modified (different from default value). For example, serving cell FTL can  
    be enabled with default values for all parameters by sending this message 
    with LTE_LL1_FTL_SCELL_CONFIG_ENABLE = 1.  
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;            ///< Common request parameters

  uint32 num_params;
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
  lte_LL1_config_element_struct config_params[LTE_LL1_FTL_SCELL_CONFIG_PARAM_COUNT];
} lte_LL1_ftl_scell_config_req_struct;

LTE_LL1_INTF_MSG( ftl_scell_config_req );


/*---------------------------------------------------------------------------                

            REQUEST AND CONFIRMATION & ABORT FOR RV-AFC

---------------------------------------------------------------------------*/
/*! 
    @brief 
    Request and confirmation message for ML to get the current VCO and     
    digital rotator settings from the RV-AFC module
*/
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;

} lte_LL1_rvafc_get_freq_corr_req_struct;

LTE_LL1_INTF_MSG( rvafc_get_freq_corr_req );


typedef struct
{
  lte_LL1_cnf_header_struct   cnf_hdr;
  int32                       rot_freq_corr; ///< Total frequency correction applied to the digital rotator in 32S16
  int32	                      vco_freq_corr; ///<  Total frequency correction applied to the VCTCXO in 32S16

} lte_LL1_rvafc_freq_corr_cnf_struct;

LTE_LL1_INTF_MSG( rvafc_freq_corr_cnf );


/*---------------------------------------------------------------------------
        REQUEST AND CONFIRMATION & ABORT FOR SERVING CELL MEASUREMENTS
---------------------------------------------------------------------------*/
/*! @brief  Indicates Rx antennas used

    @detail Indicates Rx antennas used for the following purposes:
            - performing serving cell measurements
            - indicating reliable serving cell measurements in confirmation message
            - performing neighbor cell measurements
            - indicating reliable neighbor cell measurements in confirmation message
*/
typedef enum
{
  LTE_LL1_RX_ANT_MASK_UNDEFINED = 0,  ///<  Undefined
  LTE_LL1_RX_ANT_0,                   ///<  Rx ant 0
  LTE_LL1_RX_ANT_1,                   ///<  Rx ant 1
  LTE_LL1_RX_ANT_0_1                  ///<  Both Rx ant 0, 1

} lte_LL1_rx_ant_mask_enum;


/*! @brief  Indicates number of Tx antennas used for neighbor cell measurements, tracking.

    @note   Number of Tx antennas capped at 2 in frequency tracking FW module. FTL supports max. 2x2 only
*/
typedef enum
{
  LTE_LL1_TX_ANT_MASK_UNDEFINED = 0,  ///<  Undefined
  LTE_LL1_TX_ANT_0,                   ///<  eNb Tx ant 0 used (1 Tx antenna case)
  LTE_LL1_TX_ANT_0_1,                 ///<  eNb Tx ant 0, 1 used (2 Tx antennas case)
  LTE_LL1_TX_ANT_0_1_2_3              ///<  eNb Tx ant 0, 1, 2, 3 used (3 Tx antennas case)

} lte_LL1_num_tx_ant_enum;


/*! @brief  Request message for serving cell measurements

    @detail This message is sent every time a serving cell measurement 
    is performed. Instantaneous RSRP measurements computed in FW are always
    averaged over all Tx antennas but reported per Rx antenna. The number 
    of Rx antennas over which RSRP is measured is however left to the 
    discretion of ML.
*/ 
typedef struct
{

  lte_earfcn_t  earfcn;   ///< Cell EARFCN for logging / debugging.
                          ///< Range: 0 ~ 65535

  uint16        cell_id;  ///<  Serving cell ID
  
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA

} lte_LL1_serving_cell_meas_info_struct;


/*! @brief CA message 
*/
typedef struct {

  lte_LL1_req_header_struct req_hdr;            ///<  Common request parameters
  
  uint8  num_carriers; ///< Number of carrier for serving cell  measurement report
  
  lte_LL1_serving_cell_meas_info_struct  carrier[LTE_LL1_DL_NUM_CARRIERS]; ///< 

}lte_LL1_meas_scell_req_struct;

LTE_LL1_INTF_MSG( meas_scell_req );


typedef struct {
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA

  lte_earfcn_t  earfcn;               ///< Cell EARFCN for logging / debugging.
                                      ///< Range: 0 ~ 65535

  uint16  cell_id;                    ///< Serving cell ID
 
  uint16  meas_sfn;                   //< System frame number for the subframe whose samples are used to do measurement.
                                      ///< Range: 0 ~ 1023

  uint16  meas_subfn;                 ///< Subframe number for the subframe whose samples are used to do measurement.
                                      ///< Range: 0 ~ 1023             
                                      
  boolean is_restricted;              ///< Flag to indicate if this measurement is a restricted measurement.
                                      
  uint32  cell_timing[2];             ///< Cell timing per Rx antenna as a modulo 10 ms offset in a radio frame.
                                      ///< Range: 0 ~ 307199

  uint16  cell_timing_sfn[2];         ///< SFN per Rx antenna corresponding to the cell_timing value.
                                      ///< Range: 0 ~ 1023

  int32   scell_timing_offset_rx[2];  ///< @todo !!!!!!!!!!!!!!!!!! REMOVE ONCE ML REMOVES DEPENDENCY ON THIS VARIABLE !!!!!!!!!!!!!!!!!!!


  int32   inst_rsrp_rx[2];  ///<  Instantaneous RSRP value per Rx antenna. Combined across Tx antennas. In dBm.
                            ///<  <EM> Q-factor: Currently 32S23. </EM>
                            
  int32   inst_rsrp;        ///<  Instantaneous RSRP value combined across Tx-Rx pairs. In dBm.
                            ///<  <EM> Q-factor: Currently 32S23. </EM>

  int32   inst_rsrq_rx[2];  ///<  Instantaneous RSRQ value per Rx antenna. Combined across Tx antennas. In dBm.
                            ///<  <EM> Q-factor: 32S24 </EM>
  int32   inst_rsrq_p_rx[2];///<  Instantaneous RSRQ value per Rx antenna. Combined across Tx antennas. In dBm.                          
                            ///<  Instantaneous RSRQ value combined across Tx-Rx pairs. In dBm.
  int32   inst_rsrq;        ///<  <EM> Q-factor: 32S24 </EM>                                                   
                            ///<  <EM> Q-factor: 32S24 </EM>
  int32   inst_rsrq_p;      ///<  Instantaneous proper RSRQ value combined across Tx-Rx pairs. In dBm. 
                            ///<  <EM> Q-factor: 32S24 </EM>                                    
  int32   inst_rssi_rx[2];  ///<  RSSI value per Rx antenna. In dBm. Used for debugging.
                            ///<  <EM> Q-factor: 32S24 </EM>
  int32   inst_rssi_p_rx[2];///<  proper RSSI value per Rx antenna. In dBm. Used for debugging.
                            ///<  <EM> Q-factor: 32S24 </EM>                            
  int32   inst_rssi;        ///<  RSSI value combined across Rx antennas. In dBm. Used for debugging.
                            ///<  <EM> Q-factor: 32U24 </EM>
  int32   inst_rssi_p;        ///<  RSSI value combined across Rx antennas. In dBm. Used for debugging.
                            ///<  <EM> Q-factor: 32U24 </EM>
  int32   dvga_rx_log[2];     ///< DVGA gain per Rx antenna in log domain. Q-factor: 32S24

  int32   lna_gain_rx_log[2]; ///< LNA gain per Rx antenna in log domain. Q-factor: 32S24

  /*! Additioanl fields for IOT */
  uint16  cqi_wb;             ///< Wideband CQI
  int16   res_freq_error;     ///< Residual frequency error

  uint32  filt_snr_rx[2];     ///< Filtered SNR per Rx antenna. Linear.
                              ///< <EM> Q-factor: 32U16 </EM>
  
  int32	  filt_rssi_rx[2];    ///< Filtered RSSI per Rx antenna. In dBm.
                              ///< <EM> Q-factor: 32S24 </EM>

  uint32  geometry_est[2];    ///< Geometry estimation per Rx in linear scale.
                              ///< <EM> Q-factor: 32U16 </EM>
	
	uint16 doppler_measurement; ///< Doppler frequency

  boolean  interf_active;     ///<Result is interfered by other tech

  int32  cinr_per_rx[2]; ///<CINR in dynamic antenna mapping algorithm @ ML1
  int32  rx_sig_correlation[LTE_LL1_DUAL_DEMOD_NUM_ANT_CORRELATION];  ///<result in the order of vpe rx: (0,1),(0,2),(0,3),(1,2),(1,3),(2,3)
  int32  rx_int_correlation[LTE_LL1_DUAL_DEMOD_NUM_ANT_CORRELATION];  ///<result in the same order as rx_sig_correlation
  boolean correlation_is_valid;
}lte_LL1_serving_cell_meas_result_struct;


/*! @brief  Confirmation message for serving cell measurements

    @detail The measurement results of the serving cell are reported after 
    completing processing for the ML serving cell request msg 
    lte_LL1_meas_scell_req_msg_struct. The values are reported per Rx antenna.
*/
typedef struct {    

  lte_LL1_cnf_header_struct cnf_hdr;  ///< Common confirmation message parameters
  
  uint8  num_carriers; ///< Number of carrier for serving cell  measurement report
  
  lte_LL1_serving_cell_meas_result_struct  carrier[LTE_LL1_DL_NUM_CARRIERS]; ///<
}lte_LL1_meas_scell_cnf_struct;
  

LTE_LL1_INTF_MSG( meas_scell_cnf );


/*-----------------------------------------------------------------------------------
        REQUEST, CONFIRMATION & ABORT FOR NEIGHBOR CELL MEASUREMENTS & TRACKING
-----------------------------------------------------------------------------------*/
/*! @brief  Neighbor cell measurement, tracking request message parameters

    @detail This data structure contains all ML maintained parameters required 
    to perform neighbor measurements, tracking in FW and HW. An array of this
    data structure, where each element corresponds to a neighbor cell, is used 
    in the request message
*/
typedef struct {
                                                                                                                    
  uint8  meas_duration;           ///<  Duration (in ms) over which ncell measurement is averaged
                                              ///<  <EM> Range: 1~25 subframes (or ms) </EM>

  lte_LL1_rx_ant_mask_enum  rx_ant_mask;      ///<  Rx antenna bit mask used to indicate Rx antennas used for measurements
                                              ///<  Bit 0: Rx ant 0; Bit 1: Rx ant 1                             

  lte_l1_cp_e   cp_type;          ///<  Indicate neighbor cell cyclic prefix

  lte_LL1_num_tx_ant_enum  num_tx_ant;        ///<  number of Tx antennas used for measuring / tracking neighbor cell
                                              ///<  00: Not defined, 01: 1 Tx antenna, 10: 2 Tx antennas, 11: 4 Tx antennas,                                            

  uint16  ncell_id;               ///<  Neighbor cell ID for descrambling RS.                            
                                                                                                         
  uint32  ttl_ref_time_rx_ant0;    ///<  Reference time to be applied for Rx ant 0 in sample 
                                                                ///<  server for each neighbor cell in the list.
																	///<	modulo 10 ms of the 64-bit value stored in ML is 
                                                                ///<  sent to LL. LL computes the SFN and performs the 
                                                                ///<  translation to the 7 ms searcher sample server and 
                                                                ///<  1.5 symbols sized symbol buffer. <Q-factor>

  uint32  ttl_ref_time_rx_ant1;    ///<  Reference time to be applied for Rx ant 1 in sample 
                                                                ///<  server for each neighbor cell in the list.          
                                                                ///<  modulo 10 ms of the 64-bit value stored in ML is    
                                                                ///<  sent to LL. LL computes the SFN and performs the    
                                                                ///<  translation to the 7 ms searcher sample server and  
                                                                ///<  1.5 symbols sized symbol buffer. <Q-factor>                    

  int16  ftl_ref_offset;          ///<  reference frequency offsets to be applied in sample 
                                                                ///<  server for each neighbor cell in the list. 
                                                                ///<  ftl_ref_offset[i] corresponds to neighbor cell 'i'

} lte_LL1_meas_tracking_ncell_req_element_struct;


/*! @brief  Request message for neighbor cell for measurements, tracking

    @detail This message is sent everytime neighbor cell measurements / 
    tracking or both are performed. Up to LTE_LL1_MAX_NCELLS_PER_REQ = 16  
    can be measured, tracked using one request message.
*/
typedef struct {
  lte_LL1_req_header_struct                           req_hdr;    ///<  Common request parameters
  uint16                                              num_ncells; ///<  Number of neighbor cells to measure / track.                     
                                                                  ///<  <EM> Range: 1~16 neighbor cells </EM>
  lte_LL1_meas_tracking_ncell_req_element_struct      meas_track_ncell_req_params_arr[LTE_LL1_MAX_NCELLS_PER_REQ];
                                                                  ///<  Measurements, tracking request parameters for all neighbors

} lte_LL1_meas_tracking_ncell_req_struct;

LTE_LL1_INTF_MSG( meas_tracking_ncell_req );


/*! @brief  Neighbor cell measurement, tracking confirmation message 
    parameters

    @detail This data structure contains all ML maintained parameters required 
    to report results on completion of neighbor measurements, tracking in FW 
    and HW. An array of this data structure, where each element corresponds to 
    a neighbor cell, is used in the confirmation message
*/
typedef struct {

  boolean   ttl_valid;        ///<  flag indicating validity of TTL update

  boolean   ftl_valid;        ///<  flag indicating validity of FTL update
                            
  lte_LL1_rx_ant_mask_enum   meas_rx_ant_mask;  ///<  Rx antenna bit mask used to indicate reliable Rx antennas for measurements
                              ///<  Bit 0: Rx ant 0; Bit 1: Rx ant 1                                                   

  uint16  ncell_id;                   ///<  Neighbor cell ID used for descrambling RS symbols.
                                      ///<  Used in CNF for ML to identify neighbor cell

  int16  ttl_delta_ref_time_rx_ant0;  ///<  Neighbor cell TTL update for rx ant 0

  int16  ttl_delta_ref_time_rx_ant1;  ///<  Neighbor cell TTL update for rx ant 1

  int16  ftl_delta_freq_offset;       ///<  Neighbor cell FTL update

  uint32  inst_rsrp_rx0;              ///<  Neighbor cell instantaneous RSRP value for Rx ant 0
                                      ///<  Q(m+9).n based on demod path. In linear scale.
                                                                                                                                       
  uint32  inst_rsrp_rx1;              ///<  Neighbor cell instantaneous RSRP value for Rx ant 1
                                      ///<  Q(m+9).n based on demod path. In linear scale.
                                                                    
  uint32  inst_rsrq_rx0;              ///<  Neighbor cell instantaneous RSRQ value for Rx ant 0
                                                                    
  uint32  inst_rsrq_rx1;              ///<   Neighbor cell instantaneous RSRQ value for Rx ant 1.
                                      ///<   In linear scale.
                                                                    
  uint32  inst_rssi;                  ///<   Neighbor cell instantaneous RSSI value
                                                                    ///<  Same for both antennas. In linear scale.

} lte_LL1_meas_tracking_ncell_cnf_element_struct;


/*! @brief  Confirmation message for neighbor cell measurements, tracking

    @detail This report is sent after all neighbor cell measurements & 
    tracking as requested by ML in the request msg 
    lte_LL1_ncell_meas_tracking_req_msg_struct.
*/
typedef struct {
  lte_LL1_cnf_header_struct                           cnf_hdr;    ///< Common confirmation message parameters
  uint16                                              num_ncells; ///<  Number of neighbor cells that were measured / tracked
                                                                  ///<  <EM> Range: 1~16 neighbor cells </EM>
  lte_LL1_meas_tracking_ncell_cnf_element_struct      meas_track_ncell_cnf_params_arr[LTE_LL1_MAX_NCELLS_PER_REQ];
                                                                  ///<  Measurements, tracking confirmation parameters for all neighbors

} lte_LL1_meas_tracking_ncell_cnf_struct;

LTE_LL1_INTF_MSG( meas_tracking_ncell_cnf );


/*! @brief Message to abort neighbor cell measurements / tracking modules.

    @detail This message is sent when neighbor cell measurements & tracking  
    is aborted. This message is not used unless there is a need for an 
    abortive procedure. Measurements and tracking of all neighbors 
    in the corresponding neighbor request message are aborted.
*/
typedef struct
{
  lte_LL1_req_header_struct req_hdr;          ///< Common request parameters

} lte_LL1_meas_tracking_ncell_abort_req_struct;

LTE_LL1_INTF_MSG( meas_tracking_ncell_abort_req );


/*! @brief Abort confirmation sent on receiving an abort neighbor cell 
    measurements & tracking message

    @detail This serves as a confirmation to ML that neighbor cells  
    measurements & tracking were aborted on receipt of an abort request 
    from ML.
*/
typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;                    ///<  Common confirmation message parameters

} lte_LL1_meas_tracking_ncell_abort_cnf_struct;

LTE_LL1_INTF_MSG( meas_tracking_ncell_abort_cnf );





//----------------------------------------------------------------------------------------------
// !!!!!!!!!!!!!!! NEW MESSAGES FOR NEIGHBOR CELL MEASUREMENTS, TTL, FTL !!!!!!!!!!!!!!
//----------------------------------------------------------------------------------------------
/*! @brief Enum listing configuration parameters for neighbor cell 
    measurements and tracking.

    @detail
    One time message sent before neighbor cell measurements and tracking and 
    performed for the first time
*/
typedef enum {

  LTE_LL1_MEAS_NCELL_ALPHA_RX_COMBINE,  ///<  Alpha factor to combine RSRP, RSRQ serving cell measurements across Rx antennas.
                                        ///<  <EM> Range: [0, 1] </EM>
                                        ///<  <EM> Q-factor: </EM>
                                        ///<  <EM> Default value: 0.5 in 5U4 = 8 </EM>

  LTE_LL1_MEAS_NCELL_THRESHOLD_OFFSET_1TX_6RBs,  ///<  Linear factor by which threshold for 1 Tx antenna is offset for 6 RB measurement. 
                                                 ///<  SE taps within window  length are averaged to get a threshold. This threshold is weighted by
                                                 ///<  threshold_offset as (threshold_offset * threshold) to obtain the measurement
                                                 ///<  threshold. This threshold is the same for both Rx antennas.
                                                 ///<  <EM> Q-factor: 7U4 </EM>
                                                 ///<  <EM> Default value: 5.5dB = 3.6 in linear scale = 57 in 7U4 </EM>

  LTE_LL1_MEAS_NCELL_THRESHOLD_OFFSET_2TX_6RBs,  ///<  Linear factor by which threshold for 2 Tx antennas is offset for 6 RB measurement.  
                                                 ///<  SE taps within window length are averaged to get a threshold. This threshold is weighted by
                                                 ///<  threshold_offset as (threshold_offset * threshold) to obtain the measurement
                                                 ///<  threshold. This threshold is the same for both Rx antennas.
                                                 ///<  <EM> Q-factor: 7U4 </EM>
                                                 ///<  <EM> Default value: 4.5dB = 2.8 in linear scale = 45 in 7U4 </EM>

  LTE_LL1_MEAS_NCELL_THRESHOLD_OFFSET_1TX_8RBs,  ///<  Linear factor by which threshold for 1 Tx antenna is offset for 8 RB measurement. 
                                                 ///<  SE taps within window  length are averaged to get a threshold. This threshold is weighted by
                                                 ///<  threshold_offset as (threshold_offset * threshold) to obtain the measurement
                                                 ///<  threshold. This threshold is the same for both Rx antennas.
                                                 ///<  <EM> Q-factor: 7U4 </EM>
                                                 ///<  <EM> Default value: 6.0dB = 4.0 in linear scale = 64 in 7U4 </EM>

  LTE_LL1_MEAS_NCELL_THRESHOLD_OFFSET_2TX_8RBs,  ///<  Linear factor by which threshold for 2 Tx antennas is offset for 8 RB measurement.  
                                                 ///<  SE taps within window length are averaged to get a threshold. This threshold is weighted by
                                                 ///<  threshold_offset as (threshold_offset * threshold) to obtain the measurement
                                                 ///<  threshold. This threshold is the same for both Rx antennas.
                                                 ///<  <EM> Q-factor: 7U4 </EM>
                                                 ///<  <EM> Default value: 5.0dB = 3.2 in linear scale = 51 in 7U4 </EM>

  LTE_LL1_MEAS_NCELL_WIN_LEN, ///<  Threshold computation windowing length. Same for both Rx antennas.
                              ///<  <EM> Range: [1, (# SNE taps)] </EM>
                              ///<  <EM> Default: (Number of SNE taps as a function of BW - CP length in CIR domain) </EM>
                                                          
  LTE_LL1_TTL_NCELL_CONFIG_INNER_LOOP_GAIN, ///<  Inner loop gain 
                                            ///<   <EM> Range: 0 ~ 1 </EM>
                                            ///<   <16U16>
                                                          
  LTE_LL1_TTL_NCELL_CONFIG_INNER_LOOP_STEP_SIZE,  ///<  Inner loop step size 
                                                  ///<   <EM> Range: 1 ~ 128 </EM>
                                                  ///<   <8U0>
                                                  
  LTE_LL1_TTL_NCELL_CONFIG_OUTER_LOOP_GAIN, ///<  Outer loop gain 
                                            ///<   <EM> Range: 0 ~ 1 </EM>
                                            ///<   <16U16>     
                                                  
  LTE_LL1_TTL_NCELL_CONFIG_OUTER_LOOP_STEP_SIZE,  ///<  Outer loop step size 
                                                  ///<   <EM> Range: 1 ~ 128 </EM>
                                                  ///<   <16U16>          

  LTE_LL1_TTL_NCELL_CONFIG_THRESHOLD_FOR_MEAS,  ///<  if the TTL timing offset is greater than
                                                ///<   this threshold, measurements are 
                                                ///<   deemed invalid
                                                ///<   <EM> Range is TBD </EM>

  LTE_LL1_TTL_NCELL_CONFIG_SHIFT_ORIGIN,  ///<  constant right shift applied to avoid 
                                          ///<   aliasing of CER due to erroneous FFT 
                                          ///<   window placement
                                          ///<   <EM> Range: 0 ~ 512 </EM>

  LTE_LL1_TTL_NCELL_CONFIG_DISC_SHAPE,  ///<  shape of tracking weights within CP 
                                        ///<   0: Step function
                                        ///<   1: Linear (default)

  LTE_LL1_TTL_NCELL_CONFIG_EPSILON, ///<  magnitude of tracking weights within CP 
                                    ///<   <EM> Range: 0 ~ 1 </EM>
                                    ///<   <EM> Q-factor: 16U15 </EM>

  LTE_LL1_TTL_NCELL_CONFIG_SINGLE_PATH_POSITION,  ///<  measure of asymmetry about the current 
                                                  ///<   window position.
                                                  ///<   x = 0: symmetric
                                                  ///<   x > 0: shift to the right by x
                                                  ///<   x < 0: shift to the left by x
                                                  ///<   <EM> Range: -256 ~ 255 </EM>

  LTE_LL1_TTL_NCELL_CONFIG_ALPHA_THRESHOLD_P1,  ///<  Threshold used to determine combining 
                                                ///<   weight alpha. The magnitude of the outer 
                                                ///<  loop discriminant is compared against 
                                                ///<  this threshold to determine the loop 
                                                ///<  combining factor alpha. Details in 
                                                ///<  80-VB236-4 Section 14.1.9
                                                ///<  <EM> Range: -128 ~ -1 </EM>

  LTE_LL1_TTL_NCELL_CONFIG_ALPHA_THRESHOLD_P2,  ///<  Threshold used to determine combining     
                                                ///<   weight alpha. The magnitude of the outer 
                                                ///<  loop discriminant is compared against     
                                                ///<  this threshold to determine the loop      
                                                ///<  combining factor alpha. Details in        
                                                ///<  80-VB236-4 Section 14.1.9
                                                ///<  <EM> Range: -128 ~ -1 </EM>

  LTE_LL1_TTL_NCELL_CONFIG_ALPHA_THRESHOLD_P3,  ///<  Threshold used to determine combining     
                                                ///<   weight alpha. The magnitude of the outer 
                                                ///<  loop discriminant is compared against     
                                                ///<  this threshold to determine the loop      
                                                ///<  combining factor alpha. Details in        
                                                ///<  80-VB236-4 Section 14.1.9  
                                                ///<  <EM> Range: 1 to 128 </EM>

  LTE_LL1_TTL_NCELL_CONFIG_ALPHA_THRESHOLD_P4,  ///<  Threshold used to determine combining     
                                                ///<   weight alpha. The magnitude of the outer 
                                                ///<  loop discriminant is compared against     
                                                ///<  this threshold to determine the loop      
                                                ///<  combining factor alpha. Details in        
                                                ///<  80-VB236-4 Section 14.1.9
                                                ///<  <EM> Range: 1 to 128 </EM>

  LTE_LL1_FTL_NCELL_CONFIG_INNER_LOOP_GAIN, ///<  Inner loop gain
                                            ///<  <EM> Range: 0 ~ 1 </EM>
                                            ///<  <Q-factor>                   

  LTE_LL1_FTL_NCELL_CONFIG_OUTER_LOOP_GAIN, ///<  Outer loop gain
                                            ///<  <EM> Range: 0 ~ 1 </EM>
                                            ///<  <Q-factor>                   

  LTE_LL1_FTL_NCELL_CONFIG_OUTER_LOOP_STEP_SIZE,  ///<  Outer loop step size (frequency offset for 
                                                  ///<  S-F hypotheses)
                                                  ///<  <EM> Range: 1 ~ 10 KHz </EM>             

  LTE_LL1_FTL_NCELL_CONFIG_ALPHA_THRESHOLD_LOW, ///<  lower threshold used to determine combining       
                                                ///<  weight alpha                                      
                                                ///<  <EM> Range: 400 ~ 2000 Hz </EM>                      

  LTE_LL1_FTL_NCELL_CONFIG_ALPHA_THRESHOLD_HIGH,  ///<  higher threshold used to determine combining      
                                                  ///<  weight alpha                                      
                                                  ///<  <EM> Range: 800 ~ 2000 Hz </EM>                      

  LTE_LL1_MEAS_TTL_FTL_NCELL_CONFIG_PARAM_COUNT

}lte_LL1_MEAS_TTL_FTL_NCELL_CONFIG_REQ_PARAM_enum;


/*! @brief Measurements and tracking configuration message for all neighbor 
    cells

    @detail This message should be sent before neighbor cell measurements and 
    tracking are performed or for reconfiguring neighbor cell measurements and 
    tracking.
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;            ///< Common request parameters

  uint32 num_params;
  lte_LL1_config_element_struct config_ncell_params[LTE_LL1_MEAS_TTL_FTL_NCELL_CONFIG_PARAM_COUNT];
} lte_LL1_meas_ttl_ftl_ncell_config_req_struct;

LTE_LL1_INTF_MSG( meas_ttl_ftl_ncell_config_req );


/*! @brief  Neighbor cells measurement, tracking request message parameters
*/
#define MAX_NCELL_TIERS 3
#define MAX_NUM_POSSIBLE_T_AVAILS 6
#define MAX_NUM_SP_PER_MP 4

/*! @brief When MBSFN is scheduled, inidicate which SF in radio frame is 
    processed for each neighbor cell in each tier
*/
typedef enum
{
  LTE_LL1_MEAS_TTL_FTL_NCELL_OCCASION_FDD_0_TDD_0 = 0,
  LTE_LL1_MEAS_TTL_FTL_NCELL_OCCASION_FDD_5_TDD_5,
  LTE_LL1_MEAS_TTL_FTL_NCELL_OCCASION_FDD_4_TDD_6,
  LTE_LL1_MEAS_TTL_FTL_NCELL_OCCASION_FDD_9_TDD_1

} lte_LL1_meas_ttl_ftl_ncell_occasion_enum;

typedef struct
{
  uint8   ap;       ///< Averaging period in ms when there is no LTE measurement gap and 
                    ///< T_avail is 40, 33, 28, 27 and 26 ms respectively
  uint8   N;        ///< Number of neighbor cells in current tier when there is no LTE measurement 
                    ///< gap and T_avail is 40, 33, 28, 27 and 26 ms respectively    

} lte_LL1_meas_ttl_ftl_ncell_tier_ap_N_t;

/*! Neighbor tier specific parameters used in connected mode */
typedef struct
{
  uint8   ip;         ///< IIR filtering period in ms- time after which the IIR filter state is updated
  // 4/5/2013, Amit Mahajan, ML1 set it to 128, which means no filter across SP, we can remove iir_coeff
  // it was originally intended to do the alpha = 1, 1/2, 1/3, 1/4 to average 4 SP
  uint8   iir_coeff[MAX_NUM_SP_PER_MP]; ///< IIR filtering coefficient (0 ~ 1)
                                        ///< <EM> Q-factor: 8U7 </EM>
  lte_LL1_meas_ttl_ftl_ncell_tier_ap_N_t  ap_N[MAX_NUM_POSSIBLE_T_AVAILS]; ///<.

} lte_LL1_meas_ttl_ftl_ncell_tier_struct;

/*! @brief 
  Enum for measurement set periodicity [36.331]
  MeasSubframePattern-r10 ::= CHOICE {
	  subframePatternFDD-r10		  BIT STRING (SIZE (40)),
	  subframePatternTDD-r10		  CHOICE {
		  subframeConfig1-5-r10		      BIT STRING (SIZE (20)),
		  subframeConfig0-r10		      BIT STRING (SIZE (70)),
		  subframeConfig6-r10		      BIT STRING (SIZE (60)),
		...
	},
*/
typedef enum
{
 LTE_LL1_MEAS_SET_PERIOD_INVALID = 0,
 LTE_LL1_MEAS_SET_PERIOD_FDD_40 = 40,
 LTE_LL1_MEAS_SET_PERIOD_TDD_20 = 20,
 LTE_LL1_MEAS_SET_PERIOD_TDD_70 = 70,
 LTE_LL1_MEAS_SET_PERIOD_TDD_60 = 60,
} lte_LL1_meas_set_period_enum;

/*! 
  @brief The MeasSubframePattern IE is used to specify time domain measurement resource restriction. The first/leftmost bit corresponds to the subframe #0 of the radio frame satisfying SFN mod x = 0, where SFN is that of PCell and x is the size of the bit string divided by 10. "1" denotes that the corresponding subframe is used for measurement.
*/
typedef struct
{
  // set bitmask, MSB aligned: 
  //   - bitmap[0]:bit(31): mask for first subframe(oldest) in the period: (sfn*10+sfi)%period = 0
  //   - bitmap[0]:bit(30): mask for second oldest  subframe  in the period: (sfn*10+sfi)%period = 1
  //   - bitmap[1]:bit(31): mask the subframe in the period that satisfy: (sfn*10+sfi)%period = 32
  uint32 bitmap[LTE_L1_MAX_MEAS_SUBFM_PATTERN_SIZE]; 
                    
  lte_LL1_meas_set_period_enum period;  ///< set period. 
}lte_LL1_meas_subframe_pattern_struct;

/*!
    @brief 
    Neighbor cell measurements & tracking connected mode parameters

    @detail
    Contains connected mode parameters:
      - common to all neighbor cells
      - tier specific parameters
*/
typedef struct
{
  uint8     num_sp_per_mp;  ///< Number of scheduling periods per measurement period
                            ///< Measurement period = sp * num_sp_per_mp

  uint8     first_sp_offset_due_to_gap; ///< Due to an LTE gap, upt to 7ms of samples are streamed 
                                        ///< in the presence of an MBSFN schedule and in TDD before task 
                                        ///< scheduling for connected mode neighbor cell 
                                        ///< measurements.
                                        ///< Range: 0 ~ 7 (in ms)

  uint8     gap_duration;   ///< Duration of measurement gap in ms
  uint8     gap_mask;       ///< One bit per scheduling period.
                            ///< 0: No gap
                            ///< 1: Measurement gap in the end of scheduling period  
  uint8     swrp;           ///< SW reporting period in ms
  uint8     sp;             ///< Scheduling period in ms
  // was set by ML1 before, now FW compute, 1 or 2; this is ML1/LL1 interface, so a bad place to put it!
  uint8     num_tiers;      ///< Number of neighbor cell tiers
  /* only valid when num_sp_per_mp = 1; ML1 will set T_avail = 0 when num_sp_per_mp > 1;
     T_avail is the real duration available for measurement, so need to subtract both gap_duration & first_sp_offset_due_to_gap */
  uint8     T_avail;        ///< ML1 specify directly; FW used to compute it for each SP based on gap pattern
  lte_LL1_meas_ttl_ftl_ncell_tier_struct    tier[MAX_NCELL_TIERS];  ///< Tier specific parameters

  boolean use_crsic_fd_buffer; ///< whether to use NB sample buffer of CRS-IC FD buffer?
  lte_LL1_meas_subframe_pattern_struct measSfPat; ///< restricted SF pattern for restricted cells

} lte_LL1_meas_ttl_ftl_ncell_conn_struct;

/*!
    @brief 
    Neighbor cell measurements & tracking connected mode DRX parameters

    @detail
    Contains connected mode parameters:
      - tier specific parameters
*/
typedef struct
{
  lte_LL1_meas_ttl_ftl_ncell_tier_ap_N_t  ap_N[MAX_NUM_POSSIBLE_T_AVAILS]; ///<.

  ///< Add parameters here

} lte_LL1_meas_ttl_ftl_ncell_cdrx_tier_struct;


typedef struct
{
  uint8                                         sp;                     ///< Scheduling period in ms
  uint8     																		gap_duration;   				///< Duration of measurement gap in ms
  uint8                                         num_tiers;              ///< Number of neighbor cell tiers. Maximum value=3.
  lte_LL1_meas_ttl_ftl_ncell_cdrx_tier_struct   tier[MAX_NCELL_TIERS];  ///< Tier specific parameters

} lte_LL1_meas_ttl_ftl_ncell_cdrx_struct;

/*! @brief Cell specific parameters for all measurements & tracking. Used in  
    neighbor cell measurements & tracking.
*/
typedef struct
{
  uint32   frame_bndry_ref_time_rx[2];  ///<  Neighbor cell frame boundary per Rx antenna as a modulo 10ms offset.
                                        ///<  Including neighbor cell TTL adjustment.
  int16   total_timing_adj_cir[2];  ///< p_current_IL in Ody Sys DD chap 14. Total timing adjustment in the CIR domain.   
                                    ///< Initial value should be 0.
} lte_LL1_ttl_ncell_input_t;

typedef struct
{
  int16 ftl_cumu_freq_offset; ///< Cumulative frequency offset in Hz
                              ///< Send 0 Hz if no offset is maintained in ML
                              ///< FW does not need to know if frequency correction was obtained
                              ///< from searcher's frequency offset estimation or FTL
} lte_LL1_ftl_ncell_input_t;

typedef struct
{
  int16         cell_id;    ///< Cell ID
  lte_l1_cp_e   cp_type;    ///< Normal / extended CP
  uint8         eNb_tx_ant; ///< Number of transmit antennas used by cell's eNb
/*
  ///< @todo Uncomment later once connected mode is ready for change

  uint8         iir_coeff;        ///< IIR filtering coefficient (0 ~ 1)
                                  ///< <EM> Q-factor: 8U7 </EM>
                                  ///< If this cell is newly inserted to be measured in this sp, the iir coeff is set
                                  ///< to 128 (ie. 1) to use instantaneous value
*/

  boolean       ttl_enable; ///< Enable TTL for this neighbor cell
  boolean       ftl_enable; ///< Enable FTL for this neighbor cell

  lte_LL1_ttl_ncell_input_t ttl;  ///<. 
  lte_LL1_ftl_ncell_input_t ftl;  ///<.

  boolean restricted;       // restricted cell or not? must use restricted SF pattern

} lte_LL1_meas_ttl_ftl_ncell_req_list_struct;

/*! @brief For CA
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;    ///< Common request parameters
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
  lte_LL1_nb_id_enum        nb_id;      ///< Enum for NB id mapping
  uint8                     num_ncells; ///< Total number of neighbor cells that are measured and tracked
  uint8                     num_rx_ant; ///< Number of UE Rx antennas. Common to all neighbor cells
  lte_earfcn_t              earfcn;     ///< Cell EARFCN for logging / debugging.
                                        ///< Range: 0 ~ 65535
  boolean                   bw_is_atleast_3mhz; ///< TRUE: Bandwidth of neighbors is at-least 15 RBs (3 MHz)
                                                ///< FALSE: otherwise
  lte_l1_cell_duplex_e      duplexing_mode;     ///< duplexing mode
  boolean                   is_mbsfn_present;   ///<  flag indicating presence of MBSFN scheduling 
  lte_LL1_meas_ttl_ftl_ncell_conn_struct      conn; ///< Parameters common to all neighbor cells in connected mode 
  lte_LL1_meas_ttl_ftl_ncell_occasion_enum    occasion; ///< SFs 0,4,5 or 9 in FDD mode for nbr processing; SFs 0,1,5 or 6 in TDD
  lte_LL1_meas_ttl_ftl_ncell_req_list_struct  cell_list[LTE_LL1_MAX_NCELLS_PER_REQ];  ///< Neighbor cell specific parameters
                                                                                      ///< Measurements, tracking request parameters for each neighbor cell
} lte_LL1_meas_ttl_ftl_ncell_conn_req_struct;

LTE_LL1_INTF_MSG( meas_ttl_ftl_ncell_conn_req );


/*!
    @brief
    Neighbor measurements & tracking connected DRX mode request message
*/
typedef struct
{
  int16         cell_id;    ///< Cell ID
  lte_l1_cp_e   cp_type;    ///< Normal / extended CP
  uint8         eNb_tx_ant; ///< Number of transmit antennas used by cell's eNb

  uint8         iir_coeff;        ///< IIR filtering coefficient (0 ~ 1)
                                  ///< <EM> Q-factor: 8U7 </EM>
                                  ///< If this cell is newly inserted to be measured in this sp, the iir coeff is set
                                  ///< to 128 (ie. 1) to use instantaneous value

  int32         rsrp_rx[2];       ///< RSRP in Q23 measured in the previous SP. 0 indicates prev value not available
  int32         rsrq_rx[2];       ///< RSRQ in Q24 measured in the previous SP. 0 indicates prev value not available

  boolean       ttl_enable; ///< Enable TTL for this neighbor cell
  boolean       ftl_enable; ///< Enable FTL for this neighbor cell

  lte_LL1_ttl_ncell_input_t ttl;  ///<. 
  lte_LL1_ftl_ncell_input_t ftl;  ///<.

} lte_LL1_meas_ttl_ftl_ncell_cdrx_sched_req_list_struct;

typedef struct
{
  int16         cell_id;    ///< Cell ID
  uint8         iir_coeff;        ///< IIR filtering coefficient (0 ~ 1)
                                  ///< <EM> Q-factor: 8U7 </EM>
                                  ///< If this cell is newly inserted to be measured in this sp, the iir coeff is set
                                  ///< to 128 (ie. 1) to use instantaneous value

  int32         rsrp_rx[2];       ///< RSRP in Q23 measured in the previous SP. 0 indicates prev value not available
  int32         rsrq_rx[2];       ///< RSRQ in Q24 measured in the previous SP. 0 indicates prev value not available

  boolean       ttl_enable; ///< Enable TTL for this neighbor cell
  boolean       ftl_enable; ///< Enable FTL for this neighbor cell

  lte_LL1_ttl_ncell_input_t ttl;  ///<. 
  lte_LL1_ftl_ncell_input_t ftl;  ///<.

} lte_LL1_meas_ttl_ftl_ncell_cdrx_cont_req_list_struct;

/*!
    @brief
    Neighbor measurements & tracking connected DRX mode request message for new cell list
*/
typedef struct
{
  lte_LL1_req_header_struct req_hdr;    ///< Common request parameters

  uint8                     num_ncells; ///< Total number of neighbor cells that are measured and tracked
  uint8                     num_rx_ant; ///< Number of UE Rx antennas. Common to all neighbor cells
  lte_earfcn_t              earfcn;     ///< Cell EARFCN for logging / debugging.
                                        ///< Range: 0 ~ 65535

  lte_LL1_meas_ttl_ftl_ncell_occasion_enum    occasion; ///< SFs 0,4,5 or 9 in FDD mode for nbr processing; SFs 0,1,5 or 6 in TDD
  lte_LL1_meas_ttl_ftl_ncell_cdrx_struct      cdrx; ///< Parameters common to all neighbor cells in connected mode

  lte_LL1_meas_ttl_ftl_ncell_cdrx_sched_req_list_struct cell_list_cdrx_sched[LTE_LL1_MAX_NCELLS_PER_REQ];	///< Neighbor cell specific parameters for connected DRX mode with new schedule.
  																																																				///< Used when a new cells enter the measurement set.
  																																																				///< Measurements, tracking request parameters for each neighbor cell
} lte_LL1_meas_ttl_ftl_ncell_cdrx_sched_req_struct;

LTE_LL1_INTF_MSG( meas_ttl_ftl_ncell_cdrx_sched_req );

/*!
    @brief
    Neighbor measurements & tracking connected DRX mode request message for 
    existing cell list

    @detail
    This message should only be sent after a 
    meas_ttl_ftl_ncell_cdrx_sched_req message. It will use the same cells as 
    the ones used in the previous CDRX processing window.
*/
typedef struct
{
  lte_LL1_req_header_struct req_hdr;    ///< Common request parameters

  uint8                     num_ncells; ///< Total number of neighbor cells that are measured and tracked
  uint8                     num_rx_ant; ///< Number of UE Rx antennas. Common to all neighbor cells
  lte_earfcn_t              earfcn;     ///< Cell EARFCN for logging / debugging.
                                        ///< Range: 0 ~ 65535

  lte_LL1_meas_ttl_ftl_ncell_occasion_enum    occasion; ///< SFs 0,4,5 or 9 in FDD mode for nbr processing; SFs 0,1,5 or 6 in TDD
  lte_LL1_meas_ttl_ftl_ncell_cdrx_struct      cdrx; ///< Parameters common to all neighbor cells in connected mode

  lte_LL1_meas_ttl_ftl_ncell_cdrx_cont_req_list_struct cell_list_cdrx_cont[LTE_LL1_MAX_NCELLS_PER_REQ];	///< Neighbor cell specific parameters for connected DRX mode with same schedule
  																																																			///< as previous scheduling period.	
  																																																			///< Measurements, tracking request parameters for each neighbor cell
} lte_LL1_meas_ttl_ftl_ncell_cdrx_cont_req_struct;

LTE_LL1_INTF_MSG( meas_ttl_ftl_ncell_cdrx_cont_req );

/*!
    @brief
    Neighbor measurements & tracking idle (intra and inter) and IRAT to LTE mode request message
*/
typedef enum {
TX0_TX1_AVG,
TX0_ONLY,
TX0_TX1_MAX
} lte_LL1_meas_tx_mode_e;


typedef struct
{
  lte_LL1_req_header_struct req_hdr;    ///<  Common request parameters
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
  lte_LL1_nb_id_enum        nb_id;        ///< Enum for NB id mapping
  lte_LL1_samp_serv_e       samp_rec_buf; ///< To indicate which buffer is used for sample recording

  uint8                     num_ncells; ///< Total number of neighbor cells that are measured and tracked
  uint8                     num_rx_ant; ///< Number of UE Rx antennas. Common to all neighbor cells
  uint8                     num_ms_per_cell; ///< Number of SFs averaged before computing RSRP/RSRQ for a neighbor cell
  lte_earfcn_t              earfcn;     ///< Cell EARFCN for logging / debugging.
                                        ///< Range: 0 ~ 65535
  lte_l1_cell_duplex_e      duplexing_mode; ///< duplexing mode
  boolean                   bw_is_atleast_3mhz; ///< TRUE: Bandwidth of neighbors is at-least 15 RBs (3 MHz)
                                                ///< FALSE: otherwise
  boolean                   is_mbsfn_present;   ///<  flag indicating presence of MBSFN scheduling
  //lte_LL1_meas_ttl_ftl_ncell_drx_struct       drx; ///< Parameters common to all neighbor cells in drx mode 
  lte_LL1_meas_ttl_ftl_ncell_occasion_enum    occasion; ///< SFs 0,4,5 or 9 in FDD mode for nbr processing; SFs 0,1,5 or 6 in TDD
  lte_LL1_meas_tx_mode_e    meas_tx_mode; ///< 0: average over 2 tx; 1 report tx0 2 report max tx0 and tx1
  ///< @todo Keep measurement duration parameter. Could be > 1 ms in DRX. It is always 1 ms in connected mode.
  ///< uint8                meas_duration;  ///< Measurement duration in ms

  lte_LL1_meas_ttl_ftl_ncell_req_list_struct  cell_list[LTE_LL1_MAX_NCELLS_PER_REQ];  ///< Neighbor cell specific parameters
                                                                                      ///< Measurements, tracking request parameters for each neighbor cell
  boolean use_offline_samples;

} lte_LL1_meas_ttl_ftl_ncell_drx_req_struct;

LTE_LL1_INTF_MSG( meas_ttl_ftl_ncell_drx_req );

/*! @brief  Neighbor cell measurement, tracking confirmation message 
    parameters
*/
typedef struct
{
  int32   rsrp_rx[2]; ///<  Instantaneous RSRP value per Rx antenna. Combined across Tx antennas. In dBm.
                      ///<  <EM> Q-factor: 32S23 </EM>
  int32   rsrp;       ///<  Instantaneous RSRP value combined across Tx-Rx pairs. In dBm.
                      ///<  <EM> Q-factor: 32S23 </EM>
  uint32  rsrp_snr_rx[2]; ///<  Instantaneous RSRP SNR value per Rx antenna. Combined across Tx antennas Linear.
                          ///<  <EM> Q-factor: 32U16 </EM>
  boolean rsrp_snr_rx_valid; ///<  RSRP SNR value valid.
                             ///<  <EM> Q-factor: N/A </EM>
  int32   rsrq_rx[2]; ///<  Instantaneous RSRQ value per Rx antenna. Combined across Tx antennas. In dBm.
                      ///<  <EM> <Q-factor>: 32S24 </EM>
  int32   rsrq;       ///<  Instantaneous RSRQ value combined across Tx-Rx pairs. In dBm.
                      ///<  <EM> Q-factor: 32S24 </EM>
  int32   rssi_rx[2]; ///<  RSSI value per Rx antenna. In dBm. Used for debugging.
                      ///<  <EM> Q-factor: 32S24 </EM>

  int32   rssi;       ///<  RSSI value combined across Rx antennas. In dBm. Used for debugging.
                      ///<  <EM> Q-factor: 32U24 </EM>

  int32   dvga_rx_log[2];     ///< DVGA gain per Rx antenna in log domain. Q-factor: 32S24
                              ///< DVGA gain when last measurement value was computed

  int32   lna_gain_rx_log[2]; ///< LNA gain per Rx antenna in log domain. Q-factor: 32S24
                              ///< LNA gain when last measurement value was computed

  int32	  filt_rssi_rx[2];    ///< Filtered RSSI per Rx antenna. In dBm.
															///< <EM> Q-factor: 32S24 </EM>
  lte_LL1_dl_interf_enum interfere_check; ///< will show if the results are interfered by DSDA or SLTE

} lte_LL1_meas_ncell_result_t;


typedef struct
{
  uint32   frame_bndry_ref_time_rx[2];  ///<  Neighbor cell frame boundary per Rx antenna as a modulo 10ms offset.
                                        ///<  Including neighbor cell TTL adjustment.
                                        ///<  <EM> Range: [0, 307199] </EM>
  int16   total_timing_adj_cir[2]; ///<  Total timing adjustment in CIR domain per Rx antenna to drive TTL
} lte_LL1_ttl_ncell_result_t;

typedef struct
{
  int16  ftl_cumu_freq_offset;  ///< Neighbor cell FTL offset since searcher frequency offset estimation (in Hz)
} lte_LL1_ftl_ncell_result_t;

typedef struct
{

  int16 ncell_id;       ///<  Neighbor cell ID used for descrambling RS symbols. Used in CNF for ML to identify neighbor cell

  lte_LL1_meas_ncell_result_t meas; ///<.
  lte_LL1_ttl_ncell_result_t  ttl;  ///<.
  lte_LL1_ftl_ncell_result_t  ftl;  ///<.

} lte_LL1_meas_ttl_ftl_ncell_cnf_list_struct;

/*! 
    @brief
    Confirmation message for neighbor cell measurements, tracking

    @detail
    This message is used to report results in connected, idle intra freq, idle 
    inter freq and IRAT to LTE modes
*/
typedef struct {

  lte_LL1_cnf_header_struct cnf_hdr;    ///< Common confirmation message parameters

  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA

  lte_LL1_nb_id_enum nb_id;               ///< Enum for NB id mapping

  /*! Common parameters */
  uint8     num_ncells;   ///< Total number of neighbor cells that were measured and tracked

  uint8     cnt_swrp;     ///< Indicates nth SWRP in MP. Range: 0 ~ (num_sp_per_mp-1)
                          ///< In idle intra freq, idle inter freq, IRAT to LTE modes,
                          ///< this value is always 0 since num_sp_per_mp = 1.

  lte_earfcn_t  earfcn;   ///< Cell EARFCN for logging / debugging.
                          ///< Range: 0 ~ 65535

  lte_l1_cell_duplex_e  duplexing_mode; ///< duplexing mode
  boolean               is_mbsfn_present;    ///<  flag indicating presence of MBSFN scheduling

  lte_LL1_samp_serv_e   samp_rec_buf;   ///< To indicate which buffer is used for sample recording

  /*! Neighbor cell specific results */
  lte_LL1_meas_ttl_ftl_ncell_cnf_list_struct  lte_LL1_meas_ttl_ftl_ncell_cnf_list[LTE_LL1_MAX_NCELLS_PER_REQ];
                                                      ///<  Measurements, tracking confirmation parameters for all neighbors
  boolean  interf_active;     ///<Result is interfered by other tech
}lte_LL1_meas_ttl_ftl_ncell_cnf_struct;

LTE_LL1_INTF_MSG( meas_ttl_ftl_ncell_cnf );

/*! 
    @brief
    Message to abort neighbor cell measurements / tracking modules.

    @detail
    This abort request is used in connected, idle intra freq, idle inter freq 
    and IRAT to LTE modes
*/
typedef struct
{
  lte_LL1_req_header_struct req_hdr;          ///< Common request parameters

  lte_LL1_nb_id_enum        nb_id;            ///< Enum for NB id mapping
  lte_LL1_samp_serv_e       samp_rec_buf;     ///< To indicate which buffer is used for sample recording
  lte_earfcn_t              earfcn;

} lte_LL1_meas_ttl_ftl_ncell_abort_req_struct;

LTE_LL1_INTF_MSG( meas_ttl_ftl_ncell_abort_req );

/*! 
    @brief
    Abort confirmation message for neighbor cell measurements, tracking

    @detail
    This message is used to confirm an abortive procedure in connected, idle 
    intra freq, idle inter freq and IRAT to LTE modes.
*/
typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;    ///< Common confirmation message parameters

  lte_LL1_nb_id_enum    nb_id;          ///< Enum for NB id mapping
  lte_LL1_samp_serv_e   samp_rec_buf;   ///< To indicate which buffer is used for sample recording
  lte_earfcn_t          earfcn;

} lte_LL1_meas_ttl_ftl_ncell_abort_cnf_struct;

LTE_LL1_INTF_MSG( meas_ttl_ftl_ncell_abort_cnf );

#endif /* INTF_MEAS_TTL_FTL_H */



/*!
  @file
  intf_srch.h

  @brief
  This file contains LTE LL search command id's and parameters structures

  @detail
  description of functions in this file
 
*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_srch.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef INTF_SRCH_H
#define INTF_SRCH_H


#include "intf_common.h"
#include "intf_srch_msg.h"
#include "msgr.h"
#include "intf_srch_config_param.h"
#include "lte_LL1_ue_types.h"
#include "lte_as.h"

/*===========================================================================

      Constants

===========================================================================*/
/*! @brief Maximum number of rasters for 20 MHz BW for FFS, based on 100KHz frequency separation
*/
#define MAX_NUM_FFS_100KHZ_RASTERS 200

/*! @brief Min/Max values for frequency capture time
*/

#define FSCAN_CAPTURE_TIME_MIN  1
#define FSCAN_CAPTURE_TIME_MAX  10

#define LTE_FSCAN_FDD_NUM_SPECTRUMS 1
#define LTE_FSCAN_TDD_NUM_SPECTRUMS 10

#define LTE_FSCAN_MAX_NUM_SPECTRUMS (LTE_FSCAN_TDD_NUM_SPECTRUMS)

/*! @brief Maximum number of words to store SSS indicies related to
   blocked cells. 
   A one dimensional array has bit mask for all SSS indicies that are blocked
   for each PSS index. There are 6 words per each PSS index 
   6 words per PSS * 3 (num of PSS indicies) = 18 Words
*/
#define MAX_NUM_WORD_BLOCKED_SSS_INDICIES 18
#define MAX_NUM_SRCH_IC_CELLS 6
/*===========================================================================

      Macros

===========================================================================*/

/*===========================================================================

      Typedefs

===========================================================================*/
typedef enum
{
  LTE_LL1_PRE_MID_AMBLE = 0,  // either pre or mid amble
  LTE_LL1_MID_AMBLE,          // mid amble preferred
  LTE_LL1_PRE_AMBLE,          // pre amble preffered
} lte_LL1_srch_amble_type_enum;

/*! @brief Define typedef for Frequency scan type (LFS/FFS)
*/
typedef enum
{
  LTE_LL1_SRCH_LFS_SCAN_TYPE=0,
  LTE_LL1_SRCH_FFS_SCAN_TYPE
} lte_LL1_srch_freq_scan_type_enum;

/* --------------------------------------------------------------------------
*
* Request, confirmation and indication message ID enums
*
* --------------------------------------------------------------------------*/

/*! @brief lte L1 srch confirmation message id's
*/
typedef enum
{
  LTE_LL1_SRCH_FRONT_END_CONFIG_CONF, ///<confirmation id for config request  
  LTE_LL1_SRCH_INIT_ACQ_CONF,         ///< confirmation id for initial acq request
    
} lte_LL1_srch_conf_id_enum;


typedef enum
{
  LTE_LL1_SRCH_TEST_DVGA_FBACK_COEFF = 0,
  LTE_LL1_SRCH_TEST_DVGA_UPDATE_RATE,
  LTE_LL1_SRCH_TEST_DVGA_SET_PT,
  LTE_LL1_SRCH_TEST_NUM_PSC_PEAKS,
  LTE_LL1_SRCH_TEST_NUM_PEAK_PER_PSC,
  LTE_LL1_SRCH_TEST_FINGER_THRESH_FREQ_EST,
  LTE_LL1_SRCH_TEST_STARTUP_SSC_FTL_ENABLE,
  LTE_LL1_SRCH_TEST_STARTUP_SSC_FTL_PSC_INTEGRATION_LEN,
  LTE_LL1_SRCH_TEST_STARTUP_SSC_FTL_SSC_INTEGRATION_LEN,
  LTE_LL1_SRCH_TEST_STARTUP_SSC_CLOSED_LOOP_ENABLE,
  LTE_LL1_SRCH_TEST_STARTUP_SSC_FTL_VCXO_CORRECTION_THRESH, //min error in hz for adj VCXO
  LTE_LL1_SRCH_TEST_STARTUP_SSC_FTL_VCXO_CORRECTION_LIMIT,  //max error in hz for adj VCXO
  LTE_LL1_SRCH_NS_SSC_NUM_HF_COMBINE,
  LTE_LL1_SRCH_NS_SSC_HF_GAP,
  LTE_LL1_SRCH_NS_SSC_ONE_SIDED_WIN_SZ,  // defines number of fingers used for SSC
  LTE_LL1_SRCH_NS_PSC_COH_ACC_LEN,
  LTE_LL1_SRCH_NS_INIT_ENABLE,
  LTE_LL1_SRCH_PEAK_LOCATION_ADJUSTMENT,  //offset between peak location detected vs. real peak - currently use to work around ~266us offset between real frame boundary and detected frame boundary by searcher
  LTE_LL1_SRCH_RSSI_VAL_LINEAR,
  LTE_LL1_SRCH_NS_SAMP_SRVR_FILL_ADDR_OFFSET,     // Used mainly in NS MVT
  LTE_LL1_SRCH_TEST_PARAM_COUNT  
}lte_LL1_SRCH_TEST_PARAM_enum;

/*! @brief parameters for front end config request
*/
typedef struct { 
  lte_LL1_req_header_struct   req_hdr;    ///<Common request header    
  int16 sampling_rate;
  uint8 num_rx_ant;
  
}lte_LL1_srch_front_end_config_req_struct;

/*! @brief message structure for front end config request message including header
*/
LTE_LL1_INTF_MSG( srch_front_end_config_req );

/*! @brief parameters for front end config confirmation
*/
typedef struct { 
  lte_LL1_cnf_header_struct cnf_hdr;      ///<confirmation message header
  
}lte_LL1_srch_front_end_config_cnf_struct;

/*! @brief message structure for Searcher front end config confirmation
    no payload currently*/
LTE_LL1_INTF_MSG( srch_front_end_config_cnf );

/*! @brief parameters for serving cell search request
*/
typedef struct { 
  lte_LL1_req_header_struct req_hdr;       ///<Common request header
  uint64              ref_time;            ///<Reference time reported by start search 
                                           ///<command  that corresponds to a 40 mSec 
                                           ///<boundary
  uint32              sss_thresh;          ///<Threshold for SSS correlations. To disable 
                                           ///<set to 0. 
  uint16              cell_ID;             ///<Cell ID derived from the detected PSS and 
                                           ///<SSS index
  uint8               win_left;            ///<Boundary for the window around detected PSS 
                                           ///<timing, to the left (in units of searcher 
                                           ///<samples; sampled at 1.92M samples/sec)
  
  uint8               win_right;            ///<Boundary for the window around detected PSS 
                                           ///<timing, to the right (in units of searcher 
                                           ///<samples; sampled at 1.92M samples/sec)    
  boolean             freq_offset_flag;    ///<A flag to indicate whether SSS based freq
                                           ///<offset estimation needs to be performed for 
                                           ///<this instance of serving cell srch (perform 
                                           ///<frequency offset estimation when TRUE)                                        
}lte_LL1_srch_serving_cell_srch_req_struct;

/*! @brief message structure for serving cell search request message including header
*/
LTE_LL1_INTF_MSG( srch_serving_cell_srch_req );

typedef struct 
{
  lte_LL1_req_header_struct   req_hdr;     ///<Common request header
  uint64 ref_time;         // 10 ms boundary if num_antennas = 0; 40 ms
                           //   boundary otherwise
  uint16 cell_ID;
  uint16 carrier_freq;     // in units of 100KHz, only valid if
                           //   initial_PBCH = 1
  int16  freq_offset;      // in units of Hz, only valid if
                           //   initial_PBCH = 1
  uint8  initial_PBCH;     // set to 1 for initial PBCH decode, 0 otherwise
  uint8  max_frames;
  uint8  cp_type;
  uint8  num_antennas;     // number of antennas for cell: 1, 2, or 4 
                           //  (or 0 if unknown)
} lte_LL1_srch_pbch_decode_req_struct;

/*! @brief message structure for PBCH decode request message including header
*/
LTE_LL1_INTF_MSG( srch_pbch_decode_req );

typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;      ///<confirmation message header
  uint64 ref_time;           // 40 ms boundary; invalid if crc_pass = 0
  uint64 payload;            // PBCH payload; invalid if crc_pass = 0
  uint16 cell_ID;            // echo of requested cell_ID
  int16  freq_offset;        // updated frequency offset estimate (in Hz); invalid if crc_pass = 0 or requested initial_PBCH = 0
  uint8  crc_pass;           // 0 if fail, 1 if pass
  uint8  num_decodes;        // number of 10 ms frames [1 to max_frames]    
  uint8  num_antennas;       // number of antennas for cell: 1, 2, or 4; invalid if crc_pass = 0
} lte_LL1_srch_pbch_decode_cnf_struct;

LTE_LL1_INTF_MSG( srch_pbch_decode_cnf );

/*! @brief message structure for Searcher test parameters.
*/
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;  ///<Common request header
  uint32 num_params;
  lte_LL1_config_element_struct test_srch_params[LTE_LL1_SRCH_TEST_PARAM_COUNT];
  
}lte_LL1_srch_test_param_cfg_req_struct;

LTE_LL1_INTF_MSG( srch_test_param_cfg_req );
 
/* Place holder for now: Serving cell search definition is not
   refined yet. */

typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;      ///<confirmation message header
  uint64 ref_time;   

}lte_LL1_srch_serving_cell_srch_cnf_struct;

LTE_LL1_INTF_MSG( srch_serving_cell_srch_cnf );

/*! @brief structure for detected cell related information
*/
typedef struct
{
  uint64 frame_bndry;                     ///<64 bit frame boundary RTC counter
  uint32 min_max_framebndry[2];           ///< [0] = min frame_bndry value for repeated Cell IDs (mod LTE Radio Frame ms)
                                          ///< [1] = max frame_bndry value for repeated Cell IDs (mod LTE Radio Frame ms)
  uint32 sss_metric;                  ///<uint32, range from 0 to 0xffff_ffff
                                          ///<SSS correlation value
  uint16 cell_id;                         ///<uint16, from 0 to 503 (3*sss_index+pss_index)
                                          ///<Cell ID (combination of PSS and SSS index)
  int16 freq_offset_est;                  ///<initial PSS based frequency offset
                                          ///<estimation (Hz)
  lte_l1_cp_e cp_type;                    ///<CP type
  uint8 lna_idx[2]; 					  ///< LNA Index when PSS,SSS search was done, i.e. @frame_boundary+PSS Offset
} lte_LL1_srch_cell_info_struct_t;

typedef struct
{
  uint32 frame_bndry;                     ///< 32 bit frame boundary

  int32 freq_offset;                      ///< In Hz.

  int32 rsrp;                             ///< In dBm, Q-factor: 32S23.

  uint16 cell_id;                         ///< uint16, from 0 to 503 (3*sss_index+pss_index)
                                          ///< Cell ID (combination of PSS and SSS index)
} lte_LL1_srch_ic_cell_req_struct_t;

typedef struct
{
  uint32 frame_bndry; ///< 32 bit frame boundary RTC counter

  int32 sss_cer_snr;  ///< SSS CER SNR value combined across Tx-Rx pairs. In dB.
                      ///< <EM> Q-factor: 32U16 </EM>

  int32 sss_cer_sum;  ///< SSS CER value combined across Tx-Rx pairs, sum all taps

  uint16 cell_id;     ///< uint16, from 0 to 503 (3*sss_index+pss_index)
                      ///< Cell ID (combination of PSS and SSS index)
  uint8 lna_idx[2];   ///< LNA Index when PSS,SSS search was done, i.e. @frame_boundary+PSS Offset
} lte_LL1_srch_ic_cell_info_struct_t;

/*! @brief Blocked cells are cells that will not be detected. This can be
   used to implement both cell barring and cell blacklisting scenarios.

   A one dimensional array has bit mask for all SSS indicies that are blocked
   for each PSS index.
   total number of SSS indicies=168, 6 words*32= 192 bits available, where last
   24 bits of word 5 are not used and should be set to 0 for any given PSS index.

   6 Words for PSS index 0:
   Word0       Bits:    31  30  29 ...    3   2  1  0     SSS Index: 31  30  29 ...    3   2  1  0
   Word1       Bits     31  30  29 ...    3   2  1  0                63  62  61 ...   35  34 33 32
   Word2           ....
   Word3           ....
   Word4           ....
   Word5       Bits           7  6  5  4  3   2  1  0              167 166 165 164 163 162 161 160

   6 Words for PSS index 1:
   Word0         Bits:    31  30  29 ...    3   2  1  0     SSS Index: 31  30  29 ...    3   2  1  0
   Word1           ....
                   ....
  
 6 Words for PSS index 2:
   Word0         Bits:    31  30  29 ...    3   2  1  0     SSS Index: 31  30  29 ...    3   2  1  0
   Word1           ....
                   ....
*/
typedef struct
{
  uint32  sss_indicies[MAX_NUM_WORD_BLOCKED_SSS_INDICIES];
                                          ///<block cell SSS indicies for all 3 PSS indicies
} lte_LL1_srch_blocked_cellid_struct_t;

/*! @brief Enum for FSCAN mode 
*/
typedef enum
{
  LTE_LL1_FSCAN_MODE_FFS = 0,
  LTE_LL1_FSCAN_MODE_LFS,
  LTE_LL1_FSCAN_MODE_COUNT,
}lte_LL1_fscan_mode_enum;

/*! @brief parameters for initial acquisition request. For more details
    on initial acquisition and its flowchart, refer to Odyssey Physical
    layer Control HLD, document number: 80-VM320-1 (in Agile)
*/
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;    ///<Common request header
  lte_LL1_nb_id_enum nb_id;               ///< Enum for NB id mapping
  uint16 abs_max_freq_offset;             ///<[Odyssey Only] uint16, range: 0 to 13500Hz , dafault = 13500Hz, 
                                          ///<Absolute maximum frequency offset(F)Hz
                                          ///<(a function of carrier frequency)
  lte_LL1_srch_blocked_cellid_struct_t  blocked_cells;
                                          ///<Blocked Cell IDs are any cell that ML does not
                                          ///<want to be returned or searched.
  uint8 min_hf;                           ///<uint8, range: 1 to 4, default:1, Minimum number of half frames to be used 
                                          ///<for non-coherent combining before starting
                                          ///<PSS/SSS detecion (M0 value from flowchart).
  uint8 max_hf;                           ///<uint8, range: 1 to 8, default:4, Maximum number of half frames to use for
                                          ///<non-coherent combining (M2 value from flowchart).
  uint8 max_results;                      ///<uint8, range:1 to 8, default=8, Maximum number
                                          ///<of best cells to be detected and
                                          ///<returned to the manager (N value from flowchart).
  lte_l1_cell_duplex_e  srch_fdd_tdd_mode;
                                          ///<LTE Mode selection FDD/TDD
  lte_earfcn_t  center_freq;              ///< Center frequency (N_DL channel number) to be returned in Confirmation
                                          ///< Refer to (N_DL) in 3GPP 36.101: section 5.7.3 Table 5.7.3-1 E-UTRA channel numbers
  boolean   do_targeted_acq_flag;         ///< True if targeted acq. needs to be attempted, false otherwise

  uint16     target_cid;                   ///< target Cell ID to attempt targeted acq. on
										 
  lte_LL1_fscan_mode_enum fscan_mode;      ///<fscan mode 
										 
} lte_LL1_srch_init_acq_req_struct;

LTE_LL1_INTF_MSG( srch_init_acq_req );

/*! @brief data structure for serving cell search confirmation response. Macro
    will also generate the MSG.
*/
typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;      ///<confirmation message header
  lte_LL1_nb_id_enum nb_id;               ///< Enum for NB id mapping
  lte_LL1_srch_cell_info_struct_t  scell_srch_info[MAX_NUM_DETECTED_INIT_CELLS];
                                          ///<Array of structures with all cell specific info
                                          ///<This array includes all detected cells info for last HFs
                                          ///<and built as below:
                                          ///<In each HF, if there are repeated Cell IDs, only the one 
                                          ///<with highest SSS_SNR is kept, Every 2 consecutive HFs,
                                          ///<cell information are sorted, in case of common Cell ID,
                                          ///<information related to latest case, is kept.
                                          
  uint8 num_of_detected_init_cells;       ///<Number of detected cells, This is sum of number of detected
                                          ///<cells per HF over all HFs (for multiple cells only the one
                                          ///<with highest SSS_SNR is selected, and is capped to max_results)
                                          
  lte_earfcn_t  center_freq;              ///<Center frequency (loop back to ML from request parameter)
  boolean  interf_active;                 ///<Result is interfered by other tech
} lte_LL1_srch_init_acq_cnf_struct;

LTE_LL1_INTF_MSG( srch_init_acq_cnf );

/*! @brief parameters for neighbor cell search request. For more details
    on initial acquisition and its flowchart, refer to Odyssey Physical
    layer Control HLD, document number: 80-VM320-1 (in Agile)
*/
typedef struct {

  lte_LL1_req_header_struct   req_hdr;    ///<Common request header
  
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA

  lte_LL1_nb_id_enum nb_id;               ///< Enum for NB id mapping

  lte_LL1_samp_serv_e   samp_rec_buf;   ///< To indicate which buffer is used for sample recording

  lte_LL1_srch_blocked_cellid_struct_t  blocked_cells;
                                          ///<Blocked Cell IDs are any cell that ML does not
                                          ///<want to be returned or searched.
  uint8 num_hf;                           ///<uint8, range: 1-2 
                                          ///<       1 half frame is used for SCell configuration.
                                          ///<       2 half frame is used for neighbor cell srch.
                                          ///<non-coherent combining before staring PSS/SSS
                                          ///<detecion (M0 from flowchart).
  uint8 hf_gap;                           ///<uint8, range:0 to 4, default:0,
                                          ///<Gap between two successive half frame 
                                          ///<in case if for time diversity half frames are
                                          ///<separated by a gap (in unit of number of
                                          ///<half frames between two Half frames).
  uint8 num_sf_srch_win;                  ///<uint8, range from 1 to 10, default=5 sub-frames
                                          ///<number of sub-frame to be used for search per half frame
                                          ///<(also length of non-coherent buffer). This give us the
                                          ///<flexibility to search different length)
  uint8 max_results;                      ///<uint8, range:1 to 16, default=8, Maximum
                                          ///<number of best cells to be detected and
                                          ///<returned to the manager (N value from flowchart).
  lte_l1_cp_e scell_cp_type;              ///<serving cell CP type (normal, extended), this
                                          ///<is not really needed, and is for better SW design
  lte_l1_cell_duplex_e srch_fdd_tdd_mode; ///<LTE Mode selection FDD/TDD
  lte_earfcn_t  center_freq;              ///<Center frequency (N_DL channel number) to be returned in Confirmation
                                          ///<Refer to (N_DL) in 3GPP 36.101: section 5.7.3 Table 5.7.3-1 E-UTRA channel numbers
  boolean samp_rec_continuous;            /// primary sample buffer recording duration indicator, 0 : <=7 msec, 1 : > 7 msec
                                          /// FW primary NB sample buffer used for regular as well as IRAT operation can only hold 7 msec, 
                                          /// with old samples continuously being overwritten after first 7 msec. For searcher to support
                                          /// both Idle Mode and Connect Mode W2L (for example) operations, it needs to know where in
                                          /// primary NB sample buffer to start the search.Searcher can start from current (processing
                                          /// time of neighbor search request) location (-1/3 msec since HW immediately needs 1/3 msec
                                          /// data to get started) assuming 5+ msec more samples will become available for search to
                                          /// complete. This is the case with Idle Mode W2L in which neighbor search request may come 2~3
                                          /// msec after start of sample recording.
                                          /// On the other hand, Searcher must start from very beginning (regardless of processing time
                                          /// of neighbor search request) location since the sample recording may be just enough to
                                          /// complete the process and searcher cannot afford to waste anything. This is the case with
                                          /// Connect Mode W2L.

  boolean legacy_srch;

  boolean adv_rx_srch;

  boolean double_speed_en;

  lte_LL1_srch_ic_cell_req_struct_t ic_cell_list[ MAX_NUM_SRCH_IC_CELLS];  /// ic cell list

  //uint32 num_subframe_avail;       /// number of subframe available

  boolean  scell_timing_center_flag; /// True: using scell as center; False: use ncell as center

  uint32 num_ic_cell;                /// number of IC cells

  uint8 ic_max_results;              /// uint8, range:1 to 16, default=8, Maximum

  int16 adv_rx_srch_t1;              /// sss-ic srch range, left to center

  int16 adv_rx_srch_t2;              /// sss-ic srch range, right to center

  lte_LL1_srch_amble_type_enum amble_type; /// amble type

} lte_LL1_srch_neighbor_cell_srch_req_struct;

LTE_LL1_INTF_MSG( srch_neighbor_cell_srch_req );

/*! @brief Data and message structure for neighbor cell search
    response and neighbor search abort response message. For neighbor
    search abort message, the same neighbor search response will be
    sent, with abort flag =1
*/


typedef struct {

  lte_LL1_cnf_header_struct cnf_hdr;      ///<confirmation message header

  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA

  lte_LL1_nb_id_enum nb_id;               ///< Enum for NB id mapping

  lte_LL1_samp_serv_e   samp_rec_buf;   ///< To indicate which buffer is used for sample recording

  lte_LL1_srch_cell_info_struct_t ncell_srch_info[MAX_NUM_DETECTED_NEIGHBOR_CELLS];
                                          ///<Array of structure contains cell specific parameters
  uint8 num_of_detected_neighbor_cells;   ///<Number of detected cells
  
  uint8 neighbor_srch_abort_flag;         ///<Flag to indicate if neighbor search was aborted
                                          ///<0=No abort happend, 1=neighbor search was aborted
  lte_earfcn_t  center_freq;              ///<Center frequency (loop back to ML from request parameter)
  boolean  interf_active;                 ///<Result is interfered by other tech
  lte_LL1_dl_interf_enum interferer_type; ///< Search samples interferer type
  boolean bad_samples_due_to_rf_delay;    ///< Indicate RF-Warmup delay so ML1 discards Ngbhr srch results..Applicable to Offline-DRX mode only
} lte_LL1_srch_neighbor_cell_srch_cnf_struct;

LTE_LL1_INTF_MSG( srch_neighbor_cell_srch_cnf );


typedef struct {

  lte_LL1_cnf_header_struct cnf_hdr;      ///< confirmation message header

  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA

  lte_LL1_srch_ic_cell_info_struct_t ic_cell_list[ MAX_NUM_SRCH_IC_CELLS];  // ic cell list

  uint32 num_ic_cell;                   ///< number of IC cells

  lte_LL1_srch_cell_info_struct_t ncell_srch_info[MAX_NUM_DETECTED_NEIGHBOR_CELLS];
                                        ///< Array of structure contains cell specific parameters
  uint8 num_of_detected_neighbor_cells; ///< Number of detected cells

  boolean neighbor_ic_srch_abort_flag;  ///< Flag to indicate if neighbor search was aborted
                                        ///< 0=No abort happend, 1=neighbor search was aborted
  lte_earfcn_t  center_freq;            ///< Center frequency (loop back to ML from request parameter)

} lte_LL1_srch_neighbor_cell_srch_ic_cnf_struct;

LTE_LL1_INTF_MSG( srch_neighbor_cell_srch_ic_cnf );

// XXX- ncell black list related Req/Confirmation have to be deleted, when cleaning up the search message.
/*! @brief Parameters for neighbor cell search black list. As the number
    of balck lists for neighbor search could be big, they come through a
    seperate message. FW will keep a copy for each carrier frequency.
*/
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;    ///<Common request header
  uint16 num_black_list_elements;         ///<Number of black lists elements
  lte_LL1_nsrch_black_list_index_enum_t black_list_index_enum;
                                          ///<Index to neighbor black list, is an 
                                          ///<enum, to figure out which black list array to
                                          ///<be used for neighbor search or even no black list
  lte_LL1_srch_black_cell_struct_t  black_list[MAX_NUM_BLACK_LIST_ELEMENTS];
                                          ///<black list cell IDs
} lte_LL1_srch_black_list_req_struct;

LTE_LL1_INTF_MSG( srch_black_list_req );

/*! @brief message structure for neighbor search black list
    confirmation reponse message. This structure only
    includes confirmation header
*/
typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;      ///<confirmation message header
} lte_LL1_srch_black_list_cnf_struct;


/*! @brief message structure for neighbor search black list
    confirmation reponse message. This structure is empty
    and message only includes MSG header
*/
LTE_LL1_INTF_MSG( srch_black_list_cnf );

/*! @brief Parameters for search configuration request. ML sends
    this message if any of the parameters changed
*/
typedef struct 
{
  lte_LL1_req_header_struct   req_hdr;    ///<Common request header
  uint32 num_params;
  lte_LL1_srch_config_element_struct_t srch_config_params[LTE_LL1_SRCH_CONFIG_PARAM_COUNT];
} lte_LL1_srch_proc_config_req_struct;

LTE_LL1_INTF_MSG( srch_proc_config_req );

/*! @brief Structure for srch proc config
    confirmation structure.
*/
typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;      ///<confirmation message header
} lte_LL1_srch_proc_config_cnf_struct;

/*! @brief message structure for search config response message.
    It only contains msg_hdr.
*/
LTE_LL1_INTF_MSG( srch_proc_config_cnf );

/*! @brief Structure for init acq cell srch abort 
    request structure.
*/
typedef struct
{
  lte_LL1_req_header_struct   req_hdr; ///<Common request header
  lte_LL1_nb_id_enum          nb_id;  ///< Enum for NB id mapping
} lte_LL1_srch_init_acq_abort_req_struct;

/*! @brief Message structure for initial cell search abort
    request message.
*/
LTE_LL1_INTF_MSG( srch_init_acq_abort_req );

/*! @brief Structure for init acq cell srch abort 
    confirmation structure.
*/
typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;      ///<confirmation message header
  lte_LL1_nb_id_enum        nb_id;        ///< Enum for NB id mapping
} lte_LL1_srch_init_acq_abort_cnf_struct;

/*! @brief Message structure for initial cell search abort
    confirmation message.
*/
LTE_LL1_INTF_MSG( srch_init_acq_abort_cnf );

/*! @brief Structure for neighbor cell search abort 
    request structure.
*/
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;       ///<Common request header

  lte_LL1_nb_id_enum          nb_id;         ///< Enum for NB id mapping
  lte_LL1_samp_serv_e         samp_rec_buf;  ///< To indicate which buffer is used for sample recording

} lte_LL1_srch_ncell_srch_abort_req_struct;

/*! @brief Message structure for neighbor cell search abort
    request message.
*/
LTE_LL1_INTF_MSG( srch_ncell_srch_abort_req );

/*! @brief Structure for neighbor cell srch abort 
    confirmation structure.
*/
typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;      ///<confirmation message header
  lte_LL1_nb_id_enum        nb_id;        ///< Enum for NB id mapping
  lte_LL1_samp_serv_e       samp_rec_buf; ///< To indicate which buffer is used for sample recording
} lte_LL1_srch_ncell_abort_cnf_struct;

/*! @brief Message structure for neighbor cell search abort
    confirmation message.
*/
LTE_LL1_INTF_MSG( srch_ncell_abort_cnf );

/*! @brief LFS data structure
*/
typedef struct
{
  lte_earfcn_t                  center_freq; ///< Center frequency (N_DL channel number) to be returned in Confirmation
                                             ///< Refer to (N_DL) in 3GPP 36.101: section 5.7.3 Table 5.7.3-1 E-UTRA channel numbers
  lte_bandwidth_enum            lfs_bw;      ///< LFS BW enumerator
  lte_l1_cell_duplex_e          duplex_mode; ///< System selection (TDD/FDD)
} lte_LL1_srch_list_freq_scan_struct;

typedef struct
{
  lte_LL1_req_header_struct           req_hdr;  ///< Common request header
  lte_LL1_nb_id_enum                  nb_id;    ///< Enum for NB id mapping
  lte_LL1_srch_list_freq_scan_struct  lfs_data; ///< LFS data to be returned in confirmation.
  uint8                           capture_time_per_spectrum; ///< Number of ms of capture data to accumulate over per spectrum (1 <= value <= 10)
} lte_LL1_srch_list_freq_scan_req_struct;

LTE_LL1_INTF_MSG( srch_list_freq_scan_req );

typedef struct
{
  lte_LL1_cnf_header_struct           cnf_hdr;  ///< Confirmation message header
  lte_LL1_nb_id_enum                  nb_id;    ///< Enum for NB id mapping
  lte_LL1_srch_list_freq_scan_struct  lfs_data; ///< LFS data structure from Request.
                                                ///< Below are scale that is used for FFT task, and bias
                                                ///< that is used in load vector task
  int16  scale0;                                ///< Return a scale for Rx ANT0 for ML to adjust its noise threshold
                                                ///< FW returns energy in LSB^2/100KHz (not dBm) (16S15)
  int8   bias0;                                 ///< Return a bias for Rx Ant0 for ML to adjust its noise threshold (8S0)
  int16  scale1;                                ///< Return a scale for Rx ANT1 for ML to adjust its noise threshold
                                                ///< FW returns energy in LSB^2/100KHz (not dBm)(16S15)
  int8   bias1;                                 ///< Return a bias for Rx ANT1 for ML to adjust its noise threshold (8S0)
  uint32                     energy_estimate[LTE_FSCAN_MAX_NUM_SPECTRUMS]; 
                                                ///< EE value returned from LL sent to RRC directly (LSB^2/100 kHz)
  boolean  interf_active;                       ///<Result is interfered by other tech
} lte_LL1_srch_list_freq_scan_cnf_struct;

LTE_LL1_INTF_MSG( srch_list_freq_scan_cnf );

/*! @brief FFS data structure
*/
typedef struct
{
  lte_earfcn_t                  center_freq; ///< Center frequency (N_DL channel number) to be returned in Confirmation
                                             ///< Refer to (N_DL) in 3GPP 36.101: section 5.7.3 Table 5.7.3-1 E-UTRA channel numbers
  lte_bandwidth_enum            ffs_bw;      ///< FFS BW enumerator
  lte_l1_cell_duplex_e          duplex_mode; ///< System selection (TDD/FDD)
} lte_LL1_srch_full_freq_scan_struct;


typedef struct
{
  lte_LL1_req_header_struct           req_hdr;  ///< Common request header
  lte_LL1_nb_id_enum                  nb_id;    ///< Enum for NB id mapping
  lte_LL1_srch_full_freq_scan_struct  ffs_data; ///< FFS data to be returned in confirmation.
  uint8                           capture_time_per_spectrum; ///< Number of ms of capture data to accumulate over per spectrum (1 <= value <= 10)
} lte_LL1_srch_full_freq_scan_req_struct;

LTE_LL1_INTF_MSG( srch_full_freq_scan_req );

typedef struct
{
  lte_LL1_cnf_header_struct            cnf_hdr;        ///< Confirmation message header
  lte_LL1_nb_id_enum                   nb_id;          ///< Enum for NB id mapping
  lte_LL1_srch_full_freq_scan_struct  ffs_data;        ///< Full frequency scan data structure from Request
                                                       ///< Below are scale that is used for FFT task, and bias
                                                       ///< that is used in load vector task per each Rx ANT
  int16  scale0;                                       ///< Return a scale for Rx ANT0 for ML to adjust its noise threshold
                                                       ///< FW returns energy in LSB^2/100KHz (not dBm) (16S15)
  int8   bias0;                                        ///< Return a bias for Rx Ant1 for ML to adjust its noise threshold (8S0)
  int16  scale1;                                       ///< Return a scale for Rx ANT1 for ML to adjust its noise threshold
                                                       ///< FW returns energy in LSB^2/100KHz (not dBm) (16S15)
  int8   bias1;                                        ///< Return a bias for Rx ANT1 for ML to adjust its noise threshold (8S0)
  uint32 capture_start_time;                           ///< The starting rtc % 5 for each of the spectrums
  uint8   num_entries;                                 ///< Number of entries in the subsequent result arrays
  uint32  energy_estimate[MAX_NUM_FFS_100KHZ_RASTERS]; ///< EE values returned from LL (LSB^2/100 kHz)
  boolean  interf_active;                              ///<Result is interfered by other tech
} lte_LL1_srch_full_freq_scan_cnf_struct;

LTE_LL1_INTF_MSG( srch_full_freq_scan_cnf );

/*! @brief Freq Scan abort Request 
*/
typedef struct
{
  lte_LL1_req_header_struct         req_hdr;           ///< Common request header
  lte_LL1_nb_id_enum                nb_id;             ///< Enum for NB id mapping
  lte_LL1_srch_freq_scan_type_enum  scan_type_enum;    ///< Frequency scan type enum
} lte_LL1_srch_freq_scan_abort_req_struct;

LTE_LL1_INTF_MSG( srch_freq_scan_abort_req );

/* Frequency Scan abort cnf */
typedef struct
{
  lte_LL1_cnf_header_struct         cnf_hdr;           ///< Confirmation message header
  lte_LL1_nb_id_enum                nb_id;             ///< Enum for NB id mapping
  lte_LL1_srch_freq_scan_type_enum  scan_type_enum;    ///< Frequency scan type enum
} lte_LL1_srch_freq_scan_abort_cnf_struct;

LTE_LL1_INTF_MSG( srch_freq_scan_abort_cnf );

#endif /* INTF_SRCH_H */

/** @file rr_l1_g.h */

/*===========================================================================
      COPYRIGHT INFORMATION

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/inc/rr_l1_g.h#1 $    $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
17/02/14   cja     CR605188 Add PN_offset to WCDMA best six structure
15/01/13   sjw     Pass number of active SIMs down to GL1
16/05/11   sjw     Recovery mechanism for stalled ISR when GOING_INACTIVE
01/02/11   dmb     The following RR<->L1 messages are removed (not used): 
                   MPH_EXT_MEASUREMENTS_REQ/IND, MPH_STOP_EXT_MEASUREMENTS_REQ
27/01/11   dmb     The following RR<->L1 messages are removed (not used): 
                   MPH_ADD_CHANNEL_ASSIGN_REQ/CNF,MPH_STOP_CAMPING_REQ,MPH_STOP_READ_SYS_INFO_REQ
18-10-10   sjw     Added support for FEATURE_DUAL_SIM
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
09/06/09   nf      Added FEATURE_GSM_PCH_SINGLE_BURST_DEC
05/06/09   sjw     Moved handover_details and assignment_details out of rr_data.c
13/06/07   og      Allow W to inform G about the frequency offset of the G cell
                   in W2G non-blind handover scenarios.
11/03/05   og      Addition of L1 test mode start/stop indication messages.
12/08/03   gw      Modified primitives related to SI reading to give (G)RR
                   better control over which SI messages L1 receives.
02/19/03   Rao     Added GSM Deep Sleep support.
01/20/02   Rao     Changing RSCP from word to int.
11/08/02   he      Added sync_flag to mph_gprs_reselection_req_t.
11/05/02   JAC     Added multirate support
11/04/02   Rao     Added Vocoder types for Vocoder control related changes.
                   Also WCDMA to GSM Ciphering related changes.
10/21/02   he      Renamed NORMAL to NORMAL_MSG, EXTENDED to EXTENDED_MSG.
08/26/02   rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
10/03/02   Rao     Added InterRAT Reselection ( GSM to WCDMA ) Support.
10/15/02   Rao     Modified rr_l1_interrat_measurement_T struct to match RRC struct.
08/26/02   rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
08/06/02   JAC     Added surround update failure ind
08/05/02   JAC     Added cell selection optimization types
07/23/02   Rao     Added InterRAT ( WCDMA To GSM )  Handover support
06/19/02   JAC     Added mph_reselection_cnf and mph_early_camping_req
05/30/02   rmc     Removed BA_MAX_LEN already defined in sys_cnst.h
05/22/02   rmc     Removed error_code in START/STOP GSM_MODE_CNF messages
02/25/02   rao     Added  L1 message support for dual mode.
09/17/01   JC      Added include files that this one relies on.

===========================================================================*/
#ifndef rr_l1_g_h
#define rr_l1_g_h

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "sys_cnst.h"
#include "sys_stru.h"
#include "sys_type.h"
#include "rr_l2_l1.h"
#include "mm_as_types.h"
#include "geran_multi_sim.h"
#include "timetick.h"
#include "geran_grr_msg.h"

#define CIPHER_KEY_LENGTH  8

#define CIPHER_KEY_128_LENGTH  16

#define MAX_COUNT_OF_RXLEV 5
#define CA_MAX_LEN         64

#define MAX_NUMBER_OF_DED_BEST_WCDMA_CELLS 8
#define MAX_NUMBER_OF_BEST_WCDMA_CELLS 6

#if defined (FEATURE_WCDMA) || defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)

/*! Maximum number of WCDMA measurements that may be sent to RR for a single
WCDMA frequency. */
#define MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY 8U

/*! Maximum number of UTRAN TDD measurements that may be sent to RR for a single
UTRAN TDD frequency. */
#define MAX_UTRAN_TDD_MEASUREMENTS_PER_FREQUENCY 32U

#define MAX_IRAT_STORED_DED_MEASUREMENTS 24U

/*! Maximum number of LTE measurements that may be sent to RR for a single LTE
frequency. */
#define MAX_LTE_MEASUREMENTS_PER_FREQUENCY   16U

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

#define MAX_NUMBER_OF_BEST_CELLS 6
#define MAX_NUMBER_OF_SURROUND_CELLS 32

typedef byte monitor_action_T;

#define MONITOR_INIT 0
#define MONITOR_NEXT 1
#define MONITOR_STOP 2

typedef byte cipher_algorithm_T;

#define ALGORITHM_A5_1 0
#define ALGORITHM_A5_2 1
#define ALGORITHM_A5_3 2
#define ALGORITHM_A5_4 3
#define ALGORITHM_A5_5 4
#define ALGORITHM_A5_6 5
#define ALGORITHM_A5_7 6

typedef byte hand_sync_ind_T;

#define NON_SYNCHRONISED    0
#define SYNCHRONISED        1
#define PRE_SYNCHRONISED    2
#define PSEUDO_SYNCHRONISED 3

#define RR_L1_MAX_INTERRAT_MEASUREMENTS 32

/*Maximum number of freq returned by power scan*/
#define RR_MAX_PSCAN_FREQ_SCAN 971

/* Cross-check the value used in the API */
#if RR_MAX_PSCAN_FREQ_SCAN > RR_MAX_PSCAN_FREQ_SCAN_ARRAY_SIZE
  #error RR_MAX_PSCAN_FREQ_SCAN > RR_MAX_PSCAN_FREQ_SCAN_ARRAY_SIZE
#endif

typedef enum
{
  RR_L1_DCS_1800,
  RR_L1_PCS_1900
} rr_l1_band_indicator_T;

typedef enum
{
  RR_L1_BAND_BITMAP_PGSM      = 0x01,
  RR_L1_BAND_BITMAP_EGSM      = 0x02,
  RR_L1_BAND_BITMAP_DCS_1800  = 0x04,
  RR_L1_BAND_BITMAP_PCS_1900  = 0x08,
  RR_L1_BAND_BITMAP_GSM_850   = 0x10
} rr_l1_band_bitmap_bands_T;

typedef byte rr_l1_band_bitmap_T;

/* Bit masks for specifying which SI messages to receive.
 * These masks can be ORed together to specify combinations
 * of SI messages.
 */
typedef enum {
   /* SIs on normal BCCH */
   RR_L1_SI_TYPE_NONE          = 0x0000,
   RR_L1_SI_TYPE_1             = 0x0001,
   RR_L1_SI_TYPE_2             = 0x0002,
   RR_L1_SI_TYPE_2_bis         = 0x0004,
   RR_L1_SI_TYPE_2_ter         = 0x0008,
   RR_L1_SI_TYPE_2_quater_NORM = 0x0010,
   RR_L1_SI_TYPE_3             = 0x0020,
   RR_L1_SI_TYPE_4             = 0x0040,
   RR_L1_SI_TYPE_9             = 0x0080,
   RR_L1_SI_TYPE_13_NORM       = 0x0100,
   RR_L1_SI_TYPE_13_alt_NORM   = 0x0200,
   RR_L1_SI_TYPE_15_NORM       = 0x0400,
   RR_L1_SI_TYPE_ALL_NORM      = 0xffff,

   /* SIs on extended BCCH */
   RR_L1_SI_TYPE_2_quater_EXT  = 0x00010000,
   RR_L1_SI_TYPE_7             = 0x00020000,
   RR_L1_SI_TYPE_8             = 0x00040000,
   RR_L1_SI_TYPE_13_EXT        = 0x00080000,
   RR_L1_SI_TYPE_13_alt_EXT    = 0x00100000,
   RR_L1_SI_TYPE_15_EXT        = 0x00200000,
   RR_L1_SI_TYPE_16            = 0x00400000,
   RR_L1_SI_TYPE_17            = 0x00800000,
   RR_L1_SI_TYPE_ALL_EXT       = 0x7fff0000

   /* Note this enum is implemented as an int
    * so we can't use values > 0x7fffffff.
    */
} rr_l1_si_type_T;

/* Inter-RAT measurement data type */
typedef struct
{
  /* GSM ARFCN */
  ARFCN_T bcch_arfcn;

  /* Frame offset */
  uint32 fn_offset;

  /* qbit offset */
  uint16 qbit_offset;

  /* GSM BSIC */
  uint8 bsic_ncc;
  uint8 bsic_bcc;
  /* RSSI */
  int16  rssi_dBm_x16;

  /* @@OG: fine freq */
  int16  frequency_offset;

} rr_l1_interrat_measurement_T;

/* Inter-RAT measurement data list type */
typedef struct
{
  /* number of cells */
  uint8 number_of_cells;

  /* Inter-RAT measurement data */
  rr_l1_interrat_measurement_T interrat_measurements[RR_L1_MAX_INTERRAT_MEASUREMENTS];
} rr_l1_interrat_measurement_data_T;

typedef enum
{
  RR_L1_DECODE_SI3_OR_SI4 = 0,
  RR_L1_DECODE_SI7_OR_SI8 = 1
} rr_l1_surround_cell_si_requested_T;

typedef enum
{
  RR_L1_GSM_GSM,
  RR_L1_WCDMA_GSM,
  RR_L1_GSM_WCDMA,
  RR_L1_TDS_GSM,
  RR_L1_LTE_GSM,
  RR_L1_HANDOVER_TYPE_COUNT
} rr_l1_handover_type_T;

typedef struct
{
   byte        msb;
   byte        lsb;
} imsi_mod_1000_T;

typedef struct
{
   ARFCN_T     BCCH_ARFCN;
   boolean     valid_info;
   byte        SCH_block[SCH_BLOCK_SIZE];
   byte        RXLEV_average;
   byte        RXLEV_instant;
} surrounding_channel_info_T;

/* This structure can contain up to 32 ncell entries, but only
the best six will be used for most measurement reporting.  When EMR
is active all 32 may be used. */
typedef struct
{
  byte no_of_entries;
  surrounding_channel_info_T
                          surrounding_channel_info[MAX_NUMBER_OF_SURROUND_CELLS];
#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST
  boolean reduced_ncell_list_stored_in_l1;    /* L1's snapshot of reduced_ncell_list which was last sent by RR - for L1 <-> RR sync */
  boolean reduced_ncell_list_mode_active;     /* TRUE => GL1 is using top2mode */
  boolean reduced_ncell_list_allowed;         /* TRUE when GL1 in Idle/GPRS mode, FALSE otherwise */
#endif
} best_six_info_T;


/* RR -> L1 Stop GSM Mode Reason */
typedef enum
{
  RR_L1_RAT_CHANGE,
  RR_L1_DEEP_SLEEP,
  RR_L1_GSM_STOP_REASON_UNDEFINED
} rr_l1_gsm_stop_reason_T;


typedef enum
{
  RR_L1_MODE_CHANGE,
  RR_L1_OFF_LINE,
  RR_L1_POWER_OFF
} rr_l1_gsm_deact_reason_T;


/**************************************************************
* 1. channel type
* 2. subchannel :                      RANGE 0 to 7
* 3. TN = Timeslot Number              RANGE 0 to 7
* 4. TSC = Training Sequence Code      RANGE 0 to 7
* 5. hopping_flag = TRUE,  MAIO and HSN are Valid
*                 = FALSE, the frequency list contains only
*                          one entry.
* 6. MAIO = Mobile Allocation Index Offset RANGE 0 to 63
* 7. HSN = Hopping Sequence Number         RANGE 0 to 63
*
***************************************************************/

#define RR_MAX_CANDIDATE_FREQ_LIST 140

typedef enum
{
  RR_L1_EPD_OFF_PREFERRED = 0, /* EPD OFF pref used by L1 */
  RR_L1_EPD_ON_PREFERRED,      /* EPD ON pref used by L1  */
  RR_L1_EPD_NO_CHANGE          /* No Change to be used  by  L1 */
} rr_l1_epd_preference_t;

typedef struct
{
  word    num_frequencies;
  ARFCN_T arfcn[RR_MAX_CANDIDATE_FREQ_LIST];
} rr_l1_idle_freq_list_T;

typedef enum
{
  RR_L1_SCH_FAILED,
  RR_L1_BCCH_FAILED,
  RR_L1_BCCH_BLOCKED
} rr_l1_decode_bcch_failure_T;

typedef struct
{
   channel_type_T       channel_type;
   byte                 subchannel;
   byte                 TN;
   byte                 TSC;
   boolean              hopping_flag;
   byte                 MAIO;
   byte                 HSN;
   frequency_list_T     frequency_list;
} channel_information_T;

typedef struct
{
  boolean valid;
  boolean noise_suppression_off;
  boolean start_mode;
  boolean start_mode_valid;
  byte    codec_modes;
  byte    thresholds[3];
  byte    hysteresis[3];
} multirate_config_T;

typedef struct channel_spec_tag
{
   byte                  number_of_ded_channels;
   boolean               DTX_indicator;
   byte                  power_level;
   boolean               starting_time_valid;
   byte                  starting_time[2];
   boolean               before_parameters_valid;
   boolean               cipher_flag;
   cipher_algorithm_T    cipher_algorithm;
   channel_mode_T        channel_mode_1;
   channel_mode_T        channel_mode_2;
   channel_information_T channel_info_1_after;
   channel_information_T channel_info_2_after;
   channel_information_T channel_info_1_before;
   channel_information_T channel_info_2_before;

} channel_spec_T, assignment_details_T;

typedef struct
{
  byte no_of_entries;
  ARFCN_T channel_number[CA_MAX_LEN];
} cell_channel_description_T;

typedef struct
{
  byte                              synchronisation_indication;
  byte                              ROT;
  byte                              handover_reference;
  ARFCN_T                           BCCH_ARFCN;
  BSIC_T                            BSIC;
  boolean                           real_time_difference_valid;
  byte                              real_time_difference;
  boolean                           valid_CA_received;
  byte                              NCI;
  cell_channel_description_T        cell_channel_description;
  boolean                           timing_advance_valid;
  byte                              timing_advance;
  channel_spec_T                    channel_spec;
  rr_l1_handover_type_T             handover_type;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_l1_interrat_measurement_data_T interrat_measurement_data;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  channel_mode_T                    old_channel_mode;
  channel_mode_T                    new_channel_mode;
  byte                              cipher_key[CIPHER_KEY_LENGTH];

  byte                              cipher_key_128[CIPHER_KEY_128_LENGTH];

} handover_details_T;

/******************************************
 * This is the list of the BCCH Carriers
 * broadcast as being the cell neighbours
 * description
 *****************************************/

typedef struct
{
   byte                 no_of_entries;
   ARFCN_T              BCCH_ARFCN[BA_MAX_LEN];
} inter_task_BCCH_allocation_T;

/*************************************************************
 * The Layer_1_Sys_Info structure is passed to layer 1
 * so this latter is able to tune the channels in a
 * correctly manner
 * 1. BCCH Allocation : This is the list of the BCCH Carriers
 *    of the neighbour cells broadcast on the BCCH of the
 *    current cell
 * 2. BS_PA_MFRMS : RANGE  2 to 9
 *    Indicates the number of 51 TDMA frame multiframes
 *    between transmission of paging messages to mobiles
 *    of the same paging group
 * 3. BS_AG_BLOCK_RES : RANGE 0 to 7
 *    Indicated the number of blocks on each control channels
 *    reserved for access grant messages
 * 4. BS_CC_CHANS : RANGE  1 to 4
 *    derived from CCCH_CONF,BA indicates the number of common
 *    control channels
 * 5. BS_CCCH_SDCCH_COMB RANGE TRUE or FALSE
 *    derived from CCCH_CONF, indicates wether or not CCCH and
 *    SDCCH are combined
 * 6. MS_TXPWR_MAX_CCH RANGE 0 to 31
 *    This is the power control level and corresponds
 *    to the maximum Tx power level an MS may use when accessing
 *    a Control Channel CCH
 * 7. PWRC, the power control indicator
 ***************************************************************/

typedef struct
{
   inter_task_BCCH_allocation_T BA_list;
   byte                         BS_PA_MFRMS;
   byte                         BS_AG_BLKS_RES;
   byte                         BS_CC_CHANS;
   boolean                      BS_CCCH_SDCCH_COMB;
   byte                         MS_TXPWR_MAX_CCH;
   boolean                      PWRC;
   byte                         NCC_permitted;
   byte                         TX_integer;
   boolean                      cbch_channel_information_valid;
   channel_information_T        cbch_channel_information;

   byte                         multiband_reporting;
   boolean                      LB_MS_TXPWR_MAX_CCH_valid;
   byte                         LB_MS_TXPWR_MAX_CCH;
} layer1_sys_info_T;

#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC
#define L1_RR_SIM_IMSI_SIZE  MM_SIM_IMSI_SIZE
#define L1_RR_SIM_TMSI_SIZE  MM_SIM_TMSI_SIZE
#define L1_RR_SIM_PTMSI_SIZE MM_SIM_PTMSI_SIZE

typedef struct
{
   uint8   imsi[L1_RR_SIM_IMSI_SIZE];
   boolean imsi_valid;
   uint8   tmsi[L1_RR_SIM_TMSI_SIZE];
   boolean tmsi_valid;
   uint8   ptmsi[L1_RR_SIM_PTMSI_SIZE];
   boolean ptmsi_valid;
   rr_l1_epd_preference_t epd_pref;
} xmsi_params_T;
#endif
/***************************************************************
 * MPH_XMSI_UPDATE_IND primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Request layer 1 to move to use the new XMSI
 * dedicated mode
 * PARAMETERS :
 ***************************************************************/

typedef struct
{
   IMH_T                   message_header;
   xmsi_params_T           xmsi;
   gas_id_t                gas_id;
} mph_xmsi_update_ind_T;

/***************************************************************
 * MPH_IMMEDIATE_ASSIGNMENT_REQUEST primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Request layer 1 to move to the assigned channel and pass in
 * dedicated mode
 * PARAMETERS :
 ***************************************************************/

typedef struct
{
   IMH_T                   message_header;
   boolean                 DTX_indicator;
   byte                    timing_advance;
   boolean                 starting_time_valid;
   byte                    starting_time[2];
   boolean                 before_parameters_valid;
   channel_information_T   channel_info_before;
   channel_information_T   channel_info_after;
   gas_id_t                gas_id;
} mph_immed_assign_req_T;


/***************************************************************
 * MPH_IMMEDIATE_ASSIGNMENT_CONFIRMATION primitive
 * FROM : LAYER 1
 * TO   : RR
 ***************************************************************/

typedef struct
{
   IMH_T                   message_header;
   byte                    error_code;
   gas_id_t                gas_id;
} mph_immed_assign_cnf_T;

/***************************************************************
 * MPH_DEDICATED_UPDATE_REQUEST primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION : Update the layer 1 with new system informations
 ***************************************************************/

typedef struct
{
   IMH_T                         message_header;
   boolean                       DTX_indicator;
   byte                          PWRC;
   byte                          NCC_permitted;
   inter_task_BCCH_allocation_T  BA_list;

   byte                          multiband_reporting;
   gas_id_t                      gas_id;
} mph_dedicated_update_req_T;

/***************************************************************
 mph_part_channel_release_cnf_T:
 Referenced only by FTM code - to be removed
***************************************************************/
typedef struct
{
   IMH_T                message_header;
   byte                 error_code;
   gas_id_t             gas_id;
} mph_part_channel_release_cnf_T;

/***************************************************************
 * MPH_CHANGE_PAGE_MODE_REQUEST primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Message requesting that layer 1 changes the paging mode
 ***************************************************************/

typedef struct
{
   IMH_T                message_header;
   paging_mode_T        paging_mode;
   gas_id_t             gas_id;
} mph_change_page_mode_req_T;


/**********************************************************************
 * MPH_SET_CIPHERING_REQUEST primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Ask the Layer 1 to start the ciphering or to stop it
 ***********************************************************************/

typedef struct
{
   IMH_T                message_header;
   boolean              cipher_flag;
   cipher_algorithm_T   cipher_algorithm;
   byte                 cipher_key[CIPHER_KEY_LENGTH];

   byte                 cipher_key_128[CIPHER_KEY_128_LENGTH];
   gas_id_t             gas_id;
} mph_set_ciphering_mode_req_T;

/**********************************************************************
 * MPH_SET_CIPHERING_CONFIRM primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * That's the confirmation from the request primitive of the same name
***********************************************************************/

typedef struct
{
   IMH_T                message_header;
   byte                 error_code;
   gas_id_t             gas_id;
} mph_set_ciphering_cnf_T;


/**************************************************************************
 * MPH_CHANNEL_ASSIGN_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * This primitive is used to order the layer 1 to go to the indicated
 * channels.
 * Here are both the description of the Channels before the starting time
 * and after the starting time
 **************************************************************************/

typedef struct
{
   IMH_T                message_header;
   channel_spec_T       channel_spec;
   multirate_config_T   multirate_config;
   gas_id_t             gas_id;
} mph_channel_assign_req_T;

/******************************************************************************
 * MPH_HANDOVER_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * After reception of this primitive, the layer 1 will setup the new channels
 ******************************************************************************/

typedef struct{

   IMH_T                  message_header;
   hand_sync_ind_T        handover_synchronisation;
   boolean                timing_advance_valid;
   byte                   timing_advance;
   ARFCN_T                BCCH_ARFCN;
   boolean                real_time_difference_valid;
   byte                   real_time_difference;
   byte                   NCI;
   channel_spec_T         channel_spec;
   rr_l1_handover_type_T  handover_type;
   multirate_config_T     multirate_config;
   rr_l1_interrat_measurement_data_T  interrat_measurement_data;
   channel_mode_T  old_channel_mode;
   channel_mode_T  new_channel_mode;
   BSIC_T          BSIC;
   byte            cipher_key[CIPHER_KEY_LENGTH];
   byte            cipher_key_128[CIPHER_KEY_128_LENGTH];
   gas_id_t        gas_id;
} mph_handover_req_T;

/******************************************************************************
 * MPH_HANDOVER_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * This is an acknowledgment of the request of the same name
 ******************************************************************************/

typedef struct
{
   IMH_T                message_header;
   byte                 error_code;
   gas_id_t             gas_id;
} mph_handover_ind_T;

/******************************************************************************
 * MPH_HANDOVER_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * This is an acknowledgment of the request of the same name
 ******************************************************************************/

typedef struct
{
   IMH_T                message_header;
   byte                 observed_time_difference[3];
   byte                 last_timing_advance;
   byte                 error_code;
   gas_id_t             gas_id;
} mph_handover_cnf_T;

/******************************************************************************
 * MPH_STOP_HANDOVER_ACCESS_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Message requesting that layer 1 stop sending handover access messages
 ******************************************************************************/

typedef struct
{
   IMH_T                message_header;
   byte                 timing_advance;
   gas_id_t             gas_id;
} mph_stop_handover_access_req_T;

/******************************************************************************
 * MPH_STOP_HANDOVER_ACCESS_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * confirmation that layer 1 has ceased sending handover access messages
 ******************************************************************************/

typedef struct
{
   IMH_T                message_header;
   byte                 observed_time_difference[3];
   byte                 last_timing_advance;
   gas_id_t             gas_id;
} mph_stop_handover_access_cnf_T;

/******************************************************************************
 * MPH_CHANNEL_MODE_MODIFY_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Request so L1 have to change the mode of the channel
 ******************************************************************************/

typedef struct
{
   IMH_T                message_header;
   channel_type_T       dedicated_channel_type;
   byte                 subchannel;
   channel_mode_T       channel_mode;
   multirate_config_T   multirate_config;
   gas_id_t             gas_id;
} mph_channel_mode_modify_req_T;

/******************************************************************************
 * MPH_CHANNEL_MODE_MODIFY_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * This is an acknowledgment of the request of the same name
 ******************************************************************************/

typedef struct
{
   IMH_T                message_header;
   byte                 error_code;
   gas_id_t             gas_id;
} mph_channel_mode_modify_cnf_T;

/******************************************************************************
 * MPH_FREQUENCY_REDEFINITION_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Request so L1 can update its frequency list in the hopping sequences
 ******************************************************************************/

typedef struct
{
   IMH_T                 message_header;
   byte                  starting_time[2];
   channel_information_T channel_information;
   gas_id_t              gas_id;
} mph_frequency_redefinition_req_T;

/******************************************************************************
 * MPH_FREQUENCY_REDEFINITION_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Legacy primitive, superceded by MPH_FREQUENCY_REDEFINITION_IND: keep for UTF
 ******************************************************************************/

typedef struct
{
   IMH_T                 message_header;
} mph_frequency_redefinition_cnf_T;

/******************************************************************************
 * MPH_FREQUENCY_REDEFINITION_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Indication of success/failure of frequency redefinition
 ******************************************************************************/

typedef struct
{
  IMH_T                 message_header;
  boolean               channels_in_same_band;
  boolean               starting_time_elapsed;
  gas_id_t              gas_id;
} mph_frequency_redefinition_ind_T;


/******************************************************************************
 * MPH_DECODE_BCCH_LIST_ABORT_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Requests L1 to abort any ongoing BCCH list decode and go to the NULL state.
 * If not already in NULL L1 should send a MPH_DECODE_BCCH_LIST_CNF
 ******************************************************************************/
typedef struct
{
  IMH_T                 message_header;
   gas_id_t             gas_id;
} mph_decode_bcch_list_abort_req_T;

/******************************************************************************
 * MPH_POWER_SCAN_ABORT_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Requests L1 to abort any ongoing Power Scan and go to the NULL state.
 * This is used only when the background PLMN search is aborted for some reason
 ******************************************************************************/
typedef struct
{
  IMH_T                 message_header;
   gas_id_t             gas_id;
} mph_power_scan_abort_req_T;


/* Search reasons */
typedef enum {
   RR_SEARCH_NORMAL,
   RR_SEARCH_BACKGROUND,
   RR_SEARCH_RESELECTION
} rr_search_mode_T;

/******************************************************************************
 * MPH_DECODE_BCCH_LIST_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Requests L1 to decode the BCCH in the frequency list. If a failure occurs a
 * MPH_DECODE_BCCH_LIST_FAILURE_IND should be sent for each failed frequency. A
 * MPH_FCCH_SCH_DECODE_IND should be sent for each successfully decoded cell to 
 * indicate FCCH SNR of that cell. A
 * MPH_DECODE_BCCH_LIST_CNF should be sent when L1 is finished and
 * transistioned to the NULL state
 *******************************************************************************/
typedef struct
{
  IMH_T                   message_header;
  rr_search_mode_T        search_mode;
  rr_l1_idle_freq_list_T  frequency_list;

  /* To tell L1 retry decoding bcch with reset the PDM params   */
  boolean                 retry_with_reset;

  boolean                 si3_only;   // Only read SI3, not SI3/SI4
  gas_id_t                gas_id;
} mph_decode_bcch_list_req_T;

/******************************************************************************
 * MPH_DECODE_BCCH_LIST_CNF primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * L1 has completed decoding the SCH and BCCH on all the frequencies sent in the
 * MPH_DECODE_BCCH_LIST_REQ and has transitioned to the NULL state
 *******************************************************************************/
typedef struct
{
  IMH_T                   message_header;

  /* Used in W2G BPLMN search. If it is TRUE, after receiving decode_bcch_list_cnf,
   * If need to send another MPH_DECODE_BCCH_LIST_REQ, RR will hold off it until
   * the next round.
   */
  boolean                 suspending;

  /* To indicate RR if L1 requires another round of PLMN selection procedure */
  boolean                 retry_required;
   gas_id_t               gas_id;
} mph_decode_bcch_list_cnf_T;

/******************************************************************************
 * MPH_DECODE_BCCH_LIST_FAILURE_IND primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * L1 was unable to decode the BCCH of a frequency specified in the
 * MPH_DECODE_BCCH_LIST_REQ
 *******************************************************************************/
typedef struct
{
  IMH_T                            message_header;
  ARFCN_T                          frequency;
  rr_l1_decode_bcch_failure_T      failure_cause;
  gas_id_t                         gas_id;
} mph_decode_bcch_list_failure_ind_T;

/******************************************************************************
 * MPH_POWER_SCAN_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Requests L1 to do a power scan on the requested frequencies, results should
 * be put into the buffer and sorted by signal strength. When L1 completes the
 * power scan and transitions to the NULL state it will send a MPH_POWER_SCAN_CNF
 *******************************************************************************/
typedef struct
{
  IMH_T                   message_header;
  rr_search_mode_T        search_mode;
  rr_l1_pscan_results_T*  pscan_results_buffer_ptr;
  rr_l1_band_bitmap_T     requested_bands;
#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
  boolean                 toggle_antenna;
#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */
  gas_id_t                gas_id;
} mph_power_scan_req_T;

/******************************************************************************
 * MPH_POWER_SCAN_CNF primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * L1 has completed the power scan requested
 *******************************************************************************/
typedef struct
{
  IMH_T                   message_header;

  /* Used in W2G BPLMN search. If it is TRUE, after receiving powe_scan_cnf,
   * RR will hold off sending MPH_DECODE_BCCH_LIST_REQ to the next round.
   */
  boolean                 suspending;
  boolean                 invalidate; /* if set, GRR should not consider the results valid */
  gas_id_t                gas_id;
} mph_power_scan_cnf_T;

/******************************************************************************
 * MPH_SPECIFIC_BCCH_DECODE_FAILURE_IND primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * L1 was unable to decode the BCCH of a frequency specified in the
 * MPH_SELECT_SPECIFIC_BCCH_REQ
 *******************************************************************************/
typedef struct
{
  IMH_T              message_header;
  ARFCN_T            BCCH_ARFCN;
  gas_id_t           gas_id;
} mph_specific_bcch_decode_failure_ind_T;

/******************************************************************************
 * MPH_START_IDLE_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * PLMN has been selected, system data indicates that camping is allowed
 * on the selected channel, so the mobile can camp on it
 ******************************************************************************/

typedef struct
{
   IMH_T              message_header;
   layer1_sys_info_T  layer1_sys_info;
   boolean            imsi_valid;
   imsi_mod_1000_T    imsi_mod_1000;
   boolean            decode_bcch;
#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC
   xmsi_params_T      xmsi;
#endif
   gas_id_t           gas_id;
} mph_start_idle_req_T;

/******************************************************************************
 * MPH_EARLY_CAMPING_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Asks L1 to do early camping based on the info we received in SI3, will update
 * them later after we receive the full system information
 ******************************************************************************/

typedef struct
{
   IMH_T              message_header;
   byte               BS_AG_BLKS_RES;
   byte               BS_CC_CHANS;
   byte               BS_CCCH_SDCCH_COMB;
   byte               BS_PA_MFRMS;
   boolean            imsi_valid;
   imsi_mod_1000_T    imsi_mod_1000;
   paging_mode_T      paging_mode;     /*either PAGING reorg/Normal */
#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC
   xmsi_params_T      xmsi;
#endif
   gas_id_t           gas_id;
} mph_early_camping_req_T;

/******************************************************************************
 * MPH_IDLE_UPDATE_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * A change in the system information has been detected.
 * We have to update the parameters in the layer 1
 ******************************************************************************/

typedef struct
{
   IMH_T              message_header;
   layer1_sys_info_T  layer1_sys_info;
   boolean            imsi_valid;
   imsi_mod_1000_T    imsi_mod_1000;
#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC
   xmsi_params_T      xmsi;
#endif
   gas_id_t           gas_id;
} mph_idle_update_req_T;


/******************************************************************************
 * MPH_RESELECTION_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 *
 ******************************************************************************/
typedef struct
{
   IMH_T             message_header;
   boolean           BCCH_found;
   boolean           state_changed;
   gas_id_t          gas_id;
} mph_reselection_cnf_T;


/******************************************************************************
 * MPH_SURROUND_UPDATE_FAILURE_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Indicates that L1 was unable to decode surrounding BCCH for a given channel
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   ARFCN_T           BCCH_ARFCN;
   boolean           state_changed;
   gas_id_t          gas_id;
} mph_surround_update_failure_ind_T;


/******************************************************************************
 * MPH_RECONNECT_CHANNEL_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Message requesting that layer 1 reconnects the old channels
 ******************************************************************************/

typedef struct
{
   IMH_T                message_header;
   channel_spec_T       channel_spec;
   multirate_config_T   multirate_config;
   gas_id_t             gas_id;
} mph_reconnect_channel_req_T;

/******************************************************************************
 * MPH_RECONNECT_CHANNEL_CNF primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * The layer 1 has reconnected the old channels
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   gas_id_t          gas_id;
} mph_reconnect_channel_cnf_T;

/******************************************************************************
 * MPH_READ_SYS_INFO_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Message telling layer 1 to read system information messages on the
 * camped-on cell's BCCH channel.
 *
 * If replace_prev_mask is TRUE then L1 will discard whatever mask it was
 * using previously and use si_mask from this request.  If replace_prev_mask
 * is FALSE then L1 will OR si_mask from this request with its existing
 * si_mask.
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   rr_l1_si_type_T   si_mask;
   boolean           replace_prev_mask;
   gas_id_t          gas_id;
} mph_read_sys_info_req_T;

/******************************************************************************
 * MPH_READ_SYS_INFO_FAILURE_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Message telling RR that MPH_READ_SYS_INFO_REQ cannot be completed.
 *
 ******************************************************************************/
typedef struct
{
   IMH_T             message_header;
   boolean           in_transfer;
   boolean           state_changed;
   gas_id_t          gas_id;
} mph_read_sys_info_failure_ind_T;

/******************************************************************************
 * MPH_FCCH_SCH_DECODE_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Message telling RR that an FCCH/SCH acquisition has been successful.
 * It provides FCCH SNR of a cell.
 *
 ******************************************************************************/
typedef struct
{
  IMH_T    message_header;
  ARFCN_T  BCCH_ARFCN;
  BSIC_T   BSIC;
  byte     RXLEV_average;
#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
  uint16   fcch_snr;
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
  gas_id_t gas_id;
} mph_fcch_sch_decode_ind_T;

/******************************************************************************
 * MPH_BLOCK_QUALITY_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Indication as to whether the PCH or SACCH block was decoded correctly or not
 ******************************************************************************/
typedef enum
{
  GL1_BLOCK_QUALITY_BAD  = 0,
  GL1_BLOCK_QUALITY_GOOD = 1,
  GL1_BLOCK_QUALITY_UNKNOWN
} gl1_block_quality_t;

typedef struct
{
   IMH_T               message_header;
   l2_channel_type_T   l2_channel_type;
   gl1_block_quality_t block_quality;
   timetick_type       block_timestamp;
   uint32              bplmn_fail_count;
   gas_id_t            gas_id;
} mph_block_quality_ind_T;

/******************************************************************************
 * MPH_SERVING_IDLE_MEAS_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Serving cell measurement report in idle mode
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   byte              RXLEV_average;
   byte              SCH_block[SCH_BLOCK_SIZE];
   word              trk_lo_adj;
   byte              RXLEV_instant;
   boolean           SNR_is_bad;
   gas_id_t          gas_id;
} mph_serving_idle_meas_ind_T;

/******************************************************************************
 * MPH_SERVING_AUX_MEAS_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Serving cell auxiliary measurement report
 ******************************************************************************/
typedef struct
{
   IMH_T             message_header;
   byte              RXLEV_instant;
   boolean           SNR_is_bad;
   boolean           own_paging_block;
   gas_id_t          gas_id;
} mph_serving_aux_meas_ind_T;

/******************************************************************************
 * MPH_SURROUND_MEAS_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Report of the power levels and SCH information for the 6 best surrounding
 * cells
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   best_six_info_T   best_six_info;
   gas_id_t          gas_id;
} mph_surround_meas_ind_T;

/******************************************************************************
 * MPH_ABORT_RA_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * To stop L1 sending access bursts while still staying in RA mode to receive
 * Immediate assignment messages
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   gas_id_t          gas_id;
} mph_abort_ra_req_T;

/******************************************************************************
 * MPH_ABORT_RA primitive
 * DESCRIPTION : for legacy compatibility only
 ******************************************************************************/

typedef mph_abort_ra_req_T mph_abort_ra_T;

/******************************************************************************
 * MPH_ABORT_RA_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * L1 confirms its current mode after aborting random access
 ******************************************************************************/
typedef enum
{
  GL1_IN_NULL_MODE = 0,
  GL1_IN_RACH_MODE,
  GL1_IN_IDLE_MODE,
  GL1_IN_DECOUPLED_MODE
} gl1_mode_t;

typedef struct
{
   IMH_T             message_header;
   gl1_mode_t        l1_mode;
   gas_id_t          gas_id;
} mph_abort_ra_cnf_T;

/************************************************************/
/*          --------GPRS-------                             */
/*                                                          */
/*  Code appended to support GPRS RR <-> L1 primitives      */
/************************************************************/

/**********************************************************************/
/* Additional structure/enum components to support GPRS L1 primitives */
/**********************************************************************/
typedef enum{
  NON_HOPPING,
  HOPPING
}hopping_flag_t;


typedef enum{
  BCCH_NORMAL,
  BCCH_EXTENDED
}normal_extended_t;

#define EXTENDED_MSG BCCH_EXTENDED    /* to be removed */
#define NORMAL_MSG   BCCH_NORMAL      /* to be removed */

typedef enum{
  CCCH_51,
  PCCCH_52
}channel_t;


/****************************************************************/
/*  structure ccch_power_control_params_t:-                     */
/*  --------------------                                        */
/*  Contains a list of power control parameters read from system*/
/*  information. Sent to L1 in the MPH_START_GPRS_IDLE_REQ and  */
/*  MPH_GPRS_L1PARAM_UPDATE_REQ                                 */
/****************************************************************/
typedef struct{
  byte     alpha;
  byte     t_avg_w;
  byte     t_avg_t;
  boolean  pc_meas_chan;
  byte     n_avg_i;
}ccch_power_control_params_t;

/****************************************************************/
/*  structure pccch_power_control_params_t:-                    */
/*  --------------------                                        */
/*  Contains all power control parameters for PCCCH             */
/*  Sent in the primitive MPH_START_GPRS_IDLE_REQ and           */
/*  MPH_L1PARAM_UPDATE_REQ                                      */
/****************************************************************/
typedef struct{
  byte             alpha;
  byte             t_avg_w;
  byte             t_avg_t;
  boolean          pc_meas_chan;
  byte             n_avg_i;
}pccch_power_control_params_t;

/****************************************************************/
/*  structure ccch_idle_mode_params_t:-                         */
/*  --------------------                                        */
/*  Contains all list of all the system information needed by   */
/*  L1 to camp onto CCCH successfully.                          */
/****************************************************************/
typedef struct
{
  imsi_mod_1000_T              imsi_mod1000;
  byte                         non_drx_timer;
  byte                         drx_timer_max;
  ccch_power_control_params_t  power_control;
  byte                         nmo;
  boolean                      access_burst_type;
  paging_mode_T                paging_mode;
  boolean                      decode_bcch;
  uint8                        bep_period;
  boolean                      ext_utbf_nodata;
#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC
  xmsi_params_T                xmsi;
#endif
  boolean                      read_pch_in_transfer;
}ccch_gprs_idle_params_t;

typedef struct
{
  layer1_sys_info_T        layer1_sys_info;
  ccch_gprs_idle_params_t  layer1_extra_gprs_sys_info;
}ccch_idle_mode_params_t;

/****************************************************************/
/*  structure pccch_idle_mode_params_t:-                        */
/*  --------------------                                        */
/*  Contains all list of all the system information needed by   */
/*  L1 to camp onto PCCCH successfully.                         */
/****************************************************************/
typedef struct
{
  byte                          drx_timer_max;
  pccch_power_control_params_t  power_control;
  byte                          gprs_ms_txpwr_max_cch;
}pccch_idle_mode_params_t;

/****************************************************************/
/*  union pccch_idle_mode_params_t:-                            */
/*  ------------------------------                              */
/*  Contains either CCCH system information or PCCCH system info*/
/****************************************************************/
typedef union{
  ccch_idle_mode_params_t   ccch_idle_mode_params;
}ccch_pccch_params_t;


/******************************************************************************
gprs_surround_data_t
--------------------
Structure which tells L1 which sys info messages to read from a surround cell.
This is a union of parameters for a BCCH read and parameters for a PBCCH read.

grr_l1_surround_cell_si_requested_T
-----------------------------------
enum indicating the required sys info messages for the surround read

GRR_L1_DECODE_SI3_AND_SI13
Read SI3 and SI13 from the surround cell. If the SI3 read fails, get SI4 instead.

GRR_L1_DECODE_SI3_ONLY
Read SI3 only from the surround cell. If the SI3 read fails, get SI4 instead.

GRR_L1_DECODE_SI7_AND_SI13
Read SI7 and SI13 from the surround cell. If the SI7 read fails, get SI8 instead.

GRR_L1_DECODE_SI7_ONLY
Read SI7 only from the surround cell. If the SI7 read fails, get SI8 instead.
*******************************************************************************/
typedef enum
{
  GRR_L1_DECODE_SI3_AND_SI13,
  GRR_L1_DECODE_SI3_ONLY,
  GRR_L1_DECODE_SI7_AND_SI13,
  GRR_L1_DECODE_SI7_ONLY
} grr_l1_surround_cell_si_requested_t;

typedef struct{
  ARFCN_T                              bcch_arfcn;
  BSIC_T                               bcch_bsic;
  grr_l1_surround_cell_si_requested_t  si_requested;
  normal_extended_t                    si13_normal_or_ext;
}bcch_t;


typedef union{
  bcch_t   bcch_params;
}gprs_surround_data_t;


/*##############################################################*/
/* ------DEFINITION OF GPRS RR -> L1 PRIMITIVES-------      */
/*##############################################################*/



/******************************************************************************
 * MPH_START_GPRS_IDLE_MODE_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Requests that L1 starts idle mode using the data within the primitive
 ******************************************************************************/
typedef struct
{
  IMH_T                message_header;
  channel_t            ccch_or_pccch;
  ccch_pccch_params_t  idle_mode_params;
  gas_id_t             gas_id;
}mph_start_gprs_idle_mode_req_t;

/******************************************************************************
 * MPH_GPRS_L1PARAM_UPDATE_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Sends to L1 any L1 specific system information that has changed. This may be
 * sent in idle or transfer.
 ******************************************************************************/
typedef struct
{
  IMH_T                message_header;
  channel_t            ccch_or_pccch;
  ccch_pccch_params_t  l1_system_info_params;
  gas_id_t             gas_id;
}mph_gprs_l1param_update_req_t;

/******************************************************************************
 * MPH_NC_MEASUREMENTS_REQ
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Requests that L1 performs NC measurements on the frequencies within the message
 ******************************************************************************/
typedef struct
{
  IMH_T                         message_header;
  inter_task_BCCH_allocation_T  balist;
  gas_id_t                      gas_id;
}mph_nc_measurements_req_t;

/******************************************************************************
 * MPH_NC_MEASUREMENTS_REQ
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Requests that L1 stops NC measurements and resumes normal measurements
 * on the frequencies within the message (these are the standard BA list)
 ******************************************************************************/
typedef struct
{
  IMH_T                         message_header;
  inter_task_BCCH_allocation_T  balist;
  gas_id_t                      gas_id;
}mph_stop_nc_measurements_req_t;

/******************************************************************************
 * MPH_GPRS_SURROUND_UPDATE_REQ
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Requests that L1 stops Extended measurements
 ******************************************************************************/
typedef struct
{
  IMH_T                 message_header;
  gprs_surround_data_t  gprs_surround_data;
  boolean               cancel_non_drx;
  gas_id_t              gas_id;
}mph_gprs_surround_update_req_t;

/******************************************************************************
 * MPH_GPRS_RESELECTION_REQ
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Requests that L1 reselects to the cell described
 ******************************************************************************/
typedef struct
{
  IMH_T                 message_header;
  gprs_surround_data_t  gprs_surround_data;
  boolean               sync_flag;
  gas_id_t              gas_id;
}mph_gprs_reselection_req_t;


/******************************************************************************
 * MPH_STOP_SURROUND_UPDATE_REQ
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Requests that L1 stops the decoding of surround bcch SI's. This is issued
 * once SI3, SI4 and SI13(if it exists) have all been recieved.
 ******************************************************************************/
typedef struct
{
  IMH_T         message_header;
  gas_id_t      gas_id;
}mph_stop_surround_update_req_t;


/******************************************************************************
 * MPH_START_MM_NON_DRX_IND
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Indicates to L1 that MM has entered its non-DRX period
 ******************************************************************************/
typedef struct
{
  IMH_T         message_header;
  gas_id_t      gas_id;
}mph_start_mm_non_drx_ind_t;


/******************************************************************************
 * MPH_STOP_MM_NON_DRX_IND
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Indicates to L1 that MM has left its non-DRX period
 ******************************************************************************/
typedef struct
{
  IMH_T         message_header;
  gas_id_t      gas_id;
}mph_stop_mm_non_drx_ind_t;


/*##############################################################*/
/* ------DEFINITION OF GPRS RR <- L1 PRIMITIVES-------      */
/*##############################################################*/

/******************************************************************************
 * MPH_NC_MEASUREMENT_IND
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Returns the NC measurements that were requested by RR
 ******************************************************************************/
typedef struct
{
  IMH_T                       message_header;
  byte                        no_of_entries;
  surrounding_channel_info_T  surrounding_channel_info[BA_MAX_LEN];
  gas_id_t                    gas_id;
}mph_nc_measurement_ind_t;





/******************************************************************************
 * MPH_START_GSM_MODE_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * To Request L1 to start functioning in GSM Mode.
 ******************************************************************************/

typedef struct
{
   IMH_T                         message_header;
   boolean                       gsm_only;

   /* TRUE if L1 is being started for an X2G reselection.  If this flag is TRUE
   L1 should disable sleep until receiving an MPH_START_GPRS_IDLE_MODE_REQ,
   MPH_START_IDLE_REQ or MPH_IDLE_UPDATE_REQ. */
   boolean                       x2g_resel_in_progress;

/* TRUE if L1 is being started for an X2G CCO procedure.  If this flag is
   TRUE, L1 should disable sleep until receiving an MPH_ENABLE_SLEEP_IND. */
   boolean                       x2g_cco_in_progress;

   gas_id_t                      gas_id;
   sys_modem_device_mode_e_type  multi_sim_mode;
   int                           num_sims_used;
   boolean                       sglte_mode_active;
} mph_start_gsm_mode_req_T;

/******************************************************************************
 * MPH_DEVICE_MODE_IND primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Indicates the latest multi-SIM device mode to Layer 1
 ******************************************************************************/
typedef enum
{
  GERAN_DEVICE_MODE_CAUSE_MMCP = 0,
  GERAN_DEVICE_MODE_CAUSE_PLMN,
  GERAN_DEVICE_MODE_CAUSE_NONE
} geran_device_mode_cause_t;

typedef struct
{
  IMH_T                        message_header;
  sys_modem_device_mode_e_type multi_sim_mode;
  geran_device_mode_cause_t    device_mode_cause;
  gas_id_t                     gas_id;
} mph_device_mode_ind_T;

/******************************************************************************
 * MPH_SET_PRIORITY_IND primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Indicates the latest multi-SIM priority settings to Layer 1
 ******************************************************************************/
typedef enum
{
  GERAN_ACQ_PRIORITY_NOMINAL = 0,
  GERAN_ACQ_PRIORITY_INVERTED,
  GERAN_ACQ_PRIORITY_MAXIMAL,
  GERAN_MMS_PRIORITY_NOMINAL,
  GERAN_MMS_PRIORITY_INVERTED,
  GERAN_MMS_PRIORITY_MAXIMAL,
  GERAN_PRIORITY_NOT_CHANGED,
  GERAN_PRIORITY_UNDEFINED
} geran_priority_t;

typedef struct
{
  IMH_T            message_header;
  geran_priority_t l1_priority;
  boolean          new_procedure;
  gas_id_t         gas_id;
} mph_set_priority_ind_T;

/******************************************************************************
 * MPH_STOP_GSM_MODE_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * To Request L1 to stop functioning in GSM Mode.
 ******************************************************************************/

typedef struct
{
   IMH_T                   message_header;
   rr_l1_gsm_stop_reason_T rr_l1_gsm_stop_reason;
   boolean                 g2x_resel_in_progress;
   gas_id_t                gas_id;
} mph_stop_gsm_mode_req_T;

/******************************************************************************
 * MPH_START_GSM_MODE_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Confirmation from L1 regarding GSM Mode Start/Activation.
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   gas_id_t          gas_id;
} mph_start_gsm_mode_cnf_T;

/******************************************************************************
 * MPH_STOP_GSM_MODE_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Confirmation from L1 regarding GSM Mode Stop/Deactivation.
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   gas_id_t          gas_id;
} mph_stop_gsm_mode_cnf_T;

#define MAX_RR_MPH_FREQ_UPDATE_LIST   8
#define MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST 9

typedef struct
{
  word uarfcn[MAX_RR_MPH_FREQ_UPDATE_LIST];
  byte num_of_frequencies;
} wcdma_reselection_freq_list_T;

typedef struct
{
  byte num_of_frequencies;
  word uarfcn[MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST];
} utran_tdd_reselection_freq_list_T;

/*Struct to store frequencies with a scrambling code*/
typedef struct
{
   word    uarfcn;
   word    scrambling_code;
   boolean diversity;
} wcdma_cell_T;

#define MAX_RR_MPH_CELL_UPDATE_LIST   96

typedef struct
{
  byte num_of_cells;
  wcdma_cell_T  cell_list [MAX_RR_MPH_CELL_UPDATE_LIST];
} wcdma_cell_list_T;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

/* Release 8 Priority Reselection Information */

typedef enum
{
  RR_L1_IRAT_RAT_UNDEFINED,
  RR_L1_IRAT_RAT_WCDMA,
  RR_L1_IRAT_RAT_LTE,
  RR_L1_IRAT_RAT_UTRAN_TDD,
  RR_L1_IRAT_RAT_MAX
} rr_l1_irat_rat_e;

typedef struct
{
  rr_l1_irat_rat_e      rat;      /*!< RAT that the frequency belongs to. */
  uint32                arfcn;    /*!< LTE EARFCN of WCDMA UARFCN. */
  uint8                 priority; /*!< Priority associated with the frequency, range 0 to 7 */
} rr_l1_irat_frequency_priority_t;


/* Define the maximum number of frequency/priority mappings that RR needs
to be able to send to L1 - this is a guess at the moment. */
#define MAX_RR_L1_PRIORITY_ENTRIES 16

/* For every unique IRAT frequency that RR sends to L1 in the
MPH_WCDMA_CELL_UPDATE_LIST_REQ RR will include a frequency->priority
mapping. */

typedef struct
{
  /* If count is greater than zero this implies that priority
  information is available, and RR should apply the priority/threshold
  settings. */
  uint8 count;

  /* Mapping of IRAT frequency -> priority for all frequencies sent to L1. */
  rr_l1_irat_frequency_priority_t entries[MAX_RR_L1_PRIORITY_ENTRIES];

} rr_l1_irat_frequency_priority_list_t;


typedef struct
{
  /*! Length of the value of 'pattern' in bits. */
  uint8 pattern_length;

  /*! Bit pattern. */
  uint8 pattern;

  /*! TBD. */
  uint8 pattern_sense;
} rr_l1_psc_pattern_t;

typedef enum
{
  RR_L1_UARFCN_TYPE_FDD,
  RR_L1_UARFCN_TYPE_TDD
} rr_l1_uarfcn_type_e;

/*! This structure defines a set of PSCs associated with an UARFCN.

If psc_count and psc_pattern_count are both zero then all PSCs on the frequency
belong to the CSG.
*/
#define MAX_PSC_COUNT              32
#define MAX_PSC_PATTERN_COUNT       8
typedef struct
{
  rr_l1_uarfcn_type_e uarfcn_type;
  uint16              uarfcn;

  uint32              psc_count;
  uint16              psc[MAX_PSC_COUNT];

  uint32              pcsc_pattern_count;
  rr_l1_psc_pattern_t pcsc_pattern[MAX_PSC_PATTERN_COUNT];
} rr_l1_irat_utran_csg_description_t;

#ifdef FEATURE_LTE
#define MAX_RR_L1_EUTRAN_FREQUENCIES 16  

typedef enum
{
  RR_L1_EUTRAN_TYPE_FDD,
  RR_L1_EUTRAN_TYPE_TDD
} rr_l1_eutran_type_e;

/*! The size of the PCID Group Bitmap.

  There are 504 possible PCIDs, ranging from 0 - 503.  These are represented
  by a 63 byte array, and associated macros to set / check whether bits are
  set.
*/
#define RR_L1_PCID_MIN 0
#define RR_L1_PCID_MAX 503
#define RR_L1_PCID_GROUP_BITMAP_LEN 63

/*! Utility macro for setting a PCID bit in the PCID Group bitmap. */
#define RR_L1_PCID_GROUP_BITMAP_SET(pcid_group_bitmap, pcid) \
  ( (pcid_group_bitmap)[(pcid)/8] |= (1<<((pcid) % 8)))

/*! Utility macro for checking whether a PCID bit is set in the PCID Group
bitmap. */
#define RR_L1_PCID_GROUP_BITMAP_IS_SET(pcid_group_bitmap, pcid) \
  (( (pcid_group_bitmap)[(pcid)/8] & (1<<((pcid) % 8))) ? TRUE : FALSE )

typedef struct
{
  /*! This field specifies the E-UTRA Absolute Radio Frequency Channel Number
  as defined in 3GPP TS 36.104. */
  lte_earfcn_t earfcn;

  /*! This field specifies the minimum value of the channel bandwidth of all
  valid E-UTRAN cells on the specified EARFCN. It is defined by the parameter
  Transmission Bandwidth Configuration, NRB (see 3GPP TS 36.104). The values
  indicate the number of resource blocks over which the mobile station could
  measure. The field is coded according to the following table:

  bit
  3 2 1
  0 0 0 NRB = 6
  0 0 1 NRB = 15
  0 1 0 NRB = 25
  0 1 1 NRB = 50
  1 0 0 NRB = 75
  1 0 1 NRB = 100
  */
  uint8 measurement_bandwidth;

  /*! Indicate whether the LTE EARFCN is FDD or TDD. */
  rr_l1_eutran_type_e type;

  /*! Not Allowed Cells.
  This field is a bitmap and should be manipulated using the
  RR_L1_PCID_GROUP_BITMAP_SET and RR_L1_PCID_GROUP_BITMAP_IS_SET macros.
  Bit 0 represents PCID 0, bit 503 represents PCID 503, etc.  If a bit
  position is set to 1 this indicates that the corresponding PCID is part
  of the set of Not Allowed Cells for this EARFCN, and can be ignored during
  measurements on this frequency. */
  uint8 not_allowed_cells[RR_L1_PCID_GROUP_BITMAP_LEN];

} rr_l1_eutran_frequency_t;
#endif /* FEATURE_LTE */

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

typedef enum
{
  UTRAN_TDD_3_84MCPS,
  UTRAN_TDD_1_28MCPS
} utran_tdd_bandwidth_t;

typedef struct
{
   uint16    uarfcn;
   uint16    cell_parameter_id; /* 0..127 see 25.213 */
   uint8     sync_case; /* For 3.84Mcps 0-Sync Case 1, 1-Sync Case 2.
                           For 1.28Mcps 0-TSTD not applied, 1-TSTD is applied */
   boolean   diversity;
} utran_tdd_cell_T;

typedef struct
{
  byte              num_of_cells;
  utran_tdd_cell_T  cell_list [MAX_RR_MPH_CELL_UPDATE_LIST];
} utran_tdd_cell_list_T;

typedef struct
{
  IMH_T                                 message_header;
  word                                  WCDMA_QSearch_I;
  byte                                  FDD_Qmin;
  boolean                               THREE_G_SEARCH_PRIO;
  wcdma_reselection_freq_list_T         wcdma_reselection_freq_list;
  wcdma_cell_list_T                     wcdma_cell_list;

#ifdef FEATURE_LTE

  /*! Defines whether or not the priority-based reselection algorithm should
  be used. */
  boolean                               use_priority_reselection_algo;

  /*! This controls the monitoring of IRAT frequencies with lower priority than
  the serving cell.  If the RLA_C of the serving cell is below
  THRESH_priority_search, the mobile station shall monitor cells if IRAT
  frequencies of lower priority than the serving cell.  When RLA_C of the
  serving cell is above the threshold, the mobile stations is allowed not to
  monitor cells of IRAT frequencies of lower priority than the serving cell.

  0 = -98 dBm,
  1 = -95 dBm,
  2 = -92 dBm,
  ...,
  13 = -59 dBm,
  14 = -56 dBm,
  15 = (always).
  */
  uint8 THRESH_priority_search;

  /*! Priority of GERAN.  Range 0 - 7. */
  uint8 geran_priority;

  /*! Each IRAT neighbour frequency included in the 3G and E-UTRAN has an
  associated priority. */
  rr_l1_irat_frequency_priority_list_t frequency_priority_list;

  /*! UTRAN CSG Description defines frequencies and/or Primary Scrambling Codes
  associated with CSG cells. */
  uint8                              utran_csg_description_count;
  rr_l1_irat_utran_csg_description_t utran_csg_description[MAX_RR_MPH_FREQ_UPDATE_LIST];

  /*! This structure include the frequencies belonging to the E-UTRAN
  neighour list. */
  uint8                    eutran_frequency_count;
  rr_l1_eutran_frequency_t eutran_frequencies[MAX_RR_L1_EUTRAN_FREQUENCIES];
    
/*! Search for E-UTRAN cells if signal level below threshold (0-7):
    - 98, - 94, … , - 74 dBm, infinity dBm (always)
  or above threshold (8-15):
    - 78, - 74, … , - 54 dBm, infinity dBm (never)
  */
  uint8                   qsearch_c_eutran;

#endif /* FEATURE_LTE */

  utran_tdd_bandwidth_t              utran_tdd_bandwidth;
  utran_tdd_reselection_freq_list_T  utran_tdd_reselection_freq_list;
  utran_tdd_cell_list_T              utran_tdd_cell_list;
  gas_id_t                           gas_id;
} mph_wcdma_cell_update_list_req_T;

typedef struct
{
    IMH_T   message_header;
    word    uarfcn; /*UTRAN FDD or TDD*/
    word    scrambling_code;
    boolean ignore_flag;
    boolean threshold_valid;
    uint16   s_qualmin;
    uint16   s_rxlevmin;
    uint16   cell_parameter_id; /* UTRAN TDD 0..127 see 25.213 */
    gas_id_t gas_id;
} mph_ignore_wcdma_neighbors_req_T;

/*Struct to store frequencies with a scrambling code*/
typedef struct
{
   word        UARFCN;
   word        Scrambling_code;
   int16       RSCP;
   int16       RSSI;
   int16       EcNo;
   uint16      PN_Offset;
} surrounding_wcdma_channel_info_T;

typedef struct
{
   byte no_of_entries;
   surrounding_wcdma_channel_info_T surrounding_wcdma_channel_info[MAX_NUMBER_OF_BEST_WCDMA_CELLS];
} best_six_wcdma_info_T;

/******************************************************************************
 * MPH_SURROUND_WCDMA_IDLE_MEAS_IND primitive
 * FROM : L1
 * TO   : RR
 * DESCRIPTION :
 * Sent by L1 to RR containing up to 6 Strongest WCDMA neighbor cells.
 ******************************************************************************/

typedef struct
{
   IMH_T                   message_header;
   best_six_wcdma_info_T   best_six_wcdma_info;
   gas_id_t                gas_id;
} mph_surround_wcdma_idle_meas_ind_T;

#if defined (FEATURE_WCDMA) || defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)

typedef struct
{
  uint16 scrambling_code;
  /*!< Scrambling Code of the cell. */

  boolean diversity;
  /*!< Diversity value for the cell. */

  int16  rscp;
  /*!< RSCP of the cell. */

  int16  ecno;
  /*!< Ec/No of the cell. */

  int16  rssi;
  /*!< RSSI of the frequency. */

  uint16 pn_offset;
  /*!< PN offset of the cell. */

} rr_l1_wcdma_measurement_t;

/*!
  Structure containing measurements for WCDMA neighbour cells on a single
  frequency.
*/
typedef struct
{
  uint16 uarfcn;
  /*!< UARFCN that the measurements in this structure are taken on. */

  uint16 no_of_entries;
  /*!< Number of cells that are included in the set of measurements. */

  rr_l1_wcdma_measurement_t entries[MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY];
  /*!< List of cells and their measurements. */

} rr_l1_wcdma_measurements_t;

typedef struct
{
  uint16 cell_parameter_id;
  /*!< Cell Parameter ID of the cell. */

  boolean diversity;
  /*!< Diversity value for the cell. */

  int16  rscp;
  /*!< RSCP of the cell. */

  int16  rssi;
  /*!< RSSI of the frequency. */
  
} rr_l1_utran_tdd_measurement_t;

/*!
  Structure containing measurements for UTRAN TDD neighbour cells on a single
  frequency.
*/
typedef struct
{
  uint16 uarfcn;
  /*!< UARFCN that the measurements in this structure are taken on. */

  uint16 ts0_pos;
  /*!< TS0 position of strongest cell in UARFCN. */

  uint16 no_of_entries;
  /*!< Number of cells that are included in the set of measurements. */

  rr_l1_utran_tdd_measurement_t entries[MAX_UTRAN_TDD_MEASUREMENTS_PER_FREQUENCY];
  /*!< List of cells and their measurements. */

} rr_l1_utran_tdd_measurements_t;

#ifdef FEATURE_LTE
typedef struct
{
  uint16 pcid;
  /*!< PCID of the cell. */

  int16 rsrp;
  /*!< RSRP of the cell. */

  int16 rsrq;
  /*!< RSRP of the cell. */

} rr_l1_lte_measurement_t;

/*!
  Structure containing measurements for LTE neighbour cells on a single
  frequency.
*/
typedef struct
{
  lte_earfcn_t earfcn;
  /*!< UARFCN that the measurements in this structure are taken on. */

  uint16 no_of_entries;
  /*!< Number of cells that are included in the set of measurements. */

  rr_l1_lte_measurement_t entries[MAX_LTE_MEASUREMENTS_PER_FREQUENCY];
  /*!< List of cells and their measurements. */

} rr_l1_lte_measurements_t;
#endif /* FEATURE_LTE */

typedef union
{
  rr_l1_wcdma_measurements_t     wcdma;
#ifdef FEATURE_LTE
  rr_l1_lte_measurements_t       lte;
#endif /* FEATURE_LTE */
  rr_l1_utran_tdd_measurements_t utran_tdd;
} rr_l1_irat_measurements_u;


/*! L1 sends an MPH_SURROUND_IRAT_MEAS_IND to RR for each frequency on each
  supported RAT which appears in the inter-RAT neighbour lists.

  If no cells are detected on a particular frequency, L1 will still send this
  signal.  This allows RR to stop any timers for cells that might have
  previously been reselection candidates.
*/
typedef struct
{
  /*! Message Header. */
  IMH_T                      message_header;

  /*! RAT that these measurements are associated with. */
  rr_l1_irat_rat_e           rat;

  /*! Union of measurements for the specified RAT. */
  rr_l1_irat_measurements_u  measurements;
  gas_id_t                   gas_id;
} mph_surround_irat_meas_ind_t;

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

/******************************************************************************
 * MPH_UE_MODE_CHANGE_REQ
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Requests that L1 set its mode to the value specified in the gsm_only parameter
 ******************************************************************************/

typedef struct
{
   IMH_T    message_header;
   boolean  gsm_only;
   gas_id_t gas_id;
   boolean  sglte_mode_active;
} mph_ue_mode_change_req_T;

typedef struct
{
   IMH_T                                  message_header;
   word                                   wcdma_QSearch_C;
   boolean                                SEARCH_PRIO_3G;
   wcdma_cell_list_T                      wcdma_cell_list;
   utran_tdd_cell_list_T                  utran_tdd_cell_list;
#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
   utran_tdd_reselection_freq_list_T      utran_tdd_freq_list; /*Used for Pseudo FR only. 
                                                                 Only freq list OR cell list will be populated*/
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

#ifdef FEATURE_LTE
  /*! This structure include the frequencies belonging to the E-UTRAN
  neighour list. */
  uint8                                  eutran_frequency_count;
  rr_l1_eutran_frequency_t               eutran_frequencies[MAX_RR_L1_EUTRAN_FREQUENCIES];

  /*! Search for E-UTRAN cells if signal level below threshold (0-7):
    - 98, - 94, … , - 74 dBm, infinity dBm (always)
  or above threshold (8-15):
    - 78, - 74, … , - 54 dBm, infinity dBm (never)
  */
  uint8                                  qsearch_c_eutran;
#endif /* FEATURE_LTE */

  boolean                              use_priority_reselection_algo;
  uint8                                THRESH_priority_search;
  uint8                                geran_priority;
  rr_l1_irat_frequency_priority_list_t frequency_priority_list;

   gas_id_t                               gas_id;
} mph_dedicated_wcdma_cell_update_list_req_T;

typedef struct
{
  word                    UARFCN;
  word                    Scrambling_Code;
  int16                   RSSI;
  int16                   Ec_Nox2;
  int16                   RSCP;
  uint16                  PN_Offset;
} wcdma_dedicated_meas_T;

typedef struct
{
  IMH_T                            message_header;
  byte                             num_best_cells;
  wcdma_dedicated_meas_T           wcdma_ded_meas[MAX_NUMBER_OF_DED_BEST_WCDMA_CELLS];
  gas_id_t                         gas_id;
} mph_surround_wcdma_dedicated_meas_ind_T;

/******************************************************************************
 * MPH_SURROUND_WCDMA_XFER_MEAS_IND primitive
 * FROM : L1
 * TO   : RR
 * DESCRIPTION :
 * Sent by L1 to RR during transfer, containing a single WCDMA cell
 ******************************************************************************/

typedef struct
{
  IMH_T                            message_header;
  byte                             num_best_cells;
  wcdma_dedicated_meas_T           wcdma_xfer_meas[MAX_NUMBER_OF_DED_BEST_WCDMA_CELLS];
  gas_id_t                         gas_id;
} mph_surround_wcdma_xfer_meas_ind_T;

/******************************************************************************
 * MPH_START_IDLE_MODE_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * L1 has started Idle Mode
 ******************************************************************************/

typedef struct
{
   IMH_T    message_header;
   gas_id_t gas_id;
} mph_start_idle_mode_cnf_T;

/******************************************************************************
 * MPH_L1_TEST_MODE_START_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * L1 has started test mode.
 ******************************************************************************/

typedef struct
{
   IMH_T    message_header;
   gas_id_t gas_id;
} mph_l1_test_mode_start_ind_T;

/******************************************************************************
 * MPH_L1_TEST_MODE_STOP_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * L1 has stopped test mode.
 ******************************************************************************/

typedef struct
{
   IMH_T    message_header;
   gas_id_t gas_id;
} mph_l1_test_mode_stop_ind_T;

#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH

/******************************************************************************
 * MPH_INTERRAT_LIST_SEARCH_TIME_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * RR request L1 informs it of when it may be suspended.
 ******************************************************************************/

typedef enum
{
  RR_L1_LIST_SRCH_TIME_REQ_START,
  RR_L1_LIST_SRCH_TIME_REQ_STOP
} rr_l1_list_srch_time_req_command_T;

typedef struct
{
  IMH_T                              message_header;
  rr_l1_list_srch_time_req_command_T command;
  gas_id_t                           gas_id;
  /* Indicates the RAT that RR is searching. */
  rr_l1_irat_rat_e                   rat;
} mph_interrat_list_srch_time_req_T;

/******************************************************************************
 * MPH_INTERRAT_LIST_SEARCH_TIME_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * L1 indicates to RR that it may be suspended.
 ******************************************************************************/

typedef struct
{
  IMH_T    message_header;
  uint32   timer_value;
  gas_id_t gas_id;
} mph_interrat_list_srch_time_ind_T;


/******************************************************************************
 * MPH_SUSPEND_GSM_MOD_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * RR requests L1 to suspend
 ******************************************************************************/
typedef enum
{
  RR_L1_SUSPEND_REASON_NONE
} rr_l1_suspend_reason_T;

typedef struct
{
  IMH_T                  message_header;
  rr_l1_suspend_reason_T suspend_reason;
  gas_id_t               gas_id;
} mph_suspend_gsm_mode_req_T;


/******************************************************************************
 * MPH_SUSPEND_GSM_MOD_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * L1 confirms to RR that it has suspended
 ******************************************************************************/

typedef struct
{
  IMH_T    message_header;
  boolean  suspended;
  uint32   timer_value;
  gas_id_t gas_id;
} mph_suspend_gsm_mode_cnf_T;


/******************************************************************************
 * MPH_RESUME_GSM_MOD_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * RR requests L1 to resume
 ******************************************************************************/

typedef enum
{
  RR_L1_RESUME_REASON_NONE
} rr_l1_resume_reason_T;

typedef struct
{
  IMH_T                 message_header;
  rr_l1_resume_reason_T resume_reason;
   gas_id_t             gas_id;
} mph_resume_gsm_mode_req_T;


/******************************************************************************
 * MPH_RESUME_GSM_MOD_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * L1 confirms to RR that it has resumeed
 ******************************************************************************/

typedef struct
{
  IMH_T    message_header;
  gas_id_t gas_id;
} mph_resume_gsm_mode_cnf_T;

#endif /* FEATURE_GTOW_BACKGROUND_PLMN_SEARCH */

/******************************************************************************
 * MPH_RESET_REQ primitive
 * FROM : RR
 * TO   : L1
 * DESCRIPTION :
 * A fatal error has been detected, and L1 needs to be reset
 ******************************************************************************/

typedef struct
{
   IMH_T    message_header;
   gas_id_t gas_id;
} mph_reset_req_T;

/******************************************************************************
 * MPH_RESET_CNF primitive
 * FROM : L1
 * TO   : RR
 * DESCRIPTION :
 * A fatal error has been detected, and L1 needs to be reset
 ******************************************************************************/

typedef struct
{
   IMH_T    message_header;
   gas_id_t gas_id;
} mph_reset_cnf_T;

/******************************************************************************
 * MPH_RESET_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * A fatal error has been detected by L1, and it has reset
 ******************************************************************************/

typedef struct
{
   IMH_T    message_header;
   gas_id_t gas_id;
} mph_reset_ind_T;


#ifdef FEATURE_GSM_DTM
/******************************************************************************
 * MPH_DTM_CHANNEL_RELEASE_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * GRR is requesting L1 to terminate a CS DTM call
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   boolean           ps_release;
   boolean           valid_bcch_carrier;
   ARFCN_T           serving_bcch_carrier;
   BSIC_T            BSIC;
   gas_id_t          gas_id;
} mph_dtm_channel_release_req_t;

/******************************************************************************
 * MPH_DTM_CHANNEL_RELEASE_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Provides an indication to RR that the above dtm assignment has been processed
 ******************************************************************************/

typedef struct
{
   IMH_T    message_header;
   boolean  ps_release;
   boolean  cs_release;
   boolean  error_code;
   gas_id_t gas_id;
} mph_dtm_channel_release_ind_t;


/******************************************************************************
 * MPH_DTM_CHANNEL_ASSIGN_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Provides an indication to RR that the DTM assignment that has previously been
 * sent to MAC in a MAC_L1_DTM_ASSIGN_REQ is now connected
 ******************************************************************************/

typedef struct
{
   IMH_T    message_header;
   gas_id_t gas_id;
} mph_dtm_channel_assign_cnf_t;

#ifdef FEATURE_GSM_EDTM
/******************************************************************************
 * MPH_DTM_L1PARAM_UPDATE_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Sends any L1 specific system information that has changed during DTM.
 ******************************************************************************/
typedef struct
{
  IMH_T                message_header;
  channel_t            ccch_or_pccch;
  ccch_pccch_params_t  l1_system_info_params; /* similar to GPRS_L1PARAM_UPDATE   */
  gas_id_t             gas_id;
} mph_dtm_l1param_update_req_t;

#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

typedef struct
{
  IMH_T                         message_header;
  uint32                        timer_val;  /* the DRX interval available before next WCDMA paging block */
  gas_id_t                      gas_id;
  sys_modem_device_mode_e_type  multi_sim_mode;
  int                           num_sims_used;
} mph_start_gsm_mode_for_bplmn_req_T;

typedef struct
{
  IMH_T    message_header;
  uint32   timer_val;  /* the DRX interval available before next WCDMA paging block */
  gas_id_t gas_id;
}mph_resume_gsm_mode_for_bplmn_req_T;

typedef struct
{
  IMH_T     message_header;
  gas_id_t  gas_id;
}mph_rat_change_ind_T;

/**
  @brief Sent from RR to L1 to indicate that sleep should be enabled.
*/
typedef struct
{
  IMH_T    message_header;
  gas_id_t gas_id;
} mph_enable_sleep_ind_T;

typedef struct
{
  IMH_T    message_header;
  boolean  reduced_ncell_list;
  gas_id_t gas_id;
}mph_reduced_ncell_meas_ind_T;

typedef struct
{
  IMH_T             message_header;
  gas_id_t          gas_id;
}mph_suspend_gsm_mode_ind_T;

typedef struct
{
  IMH_T    message_header;
  gas_id_t gas_id;
}mph_start_gsm_mode_for_bplmn_cnf_T;

typedef struct
{
  IMH_T    message_header;
  gas_id_t gas_id;
}mph_resume_gsm_mode_for_bplmn_cnf_T;

typedef enum
{
  RR_DS_ABORT_TO_NULL = 0, /* abort and transition to a null state */
  RR_DS_ABORT_TO_IDLE,     /* abort and resume idle paging decodes */
  RR_DS_ABORT_TRANSFER,    /* packet transfer has been pre-empted  */
  RR_DS_ABORT_TIMEDOUT,    /* an activity timed out pending on TRM */
  RR_DS_ABORT_G_PLUS_G     /* G+G mode should be deactivated by L1 */
} rr_ds_abort_type_T;

typedef struct
{
  IMH_T                   message_header;
  rr_ds_abort_type_T      abort_type;
  uint32                  timeout_ms;
  rr_l1_message_id_enum_T pending_msg;
  boolean                 complete_granted;
  gas_id_t                gas_id;
} mph_l1_ds_abort_ind_T;

/******************************************************************************
 * MPH_DSC_THRESHOLD_IND primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * Informs GL1 of the DSC as a percentage of MAX for the current serving cell
 ******************************************************************************/
#define NUM_DSC_THRESHOLDS         6
#define GERAN_DSC_THRESHOLD_0   (100)
/* these percentage thresholds are defined by GERAN Systems */
/* and should be listed in descending order, ending at zero */
#define GERAN_DSC_THRESHOLD_1    (85)
#define GERAN_DSC_THRESHOLD_2    (70)
#define GERAN_DSC_THRESHOLD_3    (60)
#define GERAN_DSC_THRESHOLD_4    (50)
#define GERAN_DSC_THRESHOLD_5    (40)
#define GERAN_DSC_THRESHOLD_6     (0)

typedef struct
{
  IMH_T                   message_header;
  uint8                   percent_of_max;
  byte                    RXLEV_average;
  l2_channel_type_T       l2_channel_type;
  rr_l1_epd_preference_t  epd_pref;
  gas_id_t                gas_id;
} mph_dsc_threshold_ind_T;

/******************************************************************************
 * MPH_NV_REFRESH_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * RR requests L1 to refresh carrier specific NV items
 ******************************************************************************/
 
typedef struct
{
  IMH_T    message_header;
  gas_id_t gas_id;
} mph_nv_refresh_req_T;

/******************************************************************************
 * MPH_NV_REFRESH_CNF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * L1 confirms to RR that it has refreshed NV items
 ******************************************************************************/
 
typedef struct
{
  IMH_T    message_header;
  gas_id_t gas_id;
} mph_nv_refresh_cnf_T;


typedef struct
{
  IMH_T               message_header;
  uint32              arfcn;
  rr_l1_irat_rat_e    rat;
  uint32              resel_timer; /*resel_timer in seconds*/
  gas_id_t            gas_id;
} mph_specific_irat_freq_update_req_T;

/******************************************************************************
 * MPH_DEEP_SLEEP_IND primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * RR requests L1 to go to deep sleep mode when already INACTIVE
 ******************************************************************************/
 
typedef struct
{
  IMH_T    message_header;
  gas_id_t gas_id;
} mph_deep_sleep_ind_T;

/******************************************************************************
 * MPH_UPDATE_IDLE_DRX_IND primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * RR indicates L1 to use parameters for scaling Idle DRX / skipping Idle Meas
 ******************************************************************************/
 
typedef struct
{
  /*! @brief Message header. */
  IMH_T              message_header;

  /*! @brief original BS_PA_MFRMS */  
  byte               BS_PA_MFRMS;

  /*! @brief To be set to TRUE when extended DRX cycle is configured */
  boolean            extended_BS_PA_MFRMS_valid;

  /*! @brief extended DRX cycle */  
  byte               extended_BS_PA_MFRMS;
  
  /*! @brief To be set to TRUE when idle mode meas have to be skipped */
  boolean            skip_idle_mode_meas;

  /*! @brief GERAN Access Stratum ID */
  gas_id_t           gas_id;
} mph_update_idle_drx_ind_T;

/**
  @brief Union of all messages sent from RR to L1.
*/
typedef union
{
  IMH_T                                      message_header;
  mph_change_page_mode_req_T                 mph_change_page_mode_req;
  mph_channel_assign_req_T                   mph_channel_assign_req;
  mph_channel_mode_modify_req_T              mph_channel_mode_modify_req;
  mph_channel_release_req_T                  mph_channel_release_req;
  mph_dedicated_update_req_T                 mph_dedicated_update_req;
  mph_frequency_redefinition_req_T           mph_frequency_redefinition_req;
  mph_handover_req_T                         mph_handover_req;
  mph_idle_update_req_T                      mph_idle_update_req;
  mph_immed_assign_req_T                     mph_immed_assign_req;
  mph_read_sys_info_req_T                    mph_read_sys_info_req;
  mph_reconnect_channel_req_T                mph_reconnect_channel_req;
  mph_select_specific_bcch_req_T             mph_select_specific_bcch_req;
  mph_set_ciphering_mode_req_T               mph_set_ciphering_mode_req;
  mph_start_idle_req_T                       mph_start_idle_req;
  mph_stop_handover_access_req_T             mph_stop_handover_access_req;
  mph_abort_ra_req_T                         mph_abort_ra_req;
  mph_early_camping_req_T                    mph_early_camping_req;
  mph_decode_bcch_list_req_T                 mph_decode_bcch_list_req;
  mph_decode_bcch_list_abort_req_T           mph_decode_bcch_list_abort_req;
  mph_power_scan_req_T                       mph_power_scan_req;
  mph_power_scan_abort_req_T                 mph_power_scan_abort_req;
  mph_start_gsm_mode_req_T                   mph_start_gsm_mode_req;
  mph_stop_gsm_mode_req_T                    mph_stop_gsm_mode_req;
  mph_ue_mode_change_req_T                   mph_ue_mode_change_req;
  mph_wcdma_cell_update_list_req_T           mph_wcdma_cell_update_list_req;
  mph_ignore_wcdma_neighbors_req_T           mph_ignore_wcdma_neighbors_req;
  mph_dedicated_wcdma_cell_update_list_req_T mph_dedicated_wcdma_cell_update_list_req;
  mph_start_gprs_idle_mode_req_t             mph_start_gprs_idle_mode_req;
  mph_gprs_l1param_update_req_t              mph_gprs_l1param_update_req;
  mph_nc_measurements_req_t                  mph_nc_measurements_req;
  mph_stop_nc_measurements_req_t             mph_stop_nc_measurements_req;
  mph_gprs_surround_update_req_t             mph_gprs_surround_update_req;
  mph_gprs_reselection_req_t                 mph_gprs_reselection_req;
  mph_start_mm_non_drx_ind_t                 mph_start_mm_non_drx_ind;
  mph_stop_mm_non_drx_ind_t                  mph_stop_mm_non_drx_ind;
#ifndef FEATURE_WCDMA_PLT
#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
  mph_interrat_list_srch_time_req_T          mph_interrat_list_srch_time_req;
  mph_suspend_gsm_mode_req_T                 mph_suspend_gsm_mode_req;
  mph_resume_gsm_mode_req_T                  mph_resume_gsm_mode_req;
#endif /* FEATURE_GTOW_BACKGROUND_PLMN_SEARCH */
#endif
  mph_start_gsm_mode_for_bplmn_req_T         mph_start_gsm_mode_for_bplmn_req;
  mph_resume_gsm_mode_for_bplmn_req_T        mph_resume_gsm_mode_for_bplmn_req;
  mph_rat_change_ind_T                       mph_rat_change_ind;
  mph_enable_sleep_ind_T                     mph_enable_sleep_ind;
  mph_reduced_ncell_meas_ind_T               mph_reduced_ncell_meas_ind;
  mph_reset_req_T                            mph_reset_req;
#ifdef FEATURE_GSM_DTM
  mph_dtm_channel_release_req_t              mph_dtm_channel_release_req;
#ifdef FEATURE_GSM_EDTM
  mph_dtm_l1param_update_req_t               mph_dtm_l1param_update_req;
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
  mph_l1_ds_abort_ind_T                      mph_l1_ds_abort_ind;
  mph_nv_refresh_req_T                       mph_nv_refresh_req;
  mph_specific_irat_freq_update_req_T        mph_specific_irat_freq_update_req;
  mph_deep_sleep_ind_T                       mph_deep_sleep_ind;
  mph_update_idle_drx_ind_T                  mph_update_idle_drx_ind;
} rr_l1_messages_u;

/******************************************************************************
 * MPH_UE_MODE_CHANGE_CNF
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Confirmation that L1 has switched to or from GSM-only mode successfully.
 ******************************************************************************/

typedef struct
{
   IMH_T    message_header;
   gas_id_t gas_id;
} mph_ue_mode_change_cnf_T;

/**
  @brief Union of all messages sent from L1 to RR.
*/
typedef union
{
  IMH_T                                   message_header;
  mph_block_quality_ind_T                 block_quality_ind;
  mph_channel_assign_cnf_T                channel_assign_cnf;
  mph_channel_mode_modify_cnf_T           channel_mode_modify_cnf;
  mph_channel_release_cnf_T               channel_release_cnf;
  mph_frequency_redefinition_cnf_T        frequency_redefinition_cnf;
  mph_handover_cnf_T                      handover_cnf;
  mph_handover_ind_T                      handover_ind;
  mph_immed_assign_cnf_T                  immed_assign_cnf;
  mph_reconnect_channel_cnf_T             reconnect_channel_cnf;
  mph_select_specific_bcch_cnf_T          select_specific_bcch_cnf;
  mph_serving_ded_meas_ind_T              serving_ded_meas_ind;
  mph_serving_idle_meas_ind_T             serving_idle_meas_ind;
  mph_set_ciphering_cnf_T                 set_ciphering_cnf;
  mph_stop_handover_access_cnf_T          stop_handover_access_cnf;
  mph_surround_meas_ind_T                 surround_meas_ind;
  mph_reselection_cnf_T                   reselection_cnf;
  mph_decode_bcch_list_cnf_T              decode_bcch_list_cnf;
  mph_decode_bcch_list_failure_ind_T      decode_bcch_list_failure_ind;
  mph_power_scan_cnf_T                    power_scan_cnf;
  mph_surround_update_failure_ind_T       surround_update_failure_ind;
  mph_specific_bcch_decode_failure_ind_T  specific_bcch_decode_failure_ind;
  mph_surround_wcdma_idle_meas_ind_T      surround_wcdma_idle_meas_ind;
  mph_serving_aux_meas_ind_T              serving_aux_meas_ind;
  mph_read_sys_info_failure_ind_T         read_sys_info_failure_ind;
  mph_fcch_sch_decode_ind_T               fcch_sch_decode_ind;
  mph_frequency_redefinition_ind_T        frequency_redefinition_ind;
  mph_start_gsm_mode_cnf_T                start_gsm_mode_cnf;
  mph_stop_gsm_mode_cnf_T                 stop_gsm_mode_cnf;
  mph_nc_measurement_ind_t                nc_measurement_ind;
  mph_serving_meas_ind_t                  serving_meas_ind;
  mph_stop_surround_update_req_t          stop_surround_update_req;
  mph_surround_wcdma_dedicated_meas_ind_T surround_wcdma_dedicated_meas_ind;
  mph_surround_wcdma_xfer_meas_ind_T      surround_wcdma_xfer_meas_ind;
  mph_start_idle_mode_cnf_T               start_idle_mode_cnf;
#ifndef FEATURE_WCDMA_PLT
#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
  mph_interrat_list_srch_time_ind_T       interrat_list_srch_time_ind;
  mph_suspend_gsm_mode_cnf_T              suspend_gsm_mode_cnf;
  mph_resume_gsm_mode_cnf_T               resume_gsm_mode_cnf;
#endif /* FEATURE_GTOW_BACKGROUND_PLMN_SEARCH */
#endif
  mph_suspend_gsm_mode_ind_T              suspend_gsm_mode_ind;
  mph_start_gsm_mode_for_bplmn_cnf_T      start_gsm_mode_for_bplmn_cnf;
  mph_resume_gsm_mode_for_bplmn_cnf_T     resume_gsm_mode_for_bplmn_cnf;
#ifdef FEATURE_GSM_DTM
  mph_dtm_channel_release_ind_t           dtm_channel_release_ind;
  mph_dtm_channel_assign_cnf_t            dtm_channel_assign_cnf;
#endif /* FEATURE_GSM_DTM */
  mph_l1_test_mode_start_ind_T            l1_test_mode_start_ind;
  mph_l1_test_mode_stop_ind_T             l1_test_mode_stop_ind;
  mph_reset_ind_T                         reset_ind;
  mph_reset_cnf_T                         reset_cnf;
#if defined (FEATURE_WCDMA) || defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  mph_surround_irat_meas_ind_t            surround_irat_meas_ind;
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
  mph_l1_ds_abort_ind_T                   l1_ds_abort_ind;
  mph_ue_mode_change_cnf_T                ue_mode_change_cnf;
  mph_coex_msc_change_ind_T               coex_msc_change_ind;
  mph_nv_refresh_cnf_T                    nv_refresh_cnf;
} l1_rr_messages_u;


#endif /* rr_l1_g_h */

/* EOF */


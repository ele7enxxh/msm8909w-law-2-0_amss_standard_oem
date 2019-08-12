/*===========================================================================

                             CSN.1 ENCODER DECODER

                            P U B L I C   T Y P E S

DESCRIPTION
  The following declarations are for use with the CSN.1 encoder/decoder.

Copyright (c) 2007-2014 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/csn_apt.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
07-06-15   sjw     Mainlined the refactored CSN files. Feature def is not required.
07-05-29   tjw     Some typedefs reverted to #defines as the effects rippled widely.
06-05-19   tjw     Format tidied.
06-05-17   tjw     Created from //depot/asic/msmshared/geran/core/gcommon/csn.h#16.

===========================================================================*/

#ifndef CSN_APT_H
#define CSN_APT_H

/* Include files */

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


  #include "sys_type.h"
  #define CSN_ARFCN_TYPE ARFCN_T

#ifdef FEATURE_GPRS_PS_HANDOVER
  #include "sys_stru.h"
#endif /* FEATURE_GPRS_PS_HANDOVER */

/*===========================================================================

                        Constant Declarations

===========================================================================*/

  #define  PACKET_SI_DATA                                 0x00 /* internal use only */
  #define  PACKET_ACCESS_REJECT                           0x21
  #define  PACKET_CELL_CHANGE_ORDER                       0x01
  #define  PACKET_DOWNLINK_ASSIGNMENT                     0x02
  #define  PACKET_MEASUREMENT_ORDER                       0x03
  #define  PACKET_PAGING_REQUEST                          0x22
  #define  PACKET_PDCH_RELEASE                            0x23
  #define  PACKET_POLLING_REQUEST                         0x04
  #define  PACKET_PWR_CTRL_TMNG_ADV                       0x05
  #define  PACKET_PRACH_PARAMETERS                        0x24
  #define  PACKET_QUEUEING_NOTIFIACTION                   0x06
  #define  PACKET_TIMESLOT_RECONFIGURE                    0x07
  #define  PACKET_TBF_RELEASE                             0x08
  #define  PACKET_UPLINK_ACK_NACK                         0x09
  #define  PACKET_UPLINK_ASSIGNMENT                       0x0a
  #define  PACKET_DOWNLINK_DUMMY_CNTRL_BLK                0x25
  #define  PACKET_PSI6                                    0x30
  #define  PACKET_PSI1                                    0x31
  #define  PACKET_PSI2                                    0x32
  #define  PACKET_PSI3                                    0x33
  #define  PACKET_PSI3_BIS                                0x34
  #define  PACKET_PSI4                                    0x35
  #define  PACKET_PSI5                                    0x36
  #define  PACKET_PSI13                                   0x37
  #define  PACKET_PSI7                                    0x38
  #define  PACKET_PSI8                                    0x39
  #define  PACKET_PSI14                                   0x3a
  #define  PACKET_PSI3_QUATER                             0x3d
  #define  PACKET_PSI15                                   0x3e
  #define  PACKET_UNUSED_ID                               0x3f
  #define  PACKET_CELL_CHANGE_CONTINUE                    0x0b /* 0 01011  */
  #define  PACKET_NEIGHBOUR_CELL_DATA                     0x0c /* 0 01100  */
  #define  PACKET_SERVING_CELL_DATA                       0x0d /* 0 01101  */
  #define  PACKET_SERVING_CELL_SI                         0x20
  #define  PACKET_CS_COMMAND                              0x27
  #define  PACKET_CS_RELEASE                              0x29
  #define  PACKET_PS_HANDOVER                             0x15
  #define  PACKET_PHYSICAL_INFORMATION                    0x16
  #define  PACKET_INVALID_MSG                             0x70  /* beyond 6-bit range of MAC message types  */
  #define  SEGMENTED_MSG                                  0xc6  /* for MAC's exclusive use  */


  /* Encoder Constants */
  #define  PACKET_CELL_CHANGE_FAILURE                     0x00
  #define  PACKET_CONTROL_ACKNOWLEDGE                     0x01
  #define  PACKET_DOWNLINK_ACK_NACK                       0x02
  #define  PACKET_UPLK_DUMMY_CNTRL_BLK                    0x03
  #define  PACKET_MEASUREMENT_REPORT                      0x04
  #define  PACKET_RESOURCE_REQUEST                        0x05
  #define  PACKET_MOBILE_TBF_STATUS                       0x06
  /* EGPRS additions*/
  #define  EGPRS_PACKET_DOWNLINK_ACK_NACK                 0x08
  #define  PACKET_PAUSE                                   0x09
  #define  PACKET_ENHANCED_MEASUREMENT_REPORT             0x0a
  #define  ADDITIONAL_MS_RAC                              0x0b
  #define  PACKET_CELL_CHANGE_NOTIFICATION                0x0c
  #define  PACKET_SI_STATUS                               0x0d
  #define  PACKET_CS_REQUEST                              0x0e
  #define  PACKET_CHANNEL_REQUEST                         0x20 /* internal use only */
  #define  PACKET_MEASUREMENT_RPT_PREPROCESS              0x24 /* internal use only */

#define GMAC_PACKET_CHANNEL_REQUEST                0x0E /* use this for logging */

/* RR decoder message identifiers */
typedef enum
{
  SI3_REST_OCTETS,
  SI4_REST_OCTETS,
  SI13_REST_OCTETS,
  P1_REST_OCTETS,
  P2_REST_OCTETS,
  P3_REST_OCTETS,
  PACKET_CHANNEL_DESCRIPTION,
  IA_REST_OCTETS,
  SI9_REST_OCTETS,
  SI7_REST_OCTETS,
  SI8_REST_OCTETS,
  SI1_REST_OCTETS,
  IAR_REST_OCTETS,
  SI6_REST_OCTETS,
  RR_PACKET_UPLINK_ASSIGNMENT,
  RR_PACKET_DOWNLINK_ASSIGNMENT,
  GPRS_BROADCAST_INFORMATION,
  DTM_INFORMATION_DETAILS
} csn_rr_decoder_msg_id_t;

/* RR encoder message identifiers */
typedef enum
{
  MOBILE_STATION_CLASSMARK_3,
  CHANNEL_REQ_DESC_2
} csn_rr_encoder_msg_id_t;

/*  Common Constants */
/* ERROR CODES */

typedef enum
{
  CSN_DECODE_NO_ERROR                            = 0x00,  /* "no error" return */
  UNKNOWN_MESSAGE_TYPE                           = 0xc0,
  DISTRIBUTION_ERROR                             = 0xc1,
  ADDRESS_INFO_ERROR                             = 0xc2,
  NON_DISTRIBUTION_ERROR                         = 0xc3,
  MESSAGE_ESCAPE_ERROR                           = 0xc4,
  IGNORE_ERROR                                   = 0xc5,
  RR_TRUNCATED_MSG                               = 0xc7,
  NO_MEMORY                                      = 0xff
} csn_decoder_error_t;

#define RESERVED_NCH_POSITION                    0xff

#define CCN_SUPPORT_BITMAP_LENGTH                16


/*With extended segmentation MAX OTA msg can be can comprise of 9 segments */
/*each containing 23 octets after stripping out control headers the actual */
/*total number of octets that can carry CSN payload is 189 octets          */
/*MAX L3 msg rcvd during CS dedicated, ie DTM command is 251               */
/*For IRAT PSHO the max size of the encapsulated GSM msg is 256            */
#define MAX_CSN_OTA_MSG_IN_OCTETS                256

/*===========================================================================

  CSN Field length constants

===========================================================================*/

#define CSN_CONTAINER_DATA_LEN                   24
#define CSN_RFL_DATA_LEN_MIN_VAL_OFFSET          3
#define FSL_LENGTH_BYTES                         8
#define CSN_PKT_CS_COMMAND_MAX_OCTETS            168

/*===========================================================================

  DATA DECLARATIONS - Decoder

===========================================================================*/

typedef enum {
  usf_only                                       = 0,
  usf_and_power_control                          = 1
} timeslot_alloc_flag_e;

typedef enum {
  dynamic                                        = 0,
  fixed                                          = 1
} alloc_flag_e;

/* struct packet_access_reject */
typedef struct
{
  uint16                                         random_access_info;
  uint16                                         frame_number;
} pkt_req_ref_t;

typedef enum
{
  uplink_valid,
  dwnlink_valid
} direction_valid_e;

typedef struct
{
  uint8                                          uplink_tfi;
  uint8                                          dwnlink_tfi;
  direction_valid_e                              direction;
} global_tfi_t;

typedef enum
{
  wait_ind_not_valid,
  wait_ind_valid
} wait_ind_valid_e;

typedef enum
{
  tlli_valid                                     = 0x01,
  req_ref_valid,
  global_tfi_valid,
  tqi_valid
} address_field_e;

typedef struct
{
  uint32                                         tlli;
  pkt_req_ref_t                                  pk_req_ref;
  global_tfi_t                                   global_tfi;
  uint8                                          wait_indication;
  uint8                                          wait_indication_size;
  wait_ind_valid_e                               wait_valid;
  address_field_e                                tlli_gtfi_pkt_ref_valid;
} reject_struct_t;

typedef struct
{
#define MAX_NO_REJ_STRUCTS                       3
  reject_struct_t                                *rej_ptr;
  reject_struct_t                                reject_struct0;
  reject_struct_t                                reject_struct1;
  reject_struct_t                                reject_struct2;
  reject_struct_t                                reject_struct3;
  uint8                                          no_addition_rej_structs;
} packet_access_reject_t;

typedef enum
{
  nc_period_not_valid,
  nc_period_valid
} nc_period_flag_e;

typedef struct
{
  uint8                                          priority_class;
  uint8                                          hcs_thr;
} hcs_params_t;

typedef enum
{
  si13_loc_valid,
  pbcch_loc_valid
} si13_loc_flag_e;

typedef struct
{
  uint8                                          si13_location;
  uint8                                          pbcch_location;
  uint8                                          psi1_repeat_period;
  si13_loc_flag_e                                si13_loc_flag;
} si13_pbcch_loc_t;

typedef enum
{
  csn_use_bcch_arfcn,
  csn_arfcn_valid,
  csn_maio_valid
} csn_bcch_arfcn_maio_t;

typedef struct
{
#define GPRS_RXLEV_VALID                         0x01
#define GPRS_TEMP_OFFSET_VALID                   0x02
#define GPRS_RESELECT_OFFEST_VALID               0x04
#define HCS_VALID                                0x08
#define SI13_PBCCH_VALID                         0x10
  uint8                                          cell_bar_access2;
  uint8                                          exc_acc;
  uint8                                          same_ra_as_serv_cell;
  uint8                                          gprs_rxlev_access_min;
  uint8                                          gprs_ms_txpwr_max_cch;
  uint8                                          gprs_temp_offset;
  uint8                                          gprs_penalty_time;
  uint8                                          gprs_reselect_offset;
  hcs_params_t                                   hcs_params;
  si13_pbcch_loc_t                               si13_pbcch_loc;
  uint8                                          option_bitmap;
} cell_sel_params_t;

typedef struct
{
  uint8                                          frequency_diff;
  uint8                                          bsic;
  cell_sel_params_t                              cel_sel_params;
} freq_diff_t;

typedef enum
{
  CSN_PCCO_GSM_TARGET_CELL,
  CSN_PCCO_3G_TARGET_CELL
} pcco_cell_type_e;

typedef enum
{
  no_timing_params_valid,
  timing_value_valid,
  timing_indx_ts_valid,
  all_timing_params_valid
} timing_adv_options_e;

typedef struct
{
  uint8                                          timing_adv_value;
  uint8                                          timing_adv_index;
  uint8                                          timing_adv_timeslot;
  timing_adv_options_e                           valid_options;
} pkt_timing_adv_t;

typedef struct
{
  uint8                                          po;
  uint8                                          bts_pwr_ctrl_mode;
  uint8                                          pr_mode;
} dwnlk_pwr_ctrl_t;

typedef enum
{
  pwr_ctrl_not_valid,
  pwr_ctrl_valid
} dwnlk_pwr_ctrl_flag_e;

#define MAX_NO_RFL_ELEMENTS                      16
#define MAX_NO_ARFCN_ELEMENTS                    16


/* newer compilers use a packed format */
typedef PACKED struct PACKED_POST
{
  uint8                                          hsn;
  uint8                                          rfl_number_list[MAX_NO_RFL_ELEMENTS];
  uint8                                          size_of_rfl_list;
  uint8                                          ma_length;
  uint64                                         ma_bitmap;
  uint8                                          arfcn_index_list[MAX_NO_ARFCN_ELEMENTS];
  uint8                                          size_of_arfcn_list;
  enum
  {
    ma_list_valid,
    arfcn_list_valid
  } ma_arfcn_flag;
} gprs_mobile_alloc_t;


typedef struct
{
  uint8                                          maio;
  uint8                                          hsn;
  uint8                                          len_ma_freq_list;
  uint8                                          ma_freq_list[19];
} direct_enc2_t;

typedef struct
{
  uint8                                          maio;
  gprs_mobile_alloc_t                            gprs_mobile_alloc;
} direct_enc1_t;

typedef enum
{
  no_change_mark_valid,
  change_mark_1_valid,
  change_mark_2_valid,
  both_change_marks_valid
} change_mark_flag_e;

typedef struct
{
  uint8                                          maio;
  uint8                                          ma_number;
  uint8                                          change_mark_1;
  uint8                                          change_mark_2;
  change_mark_flag_e                             change_mark_flag;
} indirect_enc_t;

typedef enum
{
  arfcn_valid,
  indirect_valid,
  direct1_valid,
  direct2_valid
} encoding_type_e;

typedef struct
{
#define FP_ARFCN                                 0x0
#define FP_IND_ENC                               0x1
#define FP_DIR_ENC1                              0x2
#define FP_DIR_ENC2                              0x3
  uint8                                          tsc;
  CSN_ARFCN_TYPE                                 arfcn;
  indirect_enc_t                                 indir_enc;
  direct_enc1_t                                  dir_enc1;
  direct_enc2_t                                  dir_enc2;
  encoding_type_e                                encoding_type;
} freq_params_t;

typedef struct
{
  uint8                                          alpha;
  uint8                                          gamma_tn0;
  uint8                                          gamma_tn1;
  uint8                                          gamma_tn2;
  uint8                                          gamma_tn3;
  uint8                                          gamma_tn4;
  uint8                                          gamma_tn5;
  uint8                                          gamma_tn6;
  uint8                                          gamma_tn7;
  uint8                                          gamma_bitmap;
} pwr_ctrl_paramtrs_t;

typedef struct
{
  uint16                                         absolute;
  uint16                                         relative;
  enum
  {
    absolute_valid,
    relative_valid
  } abs_rel_flag;
} start_frame_number_t;

typedef struct
{
  start_frame_number_t                           meas_start_time;
  uint8                                          measure_interval;
  uint8                                          measure_bitmap;
} measure_mapping_t;

/* COMPACT reduced MA IE */

/* newer compilers use a packed format */
typedef PACKED struct PACKED_POST
{
  uint8                                          reduced_ma_bit_length;
  uint64                                         reduced_ma_bit_1_to_64;
  uint64                                         reduced_ma_bits_65_to_127;
  boolean                                        maio_2_flag;
  uint8                                          maio_2;
} compact_reduced_ma_t;


/* Multi block alloc struct */
typedef struct
{
  uint8                                          timeslot_num;
  boolean                                        alpha_gamma_flag;
  uint8                                          alpha;
  uint8                                          gamma_tn;
  boolean                                        po_prmode_flag;
  uint8                                          po;
  uint8                                          bts_pwr_ctrl_mode;
  uint8                                          pr_mode;
  start_frame_number_t                           tbf_start_frm_num;
  uint8                                          number_of_blks_alloc;
} multi_blk_alloc_t;

/* EGPRS Ack/Nack Description */
typedef struct
{
  boolean                                        len_present;   /*1 bit  */
  uint8                                          length;        /*8 bits */
  uint8                                          fai;           /*1 bit  */
  uint8                                          bow;           /*1 bit  */
  uint8                                          eow;           /*1 bit  */
  uint16                                         ssn;           /*1 bit  */
  boolean                                        comp_present;  /*1 bit  */
  uint8                                          crbb_len;      /*7 bits */
  uint8                                          scc;           /*1 bit  */
  uint32                                         crbb[4];
  uint32                                         urbb[8];

  /*urbb_len not used to build the PDAN ack/nack des  */
  /*only relevant to the UL and the PUAN ack/nack des */
  uint8                                          urbb_len;
  } egprs_ack_nack_des_t;

typedef struct
{
#define MAX_PERSIST_VALUES                       4
  uint8                                          persistence[MAX_PERSIST_VALUES];
  boolean                                        persistence_flag;
  uint32                                         tlli;
  global_tfi_t                                   global_tfi;
  address_field_e                                tlli_gtfi_pkt_ref_valid;
  uint8                                          mac_mode;
  uint8                                          rlc_mode;
  uint8                                          ctrl_ack;
  uint8                                          timeslot_alloc;
  pkt_timing_adv_t                               pkt_timing_adv;
  dwnlk_pwr_ctrl_t                               dwnlk_pwr_ctrl;
  dwnlk_pwr_ctrl_flag_e                          dwnlk_pwr_ctrl_flag;
  freq_params_t                                  freq_params;
  boolean                                        freq_param_flag;
  uint8                                          downlink_tfi;
  boolean                                        dwnlk_tfi_flag;
  pwr_ctrl_paramtrs_t                            pwr_ctrl_paramtrs;
  boolean                                        pwr_ctrl_flag;
  start_frame_number_t                           tbf_start_frm_num;
  boolean                                        tbf_start_flag;
  measure_mapping_t                              measure_mapping;
  boolean                                        meas_map_flag;
  boolean                                        r99_additions_flag;
  boolean                                        pkt_ext_ta_flag;
  uint8                                          pkt_ext_ta;
  boolean                                        egprs_options_flag;
  uint8                                          egprs_win_sz;
  uint8                                          egprs_link_qual;
  boolean                                        egprs_bep_prd2_flag;
  uint8                                          egprs_bep_prd2;
  boolean                                        compact_reduced_ma_flag;
  compact_reduced_ma_t                           compact_reduced_ma;
} packet_downlink_assignment_t;

typedef struct
{
  uint16                                         start_freq;
  uint8                                          nr_of_freq;
  uint8                                          freq_diff_length;
  uint8                                          freq_diff[32];
} frequency_list_t;

typedef struct
{
#define MAX_NO_EXT_FREQ_ELEMENTS                 16
  uint8                                          ext_reporting_type;
  boolean                                        ext_rep_type_flag;
  uint8                                          ncc_permitted;
  uint8                                          int_frequency;
  boolean                                        int_frequency_flag;
  uint8                                          ext_reporting_period;
  boolean                                        ext_rep_prd_flag;
  frequency_list_t                               *ext_freq_list[MAX_NO_EXT_FREQ_ELEMENTS];
  uint8                                          number_frequency_list;
} em1_t;

typedef struct
{
  uint8                                          ext_meas_ordr;
  em1_t                                          em1;
} ext_meas_params_t;

#define EXT_MEAS_ORDR0                           0
#define EXT_MEAS_ORDR1                           1
#define EXT_MEAS_ORDR2                           2
#define EXT_MEAS_ORDR3                           3

#define EXT_RPRT_TYPE0                           0
#define EXT_RPRT_TYPE1                           1
#define EXT_RPRT_TYPE2                           2
#define EXT_RPRT_TYPE3                           3

typedef enum
{
  ba_present,
  psi3_change_mark_present
} ba_or_psi3_change_mark_e;

typedef enum {
  fdd_arfcn_valid,
  fdd_arfcn_index_valid
} fdd_arfcn_or_arfcn_index_valid_e;

/*
fdd_cell_information is a variable length bit field. For simplicity of
decoding, each bit is unpacked into a uint8.
*/
typedef struct
{
#define MAX_NR_OF_FDD_CELLS                      31

  uint16                                         fdd_arfcn;
  uint8                                          fdd_indic0;
  uint8                                          nr_of_fdd_cells;
  uint16                                         fdd_cell_information[MAX_NR_OF_FDD_CELLS+1];
} utran_fdd_neighbour_cell_t;

/*
WARNING: MAX_UTRAN_FDD_NEIGHBOUR_CELLS is set to 32 here. This is an arbitrary
limitation chosen because the MS must be able to monitor this many FDD cells.
There is no limit (except message length) imposed on how many FDD neighour
cells might appear in this message.
*/
typedef struct
{
#define MAX_UTRAN_FDD_NEIGHBOUR_CELLS            32

  boolean                                        bandwidth_fdd_flag;
  uint8                                          bandwidth_fdd;
  uint8                                          utran_fdd_neighbour_cell_count;
  utran_fdd_neighbour_cell_t *                   utran_fdd_neighbour_cells[MAX_UTRAN_FDD_NEIGHBOUR_CELLS];
} utran_fdd_description_t;

typedef enum {
  tdd_arfcn_valid,
  tdd_arfcn_index_valid
} tdd_arfcn_or_arfcn_index_valid_e;

typedef struct
{
#define MAX_NR_OF_TDD_CELLS                      31

  uint8                                          tdd_indic0;
  tdd_arfcn_or_arfcn_index_valid_e               arfcn_or_arfcn_index_valid;
  uint16                                         tdd_arfcn;
  uint8                                          tdd_arfcn_index;
  uint8                                          nr_of_tdd_cells;
  uint16                                         tdd_cell_information[MAX_NR_OF_TDD_CELLS+1];
}  utran_tdd_neighbour_cell_t;

/*
WARNING: MAX_UTRAN_TDD_NEIGHBOUR_CELLS is set to 32 here. This is an arbitrary
limitation chosen because the MS must be able to monitor this many TDD cells.
There is no limit (except message length) imposed on how many TDD neighour
cells might appear in this message.
*/
typedef struct
{
#define MAX_UTRAN_TDD_NEIGHBOUR_CELLS            32

  boolean                                        bandwidth_tdd_flag;
  uint8                                          bandwidth_tdd;
  uint8                                          utran_tdd_neighbour_cell_count;
  utran_tdd_neighbour_cell_t *                   utran_tdd_neighbour_cells[MAX_UTRAN_TDD_NEIGHBOUR_CELLS];
} utran_tdd_description_t;

/*
The following typedefs represent the CDMA 2000 Description
struct of the 3G Neighbour Cell Description struct.
*/
typedef enum
{
  CDMA2000_PILOT_UNKNOWN,
  CDMA2000_1X_COMMON_PILOT,
  CDMA2000_1X_COMMON_PILOT_WITH_TRANSMIT_DIVERSITY,
  CDMA2000_1X_AUXILIARY_PILOT,
  CDMA2000_1X_AUXILIARY_PILOT_WITH_TRANSMIT_DIVERSITY,
  CDMA2000_3X_COMMON_PILOT,
  CDMA2000_3X_AUXILIARY_PILOT
} cdma2000_pilot_e;

typedef struct
{
  uint16                                         pilot_pn_offset;
  cdma2000_pilot_e                               cdma2000_pilot;
  uint8                                          td_mode;
  uint8                                          td_power_level;
  uint8                                          qof;
  uint8                                          walsh_len_a;
  uint16                                         aux_pilot_walsh;
  uint8                                          walsh_len_b;
  uint16                                         aux_td_walsh;
  uint8                                          aux_td_power_level;
  uint8                                          sr3_prim_pilot;
  uint8                                          sr3_pilot_power1;
  uint8                                          sr3_pilot_power2;
  uint8                                          walsh_len_c;
  uint16                                         aux_walsh_len;
  boolean                                        walsh1_flag;
  uint8                                          qof1;
  uint8                                          walsh_length1;
  uint16                                         aux_pilot_walsh1;
  boolean                                        walsh2_flag;
  uint8                                          qof2;
  uint8                                          walsh_length2;
  uint16                                         aux_pilot_walsh2;
} cdma2000_cell_t;

typedef struct
{
  uint8                                          cdma2000_frequency_band;
  uint16                                         cdma2000_frequency;
  uint8                                          number_cdma2000_cells;
  cdma2000_cell_t                                *cdma2000_cells;
} cdma2000_description_t;

/*
The following typedefs represent the REMOVED 3G Cell Description
struct of the 3G Neighbour Cell Description struct.
*/

typedef struct
{
  uint8                                          removed_three_g_cell_index;
  uint8                                          three_g_cell_diff_length;
  uint8                                          three_g_cell_diff;
} removed_cell_t;

typedef struct
{
#define MAX_REMOVED_CELLS                        32
  uint8                                          number_of_removed_cells;
  removed_cell_t                                 removed_cells[MAX_REMOVED_CELLS];
} removed_cell_sublist_t;

typedef struct
{
#define MAX_REMOVED_CELL_SUBLISTS                4
  uint8                                          number_of_sublists;
  removed_cell_sublist_t                         removed_cell_sublists[MAX_REMOVED_CELL_SUBLISTS];
} removed_3g_cell_description_t;

/*
The following typedefs represent the 3G Neighbour Cell
Description struct of the Packet Measurement Order.
*/

typedef struct
{
  boolean                                        index_start_3g_flag;
  uint8                                          index_start_3g;

  boolean                                        absolute_index_start_emr_flag;
  uint8                                          absolute_index_start_emr;

  boolean                                        utran_fdd_description_flag;
  utran_fdd_description_t                        *utran_fdd_description;

  boolean                                        utran_tdd_description_flag;
  utran_tdd_description_t                        *utran_tdd_description;

  boolean                                        cdma2000_description_flag;
  cdma2000_description_t                         *cdma2000_description;

  boolean                                        removed_3g_cell_description_flag;
  removed_3g_cell_description_t                  *removed_3g_cell_description;
} three_g_neighbour_cell_desc_t;

typedef struct
{
  uint8                                          number_cells;
  uint8                                          *rep_priority;
} gprs_rep_priority_desc_t;

/* Start of structures for PACKET MEASUREMENT ORDER */

#define MAX_NC_MEAS_PARAMS_REMOVED_FREQS         32
#define MAX_NC_MEAS_PARAMS_ADDED_FREQS           96

typedef struct
{
  uint16                                         frequency;
  uint8                                          bsic;
  boolean                                        cell_selection_params_valid;
  cell_sel_params_t                              cell_selection_params;
} csn_nc_meas_params_nc_freq_list_added_freq_t;

typedef struct
{
  uint8                                          num_of_removed_freqs;
  uint8                                          removed_freqs[MAX_NC_MEAS_PARAMS_REMOVED_FREQS];
  uint8                                          num_of_added_freqs;
  csn_nc_meas_params_nc_freq_list_added_freq_t   *added_freqs[MAX_NC_MEAS_PARAMS_ADDED_FREQS];
} csn_nc_meas_params_nc_freq_list_t;

typedef struct
{
  uint8                                          network_control_order;
  boolean                                        nc_period_valid;
  uint8                                          nc_non_drx_period;
  uint8                                          nc_reporting_period_i;
  uint8                                          nc_reporting_period_t;
  boolean                                        nc_frequency_list_valid;
  csn_nc_meas_params_nc_freq_list_t              nc_frequency_list;
} csn_pmo_pcco_nc_meas_params_t;

/**
 * Structures that live inside the Enhanced Measurement Params.
 */
typedef struct
{
  boolean                                        index_start_3g_valid;
  uint8                                          index_start_3g;

  boolean                                        absolute_index_start_emr_valid;
  uint8                                          absolute_index_start_emr;

  boolean                                        utran_fdd_description_valid;
  utran_fdd_description_t                        *utran_fdd_description;

  boolean                                        utran_tdd_description_valid;
  utran_tdd_description_t                        *utran_tdd_description;

  boolean                                        cdma2000_description_valid;
  cdma2000_description_t                         *cdma2000_description;

  boolean                                        removed_3g_cell_description_valid;
  removed_3g_cell_description_t                  *removed_3g_cell_description;
} csn_3g_neighbour_cells_t;

#define MAX_GPRS_REP_PRIORITY_CELLS 127 /* 7 bit field gives number of entries  */

typedef struct
{
  uint8                                          num_of_cells;
  uint8                                          priority[MAX_GPRS_REP_PRIORITY_CELLS];
} csn_gprs_rep_priority_t;

typedef struct
{
  boolean                                        pmo_ind_valid;
  uint8                                          pmo_ind;

  boolean                                        report_type_valid;
  uint8                                          report_type;

  boolean                                        reporting_rate_valid;
  uint8                                          reporting_rate;

  boolean                                        invalid_bsic_reporting_valid;
  uint8                                          invalid_bsic_reporting;

  boolean                                        ncc_permitted_valid;
  uint8                                          ncc_permitted;

  boolean                                        multiband_reporting_valid;
  uint8                                          multiband_reporting;

  boolean                                        serving_band_reporting_valid;
  uint8                                          serving_band_reporting;

  boolean                                        scale_ord_valid;
  uint8                                          scale_ord;

  boolean                                        reporting_900_valid;
  uint8                                          reporting_900_offset;
  uint8                                          reporting_900_threshold;

  boolean                                        reporting_1800_valid;
  uint8                                          reporting_1800_offset;
  uint8                                          reporting_1800_threshold;

  boolean                                        reporting_450_valid;
  uint8                                          reporting_450_offset;
  uint8                                          reporting_450_threshold;

  boolean                                        reporting_1900_valid;
  uint8                                          reporting_1900_offset;
  uint8                                          reporting_1900_threshold;

  boolean                                        reporting_850_valid;
  uint8                                          reporting_850_offset;
  uint8                                          reporting_850_threshold;
} csn_gprs_meas_params_t;

typedef struct
{
  boolean                                        qsearch_p_valid;
  uint8                                          qsearch_p;

  boolean                                        fdd_gprs_qoffset_valid;
  uint8                                          fdd_gprs_qoffset;

  boolean                                        fdd_qmin_valid;
  uint8                                          fdd_qmin;

  boolean                                        tdd_gprs_qoffset_valid;
  uint8                                          tdd_gprs_qoffset;

  boolean                                        _3g_search_prio_valid;
  uint8                                          _3g_search_prio;

  boolean                                        fdd_rep_quant_valid;
  uint8                                          fdd_rep_quant;

  boolean                                        fdd_multirat_reporting_valid;
  uint8                                          fdd_multirat_reporting;

  boolean                                        fdd_reporting_offset_valid;
  uint8                                          fdd_reporting_offset;

  boolean                                        fdd_reporting_threshold_valid;
  uint8                                          fdd_reporting_threshold;

  boolean                                        tdd_multirat_reporting_valid;
  uint8                                          tdd_multirat_reporting;

  boolean                                        tdd_reporting_offset_valid;
  uint8                                          tdd_reporting_offset;

  boolean                                        tdd_reporting_threshold_valid;
  uint8                                          tdd_reporting_threshold;

  boolean                                        cdma2000_multirat_reporting_valid;
  uint8                                          cdma2000_multirat_reporting;

  boolean                                        cdma2000_reporting_offset_valid;
  uint8                                          cdma2000_reporting_offset;

  boolean                                        cdma2000_reporting_threshold_valid;
  uint8                                          cdma2000_reporting_threshold;
} csn_gprs_3g_meas_params_t;

typedef struct
{
  ba_or_psi3_change_mark_e                       ba_or_psi3_change_mark;
  uint8                                          ba_ind;
  uint8                                          _3g_ba_ind;
  uint8                                          psi3_change_mark;

  boolean                                        pmo_ind_valid;
  uint8                                          pmo_ind;

  uint8                                          report_type;
  uint8                                          reporting_rate;
  uint8                                          invalid_bsic_reporting;

  boolean                                        ncc_permitted_valid;
  uint8                                          ncc_permitted;

  boolean                                        _3g_neighbour_cells_valid;
  csn_3g_neighbour_cells_t                       *_3g_neighbour_cells;

  boolean                                        gprs_rep_priority_valid;
  csn_gprs_rep_priority_t                        *gprs_rep_priority;

  boolean                                        gprs_meas_params_valid;
  csn_gprs_meas_params_t                         *gprs_meas_params;

  boolean                                        gprs_3g_meas_params_valid;
  csn_gprs_3g_meas_params_t                      *gprs_3g_meas_params;
} csn_enh_meas_params_t;

typedef struct
{
  uint8                                          number_cells;
  uint8                                          ccn_supported[CCN_SUPPORT_BITMAP_LENGTH];
} csn_ccn_support_desc_t;

//tlx dime 2.1 with fr3009 FEATURE_GSM_TO_LTE added
typedef struct
{
  uint8 geran_priority;
  uint8 thresh_priority_search;
  uint8 thresh_gsm_low;
  uint8 h_prio;
  uint8 t_reselection;

} csn_gsm_priority_params_ie_t;

typedef struct
{
  /*! Bitmap indicating which frequencies the parameters in instances of this
  data structure are applicable to. */
  uint32  frequency_index_bitmap;
  boolean priority_valid;
  uint8   priority;
  uint8   thresh_high;
  boolean thresh_low_valid;
  uint8   thresh_low;
  boolean qrxlevmin_valid;
  uint8   qrxlevmin;

} csn_utran_priority_params_t;

#define CSN_UTRAN_PRIORITY_PARAMS_DESC_MAX 32

typedef struct
{
  boolean default_params_valid;
  struct
  {
    uint8 priority;
    uint8 thresh;
    uint8 qrxlevmin;
  } default_params;

  uint32                      utran_priority_params_count;
  csn_utran_priority_params_t utran_priority_params[CSN_UTRAN_PRIORITY_PARAMS_DESC_MAX];

} csn_utran_priority_params_ie_t;

typedef struct
{
  uint8   qsearch_p;
  uint8   rep_quant;
  uint8   multirat_reporting;

  boolean fdd_valid;
  uint8   fdd_reporting_threshold;
  boolean fdd_reporting_threshold_2_valid;
  uint8   fdd_reporting_threshold_2;
  boolean fdd_reporting_offset_valid;
  uint8   fdd_reporting_offset;

  boolean tdd_valid;
  uint8   tdd_reporting_threshold;
  boolean tdd_reporting_threshold_2_valid;
  uint8   tdd_reporting_threshold_2;
  boolean tdd_reporting_offset_valid;
  uint8   tdd_reporting_offset;

} csn_gprs_eutran_meas_params_t;

/**
  Repeated E-UTRAN Neighbour Cells

  If E-UTRAN cells or frequencies are included in the neighbour cell list,
  this information element may be included in the message.

  EARFCN (16 bit field)

  This field specifies the E-UTRA Absolute Radio Frequency Channel Number as
  defined in 3GPP TS 36.104.

  Measurement Bandwidth (3 bit field)

  This field specifies the minimum value of the channel bandwidth of all valid
  E-UTRAN cells on the specified EARFCN. It is defined by the parameter
  Transmission Bandwidth Configuration, NRB (see 3GPP TS 36.104). The values
  indicate the number of resource blocks over which the mobile station could
  measure. The field is coded according to the following table:

  bit
  3 2 1
  0 0 0       NRB = 6
  0 0 1       NRB = 15
  0 1 0       NRB = 25
  0 1 1       NRB = 50
  1 0 0       NRB = 75
  1 0 1       NRB = 100
  All others  Reserved for future use. If received by the mobile station, it
              shall be interpreted as '101'.

  When missing, this indicates NRB = 6.

  E-UTRAN_PRIORITY (3 bit field)
  THRESH_E-UTRAN_high (5 bit field)
  THRESH_E-UTRAN_low (5 bit field)
  If THRESH_E-UTRAN_low is not present, then THRESH_E-UTRAN_low shall take on
  the same value as THRESH_E-UTRAN_high.
  E-UTRAN_QRXLEVMIN (5 bit field)

  These fields are used for the inter-RAT cell re-selection algorithm based on
  priority, as defined in 3GPP TS 45.008. These values apply to each of the
  EARFCN values listed within this structure.
*/

/* Maximum number of EARFCNs that may be contained in each repeated E-UTRAN
Neighbour Cells struct */
#define CSN_EUTRAN_NEIGHBOUR_CELLS_MAX_EARFCNS 8

typedef struct
{
  uint8 earfcn_count;
  struct
  {
    uint16  earfcn;
    uint8   measurement_bandwidth;
  } earfcns[CSN_EUTRAN_NEIGHBOUR_CELLS_MAX_EARFCNS];

  boolean priority_valid;
  uint8   priority;

  uint8   thresh_high;

  boolean thresh_low_valid;
  uint8   thresh_low;

  boolean qrxlevmin_valid;
  uint8   qrxlevmin;

} csn_eutran_neighbour_cells_t;

/* Maximum PCID is 503, so total number of PCIDs is 504, which can be
represented in a 63 byte bitmap. */
#define CSN_PCID_GROUP_IE_MAX_PCID     503
#define CSN_PCID_GROUP_IE_BITMAP_LEN    63
#define CSN_PCID_GROUP_IE_MAX_PATTERNS  32

/*! Utility macro for setting a presence bit in the PCID Group */
#define CSN_PCID_GROUP_IE_SET(bitmap, pcid) \
  ( (bitmap)[(pcid)/8] |= (1<<((pcid) % 8)))

/*! Utility macro for checking a presence bit in the PCID group */
#define CSN_PCID_GROUP_IE_GET(bitmap, pcid) \
  (( (bitmap)[(pcid)/8] & (1<<((pcid) % 8))) ? TRUE : FALSE )

typedef struct
{
  uint8   bitmap[CSN_PCID_GROUP_IE_BITMAP_LEN];
  uint8   pcid_pattern_count;
  struct
  {
    uint8 length;
    uint8 value;
    uint8 sense;
  } pcid_patterns[CSN_PCID_GROUP_IE_MAX_PATTERNS];
} csn_pcid_group_ie_t;

/* Length of the eutran_frequency_indices_bitmap field. */
#define CSN_EUTRAN_FREQUENCY_INDICES_BITMAP_LEN 8

typedef struct
{
  csn_pcid_group_ie_t pcid_group;
  uint8               eutran_frequency_indices_bitmap;
} csn_eutran_not_allowed_cells_t;

/* LTE frequencies are referenced by a 3 bit frequency index, so the maximum
number of frequencies that RR needs to be able to decode / store is 8. */
#define CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS 8

typedef struct
{
  boolean                        ccn_active;

  boolean                        gprs_meas_params_valid;
  csn_gprs_eutran_meas_params_t  gprs_meas_params;

  uint8                          neighbour_cells_count;
  csn_eutran_neighbour_cells_t   neighbour_cells[CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS];

  uint8                          not_allowed_cells_count;
  csn_eutran_not_allowed_cells_t not_allowed_cells[CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS];

} csn_eutran_params_ie_t;

/* MAX_UARFCNS and MAX_EARFCNS are defined to the same values as used in
lte_irat_dedicated_priority_list_s structure. */
#define CSN_INDIVIDUAL_PRIORITIES_MAX_UARFCNS 64
#define CSN_INDIVIDUAL_PRIORITIES_MAX_EARFCNS 64

typedef enum
{
  CSN_UTRAN_TYPE_UNDEFINED,
  CSN_UTRAN_TYPE_FDD,
  CSN_UTRAN_TYPE_TDD,
  CSN_UTRAN_TYPE_MAX
} csn_utran_type_e;

typedef struct
{
  /* The 'valid' flag in this structure is actually a toggle - if it is
  TRUE then this structure provides individual priorities, if it is FALSE
  then any individual priorities held by the UE should be deleted. */
  uint8 valid;

  uint8 geran_priority;

  boolean utran_valid;
  struct
  {
    boolean default_priority_valid;
    uint8   default_priority;
    uint8   uarfcn_count;
    struct
    {
      csn_utran_type_e type;
      uint16           uarfcn;
      uint8            priority;
    } uarfcns[CSN_INDIVIDUAL_PRIORITIES_MAX_UARFCNS];
  } utran;

  boolean eutran_valid;
  struct
  {
    boolean default_priority_valid;
    uint8   default_priority;
    uint8   earfcn_count;
    struct
    {
      uint16 earfcn;
      uint8  priority;
    } earfcns[CSN_INDIVIDUAL_PRIORITIES_MAX_EARFCNS];
  } eutran;

  boolean t3230_timeout_value_valid;
  uint8   t3230_timeout_value;

} csn_individual_priorities_ie_t;

#define CSN_PSC_GROUP_IE_MAX_PSC            511
#define CSN_PSC_GROUP_IE_MAX_CELL_PARAMETER 128
#define CSN_PSC_GROUP_IE_BITMAP_LEN          64
#define CSN_PSC_GROUP_IE_MAX_PATTERNS        32

/*! Utility macro for setting a presence bit in the PSC Group */
#define CSN_PSC_GROUP_IE_SET(psc_group, psc) \
  ( (psc_group.bitmap)[(psc)/8] |= (1<<((psc) % 8)))

/*! Utility macro for checking a presence bit in the PSC group */
#define CSN_PSC_GROUP_IE_GET(psc_group, pcid) \
  (( (psc_group.bitmap)[(psc)/8] & (1<<((psc) % 8))) ? TRUE : FALSE )

typedef struct
{
  /* PSCs are stored in 9 bits, therefore we use a (2^9) bit bitmap to indicate
  the complete set of possible PSCs. */
  uint8 bitmap[CSN_PSC_GROUP_IE_BITMAP_LEN];
  uint8 psc_pattern_count;
  struct
  {
    uint8 length;
    uint8 value;
    uint8 sense;
  } psc_patterns[CSN_PSC_GROUP_IE_MAX_PATTERNS];
} csn_psc_group_ie_t;

#define CSN_UTRAN_CSG_DESCRIPTION_MAX_ENTRIES 32
typedef struct
{
  uint8 count;
  struct
  {
    csn_psc_group_ie_t csg_psc_split;
    uint32             utran_frequency_indices_bitmap;
  } entries[CSN_UTRAN_CSG_DESCRIPTION_MAX_ENTRIES];
} csn_utran_csg_description_ie_t;

#define CSN_EUTRAN_CSG_DESCRIPTION_MAX_ENTRIES 8
typedef struct
{
  uint8 count;
  struct
  {
    csn_pcid_group_ie_t csg_pci_split;
    uint8               eutran_frequency_indices_bitmap;
  } entries[CSN_EUTRAN_CSG_DESCRIPTION_MAX_ENTRIES];
} csn_eutran_csg_description_ie_t;

typedef struct
{
  boolean eutran_valid;
  struct
  {
    uint8 measurement_control;
    uint8 frequency_indices_bitmap;
  } eutran;

  boolean utran_valid;
  struct
  {
    uint8  measurement_control;
    uint32 frequency_indices_bitmap;
  } utran;
} csn_measurement_control_params_ie_t;

typedef struct
{
  csn_gsm_priority_params_ie_t                 * serving_cell_priority_params;
  csn_utran_priority_params_ie_t               * utran_priority_params;
  csn_eutran_params_ie_t                       * eutran_params;
} csn_priority_and_eutran_params_t;

//tlx dime 2.1 with fr3009 FEATURE_GSM_TO_LTE end

/**
 * Structure that a Packet Measurement Order will be decoded in to.
 */
typedef struct
{
  uint32                                         tlli;
  global_tfi_t                                   global_tfi;
  address_field_e                                tlli_gtfi_pkt_ref_valid;

  uint8                                          pmo_index;
  uint8                                          pmo_count;

  boolean                                        nc_meas_params_valid;
  csn_pmo_pcco_nc_meas_params_t *                nc_meas_params;

  boolean                                        _3g_neighbour_cells_valid;
  csn_3g_neighbour_cells_t      *                _3g_neighbour_cells;

  boolean                                        gprs_rep_priority_valid;
  csn_gprs_rep_priority_t       *                gprs_rep_priority;

  boolean                                        gprs_meas_params_valid;
  csn_gprs_meas_params_t        *                gprs_meas_params;

  boolean                                        gprs_3g_meas_params_valid;
  csn_gprs_3g_meas_params_t     *                gprs_3g_meas_params;

  boolean                                        r4_additions_flag;
  uint8                                          ccn_active;
  boolean                                        ccn_support_flag;
  csn_ccn_support_desc_t                         ccn_support_desc;
  boolean                                        r5_additions_flag;
  boolean                                        r6_additions_flag;
  uint8                                          three_g_ccn_active;
//tlx dime 2.1 with fr3009 FEATURE_GSM_TO_LTE
  boolean                                        ba_3g_ind_pmo_ind_valid;
  uint8                                          ba_3g_ind;
  uint8                                          pmo_ind;

  boolean                                        priority_and_eutran_params_valid;
  csn_priority_and_eutran_params_t               priority_and_eutran_params;
  csn_individual_priorities_ie_t               * individual_priorities;
  csn_utran_csg_description_ie_t               * utran_csg_description;
  csn_eutran_csg_description_ie_t              * eutran_csg_description;
  csn_measurement_control_params_ie_t          * measurement_control_params;
//tlx dime 2.1 with fr3009 FEATURE_GSM_TO_LTE end
} packet_measurement_order_t;

/**
 * Structure that a Packet Cell Change Order will be decoded in to.
    */
typedef struct
{
  uint32                                         tlli;
  global_tfi_t                                   global_tfi;
  address_field_e                                tlli_gtfi_pkt_ref_valid;

  pcco_cell_type_e                               gsm_3g_cell_type;
  boolean                                        immediate_rel;
  union
  {
    struct
    {
      CSN_ARFCN_TYPE                             arfcn;
      uint8                                      bsic;
      csn_pmo_pcco_nc_meas_params_t              nc_meas_params;

      boolean                                    psi3_change_mark_valid;
      uint8                                      psi3_change_mark;
      uint8                                      ba_ind;
      uint8                                      ba_ind_3g;

      boolean                                    _3g_neighbour_cells_valid;
      csn_3g_neighbour_cells_t      *            _3g_neighbour_cells;

      boolean                                    gprs_rep_priority_valid;
      csn_gprs_rep_priority_t       *            gprs_rep_priority;

      boolean                                    gprs_meas_params_valid;
      csn_gprs_meas_params_t        *            gprs_meas_params;

      boolean                                    gprs_3g_meas_params_valid;
      csn_gprs_3g_meas_params_t     *            gprs_3g_meas_params;

      boolean                                    r4_additions_flag;
      uint8                                      ccn_active;
      boolean                                    container_flag;
      uint8                                      container_id;
      boolean                                    ccn_support_flag;
      csn_ccn_support_desc_t                     ccn_support_desc;
      boolean                                    r5_additions_flag;
      boolean                                    r6_additions_flag;
      uint8                                      three_g_ccn_active;
    } gsm;

    struct
    {
      boolean                                    fdd_flag;
      uint16                                     fdd_arfcn;
      uint8                                      fdd_diversity;
      boolean                                    fdd_bandwidth_flag;
      uint8                                      fdd_bandwidth;
      uint16                                     fdd_scrambling_code;

      boolean                                    tdd_flag;
      uint16                                     tdd_arfcn;
      uint8                                      tdd_diversity;
      boolean                                    tdd_bandwidth_flag;
      uint8                                      tdd_bandwidth;
      uint8                                      tdd_cell_param;
      uint8                                      tdd_sync_case;
    } _3g;
  } cell;
} packet_cell_change_order_t;

/**
 * End of structures for PACKET MEASUREMENT ORDER/PACKET CELL CHANGE ORDER.
 */

typedef enum
{
  tbf_establishment,
  rr_establishment
} establishment_e;

typedef enum
{
  ptmsi_valid,
  tmsi_valid,
  ptmsi_mi_valid,
  tmsi_mi_valid
} ptmsi_tmsi_mi_flag_e;

typedef struct
{
#define REPEATED_PAGE_MOBILE_IDENTITY_SIZE       16
  uint32                                         ptmsi;
  uint8                                          length_of_mi;
  uint8                                          mobile_identity[REPEATED_PAGE_MOBILE_IDENTITY_SIZE];
  uint32                                         tmsi;
  uint8                                          channel_needed;
  uint8                                          emlpp_priority;
  boolean                                        emlpp_flag;
  establishment_e                                establishment;
  ptmsi_tmsi_mi_flag_e                           ptmsi_tmsi_mi_flag;
} repeated_page_t;

typedef struct
{
#define MAX_REPEATED_PAGE                        4
  uint8                                          persistence[MAX_PERSIST_VALUES];
  boolean                                        persistence_flag;
  uint8                                          nln;
  boolean                                        nln_flag;
  repeated_page_t                                repeated_page[MAX_REPEATED_PAGE];
  uint8                                          number_of_pages;
} packet_paging_request_t;

typedef struct
{
  uint8                                          timeslots_available;
} packet_pdch_release_t;

typedef struct
{
  uint32                                         tlli;
  global_tfi_t                                   global_tfi;
  uint16                                         tqi;
  address_field_e                                tlli_gtfi_pkt_ref_valid;
  uint8                                          type_of_ack;
} packet_polling_request_t;

typedef struct
{
  uint8                                          alpha;
  uint8                                          t_avg_w;
  uint8                                          t_avg_t;
  uint8                                          pb;
  uint8                                          pc_meas_chan;
  uint8                                          int_meas_chn_list;
  uint8                                          n_avg_i;
} global_pwr_ctrl_t;

typedef struct
{
  uint8                                          ta_value;
  uint8                                          uplink_ta_index;
  uint8                                          uplink_ta_ts_num;
  uint8                                          dwnlink_ta_index;
  uint8                                          dwnlink_ta_ts_num;
  boolean                                        ta_flag;
  boolean                                        uplink_ta_flag;
  boolean                                        dwnlink_ta_flag;
} global_pkt_timing_t;

typedef struct
{
#define TQI_FIELD_PRESENT                        0x2
#define PK_REQ_REF_PRESENT                       0x3
  uint16                                         tqi;
  pkt_req_ref_t                                  pk_req_ref;
  global_tfi_t                                   global_tfi;
  address_field_e                                tlli_gtfi_pkt_ref_valid;
  global_pwr_ctrl_t                              global_pwr_ctrl;
  pwr_ctrl_paramtrs_t                            pwr_ctrl_paramtrs;
  global_pkt_timing_t                            global_pkt_timing;
  boolean                                        global_pwr_ctrl_flag;
  boolean                                        global_timing_adv_flag;
  boolean                                        power_control_flag;
  boolean                                        r99_additions_flag;
  boolean                                        pkt_ext_ta_flag;
  uint8                                          pkt_ext_ta;
} packet_pwr_ctrl_tmng_adv_t;

typedef struct
{
  uint16                                         acc_contr_class;
  uint8                                          max_retrans[4];
  uint8                                          s;
  uint8                                          tx_int;
  uint8                                          persistence[MAX_PERSIST_VALUES];
  boolean                                        persistence_flag;
} packet_prach_parameters_t;

typedef struct
{
#define PQN_FIXED_BITS                           0x7
  pkt_req_ref_t                                  pk_req_ref;
  uint16                                         tqi;
} packet_queueing_notify_t;

typedef struct
{
  uint8                                          ext_dynamic_alloc;
  uint8                                          po;
  uint8                                          pr_mode;
  boolean                                        po_prmode_flag;
  uint8                                          usf_granularity;
  uint8                                          rlc_data_blks_granted;
  boolean                                        rlc_data_blks_granted_flag;
  start_frame_number_t                           tbf_start_frm_num;
  boolean                                        tbf_start_time_flag;
  uint8                                          usf_tn0;
  uint8                                          usf_tn1;
  uint8                                          usf_tn2;
  uint8                                          usf_tn3;
  uint8                                          usf_tn4;
  uint8                                          usf_tn5;
  uint8                                          usf_tn6;
  uint8                                          usf_tn7;
  uint8                                          usf_bitmap;
  uint8                                          alpha;
  uint8                                          gamma_tn0;
  uint8                                          gamma_tn1;
  uint8                                          gamma_tn2;
  uint8                                          gamma_tn3;
  uint8                                          gamma_tn4;
  uint8                                          gamma_tn5;
  uint8                                          gamma_tn6;
  uint8                                          gamma_tn7;
  uint8                                          gamma_bitmap;
  timeslot_alloc_flag_e                          timeslot_alloc_flag;
  uint8                                          uplk_tfi_assignment;
  boolean                                        uplk_tfi_assignment_flag;
} dynamic_alloc_t;

typedef struct
{
#define CSN_BLOCKS                               0
  boolean                                        message_escape;
  uint8                                          block_periods;
  uint8                                          alloc_bitmap_length;
  uint8                                          alloc_bitmap[16];
} allocation_bitmap_t;

typedef enum
{
  timeslot_alloc,
  power_control
} ts_alloc_pwr_flag_e;

typedef struct
{
  uint8                                          uplk_ts_alloc;
  pwr_ctrl_paramtrs_t                            pwr_ctrl_paramtrs;
  ts_alloc_pwr_flag_e                            ts_alloc_pwr_flag;
  uint8                                          final_alloc;
  uint8                                          dwnlk_ctrl_ts;
  boolean                                        dwnlk_pwr_ctrl_flag;
  dwnlk_pwr_ctrl_t                               dwnlk_pwr_ctrl;
  measure_mapping_t                              measure_mapping;
  boolean                                        meas_map_flag;
  start_frame_number_t                           tbf_start_frm_num;
  allocation_bitmap_t                            allocation_bitmap;
} fixed_alloc_t;

typedef struct
{
#define MAX_RECONF_MSG_LEN                       200
  global_tfi_t                                   global_tfi;
  uint8                                          chan_coding_cmd;
  global_pkt_timing_t                            global_pkt_timing;
  uint8                                          dwnlk_rlc_mode;
  uint8                                          control_ack;
  uint8                                          dwnlk_tfi_assignment;
  boolean                                        dwnlk_tfi_assignment_flag;
  uint8                                          uplk_tfi_assignment;
  boolean                                        uplk_tfi_assignment_flag;
  uint8                                          dwnlk_ts_alloc;
  freq_params_t                                  freq_params;
  boolean                                        freq_param_flag;
  dynamic_alloc_t                                dynamic_alloc;
  fixed_alloc_t                                  fixed_alloc;
  alloc_flag_e                                   alloc_flag;
  boolean                                        r99_additions_flag;
  boolean                                        pkt_ext_ta_flag;
  uint8                                          pkt_ext_ta;
  boolean                                        egprs_ptr;
  boolean                                        compact_reduced_ma_flag;
  compact_reduced_ma_t                           compact_reduced_ma;
  uint8                                          egprs_chan_coding_cmd;
  uint8                                          resegment;
  uint8                                          dl_egprs_window_size;
  boolean                                        dl_egprs_window_size_flag;
  uint8                                          ul_egprs_window_size;
  boolean                                        ul_egprs_window_size_flag;
  uint8                                          link_qual_meas_mode;

} packet_timeslot_reconfigure_t;

typedef struct
{
  global_tfi_t                                   global_tfi;
  uint8                                          uplink_release;
  uint8                                          dwnlink_release;
  uint8                                          tbf_release_cause;
} packet_tbf_release_t;

typedef struct
{
  uint8                                          final_ack_ind;
  uint8                                          strt_seq_num;
  uint8                                          received_blk_btmp[8];
} ack_nack_desc_t;


/* newer compilers use a packed format */
typedef PACKED struct PACKED_POST
{
  uint8                                          ext_length;
  uint64                                         extension;
} extension_bits_t;


typedef struct
{
  uint8                                          peak_thr_class;
  uint8                                          radio_priority;
  uint8                                          rlc_mode;
  uint8                                          llc_pdu_type;
  uint16                                         rlc_octet_cnt;
} channel_req_desc_t;

typedef struct
{
  uint8                                          pkt_est_cause;
  channel_req_desc_t                             channel_req_desc;
  boolean                                        pfi_present;
  uint8                                          pfi;
  uint8                                          mult_tbf_cap;
} csn_channel_req_desc_2_t;

typedef struct
{
  uint8                                          uplink_tfi;
  uint8                                          chan_coding_cmd;
  ack_nack_desc_t                                ack_nack_desc;
  uint32                                         contention_res_tlli;
  boolean                                        contention_res_flag;
  pkt_timing_adv_t                               pkt_timing_adv;
  boolean                                        pkt_timing_adv_flag;
  pwr_ctrl_paramtrs_t                            pwr_ctrl_paramtrs;
  boolean                                        pwr_ctrl_flag;
  extension_bits_t                               extension_bits;
  boolean                                        extension_bits_flag;
  uint8                                          final_alloc;
  uint8                                          ts_override;
  allocation_bitmap_t                            allocation_bitmap;
  boolean                                        fixed_alloc_flag;
  enum
  {
    repeated_alloc,
    bitmap_alloc
  } alloc_type_flag;

  start_frame_number_t                           tbf_start_frm_num;
  boolean                                        timeslot_alloc_flag;
  uint8                                          ts_allocation;
  boolean                                        r99_additions_flag;
  boolean                                        pkt_ext_ta_flag;
  uint8                                          pkt_ext_ta;
  uint8                                          tbf_est;
  boolean                                        egprs_puan;
  uint8                                          egprs_chan_coding_cmd;
  uint8                                          resegment;
  uint8                                          pre_emptive_tran;
  uint8                                          prr_retran_req;
  uint8                                          arac_retran_req;
  egprs_ack_nack_des_t                           egprs_ack_nack_des;
} packet_uplink_ack_nack_t;

typedef struct
{
  uint8                                          timeslot_num;
  uint8                                          alpha;
  boolean                                        alpha_gamma_flag;
  uint8                                          gamma_tn;
  uint8                                          po;
  uint8                                          bts_pwr_ctrl_mode;
  uint8                                          pr_mode;
  boolean                                        po_prmode_flag;
  start_frame_number_t                           tbf_start_frm_num;
} sngl_blk_alloc_t;

typedef struct
{
#define EXTENSION                                0x0
#define DYNAMIC_ALLOC                            0x1
#define SINGLE_BLK_ALLOC                         0x2
#define FIXED_ALLOC                              0x3
#define MAX_ACCESS_TECH_TYPE                     16
#define MULTI_BLOCK_ALLOC                        0x2

  uint8                                          persistence[MAX_PERSIST_VALUES];
  boolean                                        persistence_flag;
  global_tfi_t                                   global_tfi;
  address_field_e                                tlli_gtfi_pkt_ref_valid;
  uint32                                         tlli;
  uint16                                         tqi;
  pkt_req_ref_t                                  pk_req_ref;
  uint8                                          chan_coding_cmd;
  uint8                                          tlli_blk_chan_coding;
  pkt_timing_adv_t                               pkt_timing_adv;
  freq_params_t                                  freq_params;
  boolean                                        freq_param_flag;
  dynamic_alloc_t                                dynamic_alloc;
  sngl_blk_alloc_t                               sngl_blk_alloc;
  enum
  {
    extension_alloc,
    dynamic_alloc_e,
    single_blk_alloc,
    fixed_alloc,
    multi_blk_allocation
  } allocation_flag;

  uint8                                          uplk_tfi_assignment;
  boolean                                        uplk_tfi_assignment_flag;
  uint8                                          final_alloc;
  uint8                                          dwnlk_ctrl_ts;
  uint8                                          po;
  uint8                                          pr_mode;
  boolean                                        po_prmode_flag;
  uint8                                          ts_allocation;
  pwr_ctrl_paramtrs_t                            pwr_ctrl_paramtrs;
  ts_alloc_pwr_flag_e                            ts_alloc_pwr_flag;
  uint8                                          half_duplex;
  start_frame_number_t                           tbf_start_frm_num;
  allocation_bitmap_t                            allocation_bitmap;
  uint8                                          bts_pwr_ctrl_mode;
  boolean                                        r99_additions_flag;
  boolean                                        pkt_ext_ta_flag;
  uint8                                          pkt_ext_ta;
  boolean                                        egprs_pua;
  boolean                                        cont_res_tlli_flag;
  uint32                                         cont_res_tlli;
  boolean                                        compact_reduced_ma_flag;
  compact_reduced_ma_t                           compact_reduced_ma;
  uint8                                          egprs_chan_coding_cmd;
  uint8                                          resegment;
  uint8                                          egprs_window_size;
  boolean                                        access_technologies_req_flag;
  uint8                                          access_technologies_req[MAX_ACCESS_TECH_TYPE];
  uint8                                          access_tech_reg_length;
  uint8                                          arac_retran_req;
  boolean                                        bep_period2_flag;
  uint8                                          bep_period2;
  multi_blk_alloc_t                              multi_blk_alloc;
} packet_uplink_assignment_t;

typedef struct
{
  uint8                                          persistence[MAX_PERSIST_VALUES];
  boolean                                        persistence_flag;
} packet_dwnlk_dummy_cntrl_blk_t;

typedef struct {
  boolean                                        egprs_supported_flag;
  uint8                                          egprs_packet_channel_request;
  uint8                                          bep_period;
  uint8                                          pfc_feature_mode;
  uint8                                          dtm_support;
  uint8                                          bss_paging_coordination;
  uint8                                          ccn_active;
  uint8                                          nw_ext_tbf;
  uint8                                          multiple_tbf_capability;
  uint8                                          ext_utbf_no_data;
  uint8                                          dtm_enhancements_capability;
} optional_extension_info_t;

typedef struct
{
  uint8                                          nmo;
  uint8                                          t3168;
  uint8                                          t3192;
  uint8                                          drx_timer_max;
  uint8                                          access_burst_type;
  uint8                                          ctrl_ack_type;
  uint8                                          bs_cv_max;
  uint8                                          pan_dec;
  uint8                                          pan_inc;
  uint8                                          pan_max;
  boolean                                        pan_flag;
  boolean                                        optional_extension_info_flag;
  optional_extension_info_t                      optional_extension_info;
} gprs_cell_options_t;

typedef struct
{
  uint8                                          alpha;
  uint8                                          t_avg_w;
  uint8                                          t_avg_t;
  boolean                                        pc_meas_chan;
  uint8                                          n_avg_i;
} gprs_power_control_params_t;

typedef struct
{
  uint8                                          bs_pcc_rel;
  uint8                                          bs_pbcch_blks;
  uint8                                          bs_pag_blks_res;
  uint8                                          bs_prach_blks;
} pccch_org_params_t;

typedef struct
{
  uint8                                          pbcch_chng_mrk;
  uint8                                          psi_chng_fld;
  uint8                                          psi1_rpt_period;
  uint8                                          psi_cnt_lr;
  uint8                                          psi_cnt_hr;
  boolean                                        psi_cnt_hr_flag;
  uint8                                          meas_order;
  gprs_cell_options_t                            gprs_cell_options;
  packet_prach_parameters_t                      prach_params;
  pccch_org_params_t                             pccch_org_params;
  global_pwr_ctrl_t                              global_pwr_ctrl;
  uint8                                          psi_status_ind;
  boolean                                        r99_additions_flag;
  uint8                                          mscr;
  uint8                                          sgsnr;
  uint8                                          band_indicator;
} packet_psi1_t;

typedef struct
{
  /* The number of octets in the RFL data is length_rfl_data + 3, as the length is a */
  /* minimum of 3 octets, but this min value is not included in the encoded length value */
  /* e.g. actual_length_rfl_data = length_rfl_data + CSN_RFL_DATA_LEN_MIN_VAL_OFFSET */

  uint8                                          rfl_num;
  uint8                                          length_rfl_data;
  uint8                                          rfl[18];
} ref_freq_struct_t;

typedef struct
{
  uint8                                          ma_number;
  gprs_mobile_alloc_t                            gprs_ma_alloc;
} gprs_ma_t;

typedef struct
{
  uint8                                          cell_bar_access_2;
  uint8                                          exc_acc;
  uint8                                          gprs_rxlev_access_min;
  uint8                                          gprs_ms_txpwr_max_cch;
  hcs_params_t                                   hcs_params;
  boolean                                        hcs_flag;
  uint8                                          multiband_rprt;
} serving_cell_t;

typedef struct
{
  uint8                                          gprs_cell_resel_hyst;
  uint8                                          c31_hyst;
  uint8                                          c32_qual;
  uint8                                          random_access_retry;
  uint8                                          t_resel;
  boolean                                        t_resel_flag;
  uint8                                          ra_resel_hyst;
  boolean                                        ra_resel_hyst_flag;
} gen_cell_sel_t;

typedef struct
{
  uint8                                          freq_diff;
  uint8                                          bsic;
  cell_sel_params_t                              cel_sel_params;
} ncell_t;

typedef struct
{
#define MAX_NCELLS                               15

  uint16                                         start_freq;
  uint8                                          bsic;
  cell_sel_params_t                              cel_sel_params; /* bsic has been pulled out of cel_sel_params  */
  uint8                                          nr_remaining_cells;
  uint8                                          freq_diff_length;
  ncell_t                                        ncell[15];
} neighbour_cell_t;

typedef struct
{
  uint8                                          psi3_chng_mrk;
  uint8                                          psi3_bis_count;
  serving_cell_t                                 serving_cell;
  gen_cell_sel_t                                 gen_cell_sel;
  neighbour_cell_t                               *neighbour_cell[MAX_NCELLS];
  uint8                                          num_nc_structs;
  boolean                                        r98_additions_flag;
  boolean                                        r99_additions_flag;
  boolean                                        r4_additions_flag;
  boolean                                        ccn_support_flag;
  csn_ccn_support_desc_t                         ccn_support_desc;
} packet_psi3_t;

typedef struct
{
  uint8                                          ncc;
  boolean                                        ncc_flag;
  uint8                                          exc_acc;
  uint8                                          gprs_rxlev_access_min;
  boolean                                        gprs_rxlev_flag;
  uint8                                          gprs_ms_txpwr_max_cch;
  boolean                                        gprs_txpwr_max_flag;
  uint8                                          priority_class;
  boolean                                        priority_class_flag;
  uint8                                          hcs_thr;
  boolean                                        hcs_thr_flag;
  si13_pbcch_loc_t                               si13_pbcch_loc;
  boolean                                        si13_pbcch_loc_flag;
  uint8                                          gprs_temp_offset;
  uint8                                          gprs_penalty_time;
  uint8                                          gprs_reselect_offset;
} neighbour_sets_t;

typedef struct
{
  uint8                                          freq_diff;
  uint8                                          same_ra_as_serv_cell;
  uint8                                          cell_bar_access2;
  uint8                                          bcc;
} ncell_2_t;

typedef struct
{
  uint16                                         start_freq;
  uint8                                          same_ra_as_serv_cell;
  uint8                                          cell_bar_access_2;
  uint8                                          bcc;
  uint8                                          nr_remaining_cells;
  uint8                                          freq_diff_length;
  ncell_2_t                                      ncell[15];
} multi_start_freq_t;

typedef struct
{
#define MAX_MULTI_START_FREQ                     10
  multi_start_freq_t                             *multi_start_freq[MAX_MULTI_START_FREQ];
  uint8                                          start_f_count;
  uint8                                          cell_param_ptr;
} cell_params_ptr_t;

typedef struct
{
  cell_params_ptr_t                              cell_params_ptr[4];
  uint8                                          cell_param_ptr_count;
  neighbour_sets_t                               neighbour_sets[4];
  uint8                                          neighbour_sets_count;
} neighbour_cell_2_t;

typedef struct
{
#define MAX_MULTIPLE_START                       10
  uint8                                          psi3_chng_mrk;
  uint8                                          psi3_bis_index;
  uint8                                          psi3_bis_count;
  neighbour_cell_t                               *neighbour_cell[MAX_NCELLS];
  uint8                                          neighbour_cell_count;
  neighbour_cell_2_t                             *neighbour_cell_2[MAX_NCELLS];
  uint8                                          neighbour_cell_2_count;
  boolean                                        r98_additions_flag;
  boolean                                        r99_additions_flag;
  boolean                                        r4_additions_flag;
  boolean                                        ccn_support_flag;
  csn_ccn_support_desc_t                         ccn_support_desc;
} packet_psi3_bis_t;

typedef struct
{
  CSN_ARFCN_TYPE                                 arfcn;
  enum
  {
    valid_arfcn,
    valid_ma_maio
  } arfcn_ma_flag;

  uint8                                          ma_number;
  uint8                                          maio;
  uint8                                          timeslot_alloc;
} int_meas_chan_lst_t;

typedef struct
{
#define MAX_CHAN_GRP_STRCT                       10
  uint8                                          psi4_chng_mrk;
  uint8                                          psi4_index;
  uint8                                          psi4_count;
  int_meas_chan_lst_t                            int_meas_chan_lst[MAX_CHAN_GRP_STRCT];
  uint8                                          chan_grp_count;
} packet_psi4_t;

typedef struct
{
  uint8                                          network_control_order;
  boolean                                        nc_period_valid;
  uint8                                          nc_non_drx_period;
  uint8                                          nc_reporting_period_i;
  uint8                                          nc_reporting_period_t;
} csn_nc_meas_params_t;

typedef struct
{
  uint8                                          psi5_chng_mrk;
  uint8                                          psi5_index;
  uint8                                          psi5_count;

  boolean                                        nc_meas_params_valid;
  csn_nc_meas_params_t                           nc_meas_params;

  boolean                                        ext_meas_params_valid;
  ext_meas_params_t                              *ext_meas_params;

  boolean                                        gprs_meas_params_valid;
  csn_gprs_meas_params_t                         *gprs_meas_params;

  boolean                                        gprs_3g_meas_params_valid;
  csn_gprs_3g_meas_params_t                      *gprs_3g_meas_params;
} packet_psi5_t;

#define INVALID_LB_MS_TXPWR_MAX_CCH              0xFF

/* SI2n support - see 3GPP TS 44.018 v6.15.0  */
#define SI2n_NOT_SUPPORTED                       0x00 /* SI2n is not supported  */
#define SI2n_SUPPORTED_PACCH                     0x01 /* SI2n is supported on PACCH */
#define SI2n_SUPPORTED_PACCH_BCCH_NORM           0x02 /* SI2n is supported on PACCH and BCCH  */
#define SI2n_SUPPORTED_PACCH_BCCH_EXTD           0x03 /* SI2n is supported on PACCH and BCCH Ext  */

typedef struct
{
  uint8                                          bcch_chng_mrk;
  uint8                                          si_chng_field;
  uint8                                          psi13_chng_mrk;
  boolean                                        si13_cm_gprs_ma_flag;
  gprs_mobile_alloc_t                            gprs_mobile_alloc;
  uint8                                          rac;
  uint8                                          spgc_ccch_sup;
  uint8                                          priority_access_thr;
  uint8                                          network_ctrl_ordr;
  gprs_cell_options_t                            gprs_cell_options;
  uint8                                          alpha;
  uint8                                          t_avg_w;
  uint8                                          t_avg_t;
  uint8                                          pc_meas_chan;
  uint8                                          n_avg_i;
  boolean                                        pbcch_present_flag;
  uint8                                          psi1_repeat_period;
  uint8                                          pb;
  uint8                                          tsc;
  uint8                                          tn;
  CSN_ARFCN_TYPE                                 arfcn;
  uint8                                          maio;
  csn_bcch_arfcn_maio_t                          bcch_arfcn_maio_flag;
  boolean                                        r99_additions_flag;
  uint8                                          sgsnr;
  boolean                                        r4_additions_flag;
  uint8                                          si_status_ind;
  boolean                                        r6_additions_flag;
  uint8                                          lb_ms_txpwr_max_cch;
  uint8                                          si2n_support;
} packet_psi13_t;

typedef struct
{
  uint8                                          bcch_chng_mrk;
  boolean                                        si13_cm_gprs_ma_flag;
  uint8                                          si13_chng_mrk;
  gprs_mobile_alloc_t                            gprs_mobile_alloc;
  uint8                                          rac;
  uint8                                          spgc_ccch_sup;
  uint8                                          priority_access_thr;
  uint8                                          network_ctrl_ordr;
  gprs_cell_options_t                            gprs_cell_options;
  uint8                                          alpha;
  uint8                                          t_avg_w;
  uint8                                          t_avg_t;
  uint8                                          pc_meas_chan;
  uint8                                          n_avg_i;
  uint8                                          sgsnr;
  uint8                                          si_status_ind;
  uint8                                          lb_ms_txpwr_max_cch;
  uint8                                          si2n_support;
} ccch_access_info_t;

typedef struct
{
  uint8                                          psi1_repeat_period;
  uint8                                          pb;
  uint8                                          tn;
  freq_params_t                                  freq_params;
  uint8                                          psi_changed_ind;
} pbcch_desc_t;

typedef struct
{
  uint8                                          ccch_pbcch_flag;
  union
  {
    ccch_access_info_t                           ccch_access_info;
    pbcch_desc_t                                 pbcch_description;
  } ccch_or_pbcch;
  boolean                                        dtm_additions_flag;
  boolean                                        r6_additions_flag;
} packet_psi14_t;

/*
Packet PSI3 Quater data type
*/
typedef struct
{
  uint8                                          _3g_ba_ind;
  uint8                                          qsearch_i;
  uint8                                          qsearch_c_initial;
  boolean                                        fdd_info;
  uint8                                          fdd_qoffset;
  uint8                                          fdd_rep_quant;
  uint8                                          fdd_multirat_reporting;
  boolean                                        tdd_info;
  uint8                                          tdd_qoffset;
  uint8                                          tdd_multirat_reporting;
} three_g_initial_ded_mode_rep_desc_t;

/*
Packet PSI3 Quater data type
*/
typedef struct
{

  uint8                                          psi3_change_mark;
  uint8                                          psi3_quater_index;
  uint8                                          psi3_quater_count;

  boolean                                        gprs_rep_priority_flag;
  gprs_rep_priority_desc_t                       gprs_rep_priority_desc;

  boolean                                        three_g_ncells_desc_flag;
  struct
  {
    boolean                                      index_start_3g_flag;
    uint8                                        index_start_3g;
    boolean                                      absolute_index_start_emr_flag;
    uint8                                        absolute_index_start_emr;
    boolean                                      utran_fdd_desc_flag;
    utran_fdd_description_t                      utran_fdd_desc;
    boolean                                      utran_tdd_desc_flag;
    utran_tdd_description_t                      utran_tdd_desc;
  } three_g_ncells_desc;

  boolean                                        gprs_3g_meas_params_valid;
  csn_gprs_3g_meas_params_t                      gprs_3g_meas_params;

  boolean                                        three_g_initial_ded_mode_rep_desc_flag;
  three_g_initial_ded_mode_rep_desc_t            three_g_initial_ded_mode_rep_desc;

  boolean                                        three_g_additional_meas_params_valid;
  uint8                                          fdd_qmin_offset;
  uint8                                          fdd_rscpmin;

  boolean                                        three_g_additional_meas_params_2_valid;
  uint8                                          fdd_reporting_threshold_2;

  boolean                                        three_g_ccn_active_valid;
  uint8                                          three_g_ccn_active;
} packet_psi3_quater_t;
/*
End of Packet PSI3 Quater data type
*/

/*
Packet PSI15 data type
*/

typedef struct
{
#define MAX_FDD_ARFCNS                           8
#define MAX_TDD_ARFCNS                           8

  uint8                                          fdd_arfcn_count;
  uint16                                         fdd_arfcn[MAX_FDD_ARFCNS];
  uint8                                          tdd_arfcn_count;
  uint16                                         tdd_arfcn[MAX_TDD_ARFCNS];
} utran_frequency_list_t;

typedef struct
{
  boolean                                        utran_frequency_list_flag;
  utran_frequency_list_t                         utran_frequency_list;
} packet_psi15_t;
/*
End of Packet PSI15 data type
*/

/* Definitions for Packet Serving Cell Data   (PSCD) */
/*             and Packet Neighbour Cell Data (PNCD) (NACC builds only) */
/* A PSCD msg is at most 22 octets, of which the first 3 octets are already used */
/* Since one repeat structure must consist of at least 2 octets (pd/cd hdr+data) */
/* we can determine that the maximum number of repeat structures in one msg is 9 */
#define MAX_CONTAINER_DATA_STRUCTS               9

/* Each container repetition struct has a length determined by a 5 bit field   */
/* The values 0x00 and 0x1F are reserved, so there are 30 usable length values */
/* In fact, the spec only allows the encoding for lengths of between 0 and 18  */
#define MAX_CONTAINER_DATA_OCTETS                18

/* CD LENGTH can be set to a special value to indicate "data fills this msg" */
/* See specifications in 3GPP 44.060, Version 4.1.0, section 11.2.9e         */
#define CONTAINER_DATA_FILLS_MSG                 0x1F

#define BCCH_PD                                  0x00
#define PBCCH_PD                                 0x01
#define DUMMY_PD                                 0xFF

typedef struct
{
  uint8                                          pd;
  uint8                                          cd_length;
  uint8                                          ex_length;
  uint8                                          data[MAX_CONTAINER_DATA_OCTETS];
} container_data_t;

typedef struct
{
  uint8                                          num_data_structs;
  container_data_t                               data_structs[MAX_CONTAINER_DATA_STRUCTS];
  uint8                                          octet_counts[2];
  uint8                                          struct_counts[2];
  boolean                                        last_struct;
} container_repetition_struct_t;

typedef struct
{
  global_tfi_t                                   global_tfi;
  uint8                                          container_index;
  container_repetition_struct_t                  container_repetition_struct;
} packet_scell_data_t;

typedef struct
{
  uint8                                          container_data[CSN_CONTAINER_DATA_LEN];
} packet_serving_cell_si_t;

typedef struct
{
  global_tfi_t                                   global_tfi;
  uint8                                          container_id;
  uint8                                          container_index;
  boolean                                        arfcn_bsic_included;
  CSN_ARFCN_TYPE                                 arfcn;
  uint8                                          bsic;
  container_repetition_struct_t                  container_repetition_struct;
} packet_ncell_data_t;

typedef struct
{
  global_tfi_t                                   global_tfi;
  uint8                                          container_length;
  uint8                                          container_data[CSN_PKT_CS_COMMAND_MAX_OCTETS+1];
} packet_cs_command_t;

#define MAX_NUM_MULTIPLE_DL_ASSIGNMENTS 4
#define MAX_NUM_DL_TBF_ASSIGNMENTS      4
#define MAX_NUM_UL_TBF_ASSIGNMENTS      4

typedef struct
{
  boolean pfi_present;
  byte    pfi;
  byte    downlink_rlc_mode;
  boolean uplink_control_timeslot_present;
  byte    uplink_control_timeslot;
  byte    tfi_assignment;
  byte    control_ack;
  boolean dl_egprs_window_size_present;
  byte    dl_egprs_window_size;
} dl_tbf_assign_t;

typedef struct
{
  byte            timeslot_allocation;
  boolean         uplink_control_timeslot_present;
  byte            uplink_control_timeslot;
  byte            num_dl_tbf_assignments;
  dl_tbf_assign_t dl_tbf_assignments[MAX_NUM_DL_TBF_ASSIGNMENTS];
} multiple_dl_assign_t;

typedef struct
{
  byte    num_timeslots;
  boolean pfi_present;
  byte    pfi;
  byte    tfi_assignment;
  boolean channel_coding_present;
  byte    channel_coding_command;
  boolean egprs_channel_coding_present;
  byte    egprs_channel_coding_command;
  boolean ul_egprs_window_size_present;
  byte    ul_egprs_window_size;
  byte    usf_granularity;
  boolean timeslot_allocation_present;
  byte    timeslot_allocation;
  boolean usf_allocation_per_slot;
  byte    num_usf_allocations;
  byte    usf_allocation[8];
} ul_tbf_assign_t;

typedef struct
{
  byte                ms_timeslot_allocation;
  boolean             power_control_included;
  pwr_ctrl_paramtrs_t power_control_params;
} timeslot_description_t;

typedef struct
{
  byte                   extended_dynamic_allocation;
  boolean                p0_present;
  byte                   p0;
  byte                   pr_mode;
  boolean                timeslot_description_present;
  timeslot_description_t timeslot_description;
  byte                   num_ul_tbf_assignments;
  ul_tbf_assign_t        ul_tbf_assignments[MAX_NUM_UL_TBF_ASSIGNMENTS];
} multiple_ul_assign_t;

typedef enum
{
  EDTM_MAINTAIN_EXISTING_TBFS  = 0x00,
  EDTM_CONVERT_TO_FULL_RATE    = 0x01,
  EDTM_USE_ALLOCATION_PROVIDED = 0x02,
  EDTM_RESERVED_TREAT_AS_00    = 0x03
} edtm_realloc_type_t;

typedef struct
{
  global_tfi_t                     global_tfi;
  byte                             enhanced_dtm_cs_release_ind;
  global_pkt_timing_t              global_pkt_timing;
  edtm_realloc_type_t              edtm_realloc_type;
  byte                             egprs_mode_tbfs;
  boolean                          freq_params_present;
  freq_params_t                    freq_params;
  boolean                          extended_timing_advance_present;
  byte                             extended_timing_advance;
  boolean                          bep_period2_present;
  byte                             bep_period2;
  boolean                          channel_coding_present;
  byte                             channel_coding_command;
  byte                             num_mult_dl_assignments;
  multiple_dl_assign_t             multiple_dl_assignments[MAX_NUM_MULTIPLE_DL_ASSIGNMENTS];
  byte                             num_mult_ul_assignments;
  multiple_ul_assign_t             multiple_ul_assignment;
  byte                             link_quality_measurement_mode;
  boolean                          egprs_channel_coding_present;
  byte                             egprs_channel_coding_command;
  byte                             resegment;
  boolean                          dl_egprs_window_size_present;
  byte                             dl_egprs_window_size;
  boolean                          ul_egprs_window_size_present;
  byte                             ul_egprs_window_size;
} packet_cs_release_ind_t;

typedef struct
{
  boolean pfi_present;
  byte    pfi;
  byte    downlink_rlc_mode;
  byte    tfi_assignment;
  byte    control_ack;
  boolean dl_egprs_window_size_present;
  byte    dl_egprs_window_size;
} dl_tbf_assignment_t;

typedef struct
{

  byte    timeslot_allocation;
  dl_tbf_assignment_t dl_tbf_assignment;
} dl_assignment_t;

typedef struct
{
  byte    num_timeslots;
  boolean pfi_present;
  byte    pfi;
  byte    rlc_mode;
  byte    tfi_assignment;
  boolean channel_coding_present;
  byte    channel_coding_command;
  boolean egprs_channel_coding_present;
  byte    egprs_channel_coding_command;
  boolean ul_egprs_window_size_present;
  byte    ul_egprs_window_size;
  byte    usf_granularity;
  boolean timeslot_allocation_present;
  byte    timeslot_allocation;
  boolean usf_allocation_per_slot;
  byte    num_usf_allocations;
  byte    usf_allocation[8];
} ul_tbf_assignment_t;

typedef struct
{
  boolean                channel_coding_command_present;
  byte                   channel_coding_command;
  boolean                timeslot_description_present;
  timeslot_description_t timeslot_description;
  byte                   num_ul_tbf_assignments;
  ul_tbf_assignment_t    ul_tbf_assignments[MAX_NUM_UL_TBF_ASSIGNMENTS];
  byte                   num_dl_assignments;
  dl_assignment_t        dl_assignments[MAX_NUM_DL_TBF_ASSIGNMENTS];
} gprs_mode_t;

typedef struct
{
  boolean                ul_egprs_window_size_present;
  byte                   ul_egprs_window_size;
  boolean                egprs_channel_coding_present;
  byte                   egprs_channel_coding_command;
  boolean                bep_period2_present_flag;
  byte                   bep_period2;
  boolean                timeslot_description_present;
  timeslot_description_t timeslot_description;
  byte                   num_ul_tbf_assignments;
  ul_tbf_assignment_t    ul_tbf_assignments[MAX_NUM_UL_TBF_ASSIGNMENTS];
} egprs_ul_tbf_t;

typedef struct
{
  boolean         dl_egprs_window_size_present;
  byte            dl_egprs_window_size;
  boolean         bep_period2_present_flag;
  byte            bep_period2;
  boolean         link_quality_meas_mode_present;
  byte            link_quality_meas_mode;
  byte            num_dl_assignments;
  dl_assignment_t dl_assignments[MAX_NUM_DL_TBF_ASSIGNMENTS];
} egprs_dl_tbf_t;

typedef struct
{
  egprs_ul_tbf_t   egprs_ul_tbf;
  boolean          egprs_dl_tbf_present;
  egprs_dl_tbf_t   egprs_dl_tbf;
} egprs_mode_t;

typedef struct
{
  boolean                 handover_reference_present;
  byte                    handover_reference;
  CSN_ARFCN_TYPE          arfcn;
  byte                    si;
  byte                    nci;
  byte                    bsic;
  boolean                 ccn_active_present;
  byte                    ccn_active;
  boolean                 ccn_3g_active_present;
  byte                    ccn_3g_active;
  boolean                 ccn_support_desc_present;
  csn_ccn_support_desc_t  ccn_support_desc;
  freq_params_t           freq_params;
  byte                    network_control_order;
  boolean                 global_pkt_timing_present;
  global_pkt_timing_t     global_pkt_timing;
  boolean                 pkt_ext_timing_advance_present;
  byte                    pkt_ext_timing_advance;
  byte                    ext_dynamic_allocation;
  byte                    rlc_reset;
  boolean                 pwr_control_present;
  byte                    po;
  byte                    pr_mode;
  boolean                 ul_ctrl_timeslot_present;
  byte                    ul_ctrl_timeslot;
  byte                    type_of_mode;
  union
  {
    gprs_mode_t             gprs_mode;
    egprs_mode_t            egprs_mode;
  }tbf_mode;
} ps_handover_radio_resources_t;

typedef struct
{
  byte                       rrc_container_length;
  byte                       rrc_container_data[255];
} rrc_container_t;

typedef struct
{
  byte                       nas_container_length;
  byte                       nas_container_data[9];
} nas_container_t;

typedef struct
{
  ps_handover_radio_resources_t ps_handover_radio_resources;
  boolean                       nas_container_present;
  nas_container_t               nas_container;
} ps_handover_to_a_gb_mode_payload_t;

typedef struct
{
  rrc_container_t       rrc_container;
} ps_handover_to_utran_payload_t;


typedef enum
{
  CSN_PSHO_GSM_TARGET_CELL,
  CSN_PSHO_3G_TARGET_CELL
} psho_target_type_e;


typedef struct
{
  boolean                    global_tfi_present;
  global_tfi_t               global_tfi;
  byte                       container_id;

  psho_target_type_e         target_type;
  union
  {
    ps_handover_to_a_gb_mode_payload_t  a_gb;
    ps_handover_to_utran_payload_t      utran;
  }payload;

} ps_handover_command_t;

typedef struct
{
  global_tfi_t               global_tfi;
  byte                       timing_advance_value;
} packet_physical_information_t;

typedef struct
{
  uint8 container_data[CSN_CONTAINER_DATA_LEN];
} packet_si_data_t;

/*****************************************************************************/
typedef struct
{
  /*  Error Code */
  csn_decoder_error_t                            error_code;
  /* Message Type */
  uint8                                          message_type;
  /* Page mode field coding, 2 bit field. */
  uint8                                          page_mode;
  uint8                                          ___padding___;
} rlc_mac_cntrl_t;

/*****************************************************************************/
/* NOTE:- The CSN_DECODE_HDR_SZ macro includes a sizeof() check that will    */
/* cause a compiler error (div by 0) if the value is not as expected         */
/*****************************************************************************/

#if defined(PERLUTF) || defined(WIN32)
  /* Visual C uses 4 bytes for enums, the test cases expect
     the struct to occupy 8 bytes */
  #define CSN_DECODE_HDR_EXPECTED_SZ 8
#else
  /* ARM uses 1 byte for enums where possible, the end of the
     struct must be aligned to a byte boundary */
  #define CSN_DECODE_HDR_EXPECTED_SZ 4
#endif /* PERLUTF */

/*lint -emacro((54,414,506),CSN_DECODE_HDR_SZ) Suppress "Division by 0",
              "Possible division by 0", "Constant value Boolean" in this macro */
#define CSN_DECODE_HDR_SZ (sizeof(rlc_mac_cntrl_t)/ \
                          (sizeof(rlc_mac_cntrl_t)==CSN_DECODE_HDR_EXPECTED_SZ\
                          ?1:0))
/*****************************************************************************/

typedef struct
{
  rlc_mac_cntrl_t                                *header_ptr;
  uint8                                          *data_ptr;
  void                                           *allocated_memory_ptr;
} csn_decoder_ptrs_t;

/*===========================================================================

  DATA DECLARATIONS - Encoder

===========================================================================*/

typedef struct
{
  uint32                                         tlli;
  CSN_ARFCN_TYPE                                 arfcn;
  uint8                                          bsic;
  uint8                                          cause;

  /* Rel 99 additions */
  boolean                                        rel99;

  boolean                                        fdd_flag;
  uint16                                         fdd_arfcn;
  uint8                                          fdd_diversity;
  boolean                                        fdd_bandwidth_flag;
  uint8                                          fdd_bandwidth;
  uint16                                         fdd_scrambling_code;

  boolean                                        tdd_flag;
  uint16                                         tdd_arfcn;
  uint8                                          tdd_diversity;
  boolean                                        tdd_bandwidth_flag;
  uint8                                          tdd_bandwidth;
  uint8                                          tdd_cell_param;
  uint8                                          tdd_sync_case;
} packet_cell_change_failure_t;

typedef struct
{
  uint32                                         tlli;
  uint8                                          up_ctrl_ack;
#ifdef FEATURE_GSM_EDTM
  boolean                                        rel5_additions_flag;
  boolean                                        tn_rrbp_flag;
  uint8                                          tn_rrbp;
  boolean                                        grnti_ext_flag;
  uint8                                          grnti_ext;
  boolean                                        rel6_additions_flag;
  boolean                                        ctrl_ack_ext_flag;
  uint16                                         ctrl_ack_extension;
#endif /* FEATURE_GSM_EDTM */
} packet_control_acknowledge_t;

typedef struct
{
  uint8                                          downlink_tfi;
  ack_nack_desc_t                                ack_nack_desc;
  boolean                                        chan_req_desc_flag;
  uint8                                          c_value;
  uint8                                          rxqual;
  uint8                                          sign_var;
  uint8                                          i_level_bitmap;
  uint8                                          i_level[8];
  channel_req_desc_t                             chan_req_desc;
  boolean                                        pfi_present;
  uint8                                          pfi;
} packet_downlink_ack_nack_t;

typedef struct
{
  uint32                                         tlli;
} packet_uplk_dummy_cntrl_blk_t;

typedef struct
{
  uint8                                          freq_n;
  boolean                                        bsic_n_flag;
  uint8                                          bsic_n;
  uint8                                          rxlev_n;
} num_meas_strct_t;

typedef struct
{
#define MAX_NC_MEAS                              7
  uint8                                          nc_mode;
  uint8                                          rxlev_srvg_cell;
  boolean                                        intrfrnce_srvg_cell_flag;
  uint8                                          intrfrnce_srvg_cell;
  uint8                                          num_nc_meas;
  num_meas_strct_t                               num_meas[MAX_NC_MEAS];
} enc_nc_meas_rprt_strct_t;

typedef struct
{
  uint8                                          three_g_cell_list_index;
  uint8                                          rep_quality;
} three_g_cell_list_t;

typedef struct
{
#define MAX_N3G_MEAS                             8

  uint8                                          n3g;
  three_g_cell_list_t                            three_g_cell_list[MAX_N3G_MEAS];
} three_g_measurement_report_t;

typedef struct
{
  uint32                                         tlli;
  boolean                                        psi5_chng_mrk_flag;
  uint8                                          psi5_chng_mrk;
  enc_nc_meas_rprt_strct_t                       enc_nc_meas_rprt_strct;

  /* Additions in release 99 */
  boolean                                        ba_or_psi3_and_pmo_included;
  /* set that to TRUE if either (the BA/3G BA used or the PSI3 Change Mark) AND (the PMO Used) should be included */
  ba_or_psi3_change_mark_e                       ba_or_psi3_change_mark;
  /* set that to say what is being used (either BA/3G BA or PSI3 Change Mark) */
  uint8                                          ba_used;
  uint8                                          three_g_ba_used;
  uint8                                          psi3_change_mark;
  uint8                                          pmo_used;
  boolean                                        three_g_measurement_report_flag;
  three_g_measurement_report_t                   three_g_measurement_report;
} packet_measurement_report_t;

typedef struct
{
  uint8                                          lst_index;
  uint8                                          rep_quant;
} nacc_meas_3g_t;

typedef struct
{
#define MAX_3G_MEAS                              8
  uint8                                          n_3g;
  nacc_meas_3g_t                                 meas_3g[MAX_3G_MEAS];
} enc_nacc_3g_meas_rprt_strct_t;

typedef struct
{
  boolean                                        fdd_arfcn_valid;
  uint16                                         fdd_arfcn;
  boolean                                        fdd_bandwidth_valid;
  uint8                                          fdd_bandwidth;
  uint16                                         fdd_scrambling_code;
  boolean                                        tdd_arfcn_valid;
  uint16                                         tdd_arfcn;
  boolean                                        tdd_bandwidth_valid;
  uint8                                          tdd_bandwidth;
  uint8                                          tdd_cell_param;
  uint8                                          tdd_sync_case;
  uint8                                          rep_quant;
} nacc_3g_target_cell_t;

typedef struct
{
  uint8                                          rxlev_srvg_cell;
  boolean                                        intrfrnce_srvg_cell_flag;
  uint8                                          intrfrnce_srvg_cell;
  uint8                                          num_nc_meas;
  num_meas_strct_t                               nc_meas[MAX_NC_MEAS];
} enc_nacc_nc_meas_rprt_strct_t;

typedef struct
{
  global_tfi_t                                   global_tfi;
  ARFCN_T                                        arfcn;
  uint8                                          bsic;
  boolean                                        target_cell_3g_valid;
  nacc_3g_target_cell_t                          target_cell_3g;
  ba_or_psi3_change_mark_e                       ba_or_psi3_change_mark;
  /* set that to say what is being used (either BA/3G BA or PSI3 Change Mark) */
  uint8                                          ba_used;
  uint8                                          psi3_change_mark;
  uint8                                          pmo_used;
  uint8                                          pccn_sending;
  enc_nacc_nc_meas_rprt_strct_t                  ccn_meas_rpt;
  boolean                                        ccn_meas_3g_valid;
  boolean                                        ba_used_3g_valid;
  uint8                                          ba_used_3g;
  enc_nacc_3g_meas_rprt_strct_t                  ccn_meas_3g;
} packet_cell_change_notification_t;

typedef struct
{
  global_tfi_t                                   global_tfi;
  boolean                                        data_valid;
  CSN_ARFCN_TYPE                                 arfcn;
  uint8                                          bsic;
  uint8                                          container_id;
} packet_cell_change_continue_t;

/**
 * csn_invalid_bsic_reporting_t
 *
 * Used to report cells with invalid bsic in PEMR.
 */
typedef struct
{
  uint8                                          bcch_freq_ncell;
  uint8                                          bsic;
  uint8                                          rxlev_ncell;
} csn_invalid_bsic_information_t;

/**
 * csn_serving_cell_data_t
 *
 * Used to report serving cell data in PEMR.
 */
typedef struct
{
  uint8                                          rxlev;
  boolean                                        interference_valid;
  uint8                                          interference;
} csn_serving_cell_data_t;

/**
 * csn_reporting_quantity_t
 */
typedef struct
{
  boolean                                        valid;
  boolean                                        value;
} csn_reporting_quantity_t;

typedef struct
{
#define MAX_INVALID_BSIC_INFORMATION             96
#define MAX_REPORTING_QUANTITY                   96

  uint32                                         tlli;
  uint8                                          nc_mode;
  boolean                                        ba_used_and_3g_ba_used_valid;
  /* TRUE if BA_USED and BA_3G_USED are TRUE, FALSE if PSI3_CHANGE_MARK is TRUE */
  uint8                                          ba_used;
  uint8                                          ba_3g_used;
  uint8                                          psi3_change_mark;
  uint8                                          pmo_used;
    /* value of PMO_IND in PCCO or PMO or 0 if none received  */
  boolean                                        bsic_seen;
    /* TRUE if a GSM cell with invalid BSIC and allowed NCC part of BSIC is one of the six strongest, see 3GPP TS 05.08 */
  uint8                                          scale;

  boolean                                        serving_cell_data_valid;
  csn_serving_cell_data_t                        serving_cell_data;

  uint8                                          invalid_bsic_information_count;
  csn_invalid_bsic_information_t                 invalid_bsic_information[MAX_INVALID_BSIC_INFORMATION];

  boolean                                        reporting_quantity_valid;
  csn_reporting_quantity_t                       reporting_quantity[MAX_REPORTING_QUANTITY];
} packet_enhanced_measurement_report_t;

#define MAX_MS_RA_CAP_IE_LEN                     51 /* 51 octets max length */

typedef struct
{
  uint16                                         ms_cap_len_bits;
  uint8                                          ms_cap[MAX_MS_RA_CAP_IE_LEN];
} ms_cap_t;

typedef struct
{
  /*< EGPRS BEP Link Quality Measurements */
  boolean                                        gmsk_mean_bep_present;
  uint8                                          gmsk_m_bep;          /*5 bits*/
  uint8                                          gmsk_cv_bep;         /*3 bits*/
  boolean                                        psk_mean_bep_present;
  uint8                                          psk_m_bep;           /*5 bits*/
  uint8                                          psk_cv_bep;          /*3 bits*/

  /*< C_VALUE : bit (6) > */
  uint8                                          c_value;

  /*< EGPRS Timeslot Link Quality Measurements*/
  boolean                                        bep_meas_rpt_flg;

  uint8                                          tn_bitmap;           /*1 = tn present,  0 = tn not present*/
  uint8                                          gmsk_or_8psk_bitmap; /*1 = 8psk bep,    0 = gmak bep*/
  uint8                                          mean_bep[8];

  /*{ 0 | 1 < INTERFERENCE_MEASUREMENTS : Interference Measurement Report Struct >}; */
  boolean                                        int_meas_rpt_flg;
  uint8                                          i_level_bitmap;      /*1 = tn present, 0 tn not  present*/
  uint8                                          i_level[8];
} egprs_chn_qual_rpt_t;

typedef struct
{
  uint8 dummy;
} extension_bits;

typedef struct
{
  uint8                                          downlink_tfi;
  uint8                                          ms_out_of_mem;
  boolean                                        egprs_chn_qual_rpt_flag;
  egprs_chn_qual_rpt_t                           egprs_chn_qual_rpt;
  boolean                                        chan_req_desc_flag;
  channel_req_desc_t                             chan_req_desc;
  boolean                                        pfi_flag;
  uint8                                          pfi;
  boolean                                        extension_bits_flag;
  extension_bits_t                               extension_bits;
  egprs_ack_nack_des_t                           egprs_ack_nack_des;
} egprs_packet_downlink_ack_nack_t;

typedef struct
{
  /*  Defined as 00100100 */
#define MS_ACC_CAP                               0x24
#define MAX_ACC_CAP_SIZE                         15
  boolean                                        access_type_flag;
  uint8                                          access_type;
  enum
  {
    CSN_USE_GLOBAL_TFI,
    CSN_USE_TLLI
  } tfi_tlli_flag;
  global_tfi_t                                   global_tfi;
  uint32                                         tlli;
  boolean                                        ms_radio_access_cap_flag;
  ms_cap_t                                       access_capability;
  channel_req_desc_t                             chan_req_desc;
  boolean                                        change_mark_flag;
  uint8                                          change_mark;
  uint8                                          c_value;
  boolean                                        sign_var_flag;
  uint8                                          sign_var;
  uint8                                          i_level_tn_bitmap;
  uint8                                          i_level_tn[8];

  /*EGPRS additions to PRR*/
  boolean                                        include_egprs;
  boolean                                        egprs_bep_link_qual_present;
  boolean                                        egprs_ts_link_qual_present;
  boolean                                        pfi_present;
  uint8                                          pfi;
  boolean                                        additional_rac_info;
  boolean                                        rac_retran;
  egprs_chn_qual_rpt_t                           egprs_qual_rpt;
} packet_resource_request_t;

typedef struct
{
  global_tfi_t                                   global_tfi;
  uint8                                          tbf_cause;
  boolean                                        status_mess_type_flag;
  uint8                                          status_mess_type;
} packet_mobile_tbf_status_t;

typedef struct
{
  uint8                                          msg_type;
  uint8                                          mess_rec;
  uint8                                          six_chng_mrk;
  uint8                                          six_count;
  uint16                                         instance_bitmap;
} si_msg_list_t;

typedef struct
{
#define MAX_SI_MSG                               10
  uint8                                          num_msg;
  si_msg_list_t                                  si_msg_list[MAX_SI_MSG];
  uint8                                          additional_msg_type;
} rx_si_msg_struct_t;

typedef struct
{
#define MAX_UNKWN_SI_MSG                         10
  uint8                                          num_msg;
  uint8                                          msg_type[MAX_UNKWN_SI_MSG];
  uint8                                          additional_msg_type;
} rx_unknw_si_msg_struct_t;

typedef struct
{
  global_tfi_t                                   global_tfi;
  uint8                                          bcch_chng_mrk;
  rx_si_msg_struct_t                             rx_si_msg_struct;
  rx_unknw_si_msg_struct_t                       rx_unknw_si_msg_struct;
  boolean                                        rel6_additions_valid;
  boolean                                        ps_rel_req;
} packet_si_status_t;

typedef enum
{
  one_phase,
  short_acc,
  two_phase,
  page_response,
  cell_update,
  mm_proc,
  single_block,
  signalling
} access_type_e;

typedef struct
{
  boolean                                        tlli_valid_flg;
  global_tfi_t                                   global_tfi;
  uint32                                         tlli;
  ms_cap_t                                       access_capability;
} egprs_pkt_add_rac_info_t;

typedef struct
{
#define ONE_PHASE_FIXED_BITS_8                   0x01
#define ONE_PHASE_FIXED_BITS_11                  0x00
#define SHORT_ACC_FIXED_BITS_8                   0x00
#define SHORT_ACC_FIXED_BITS_11                  0x04
#define TWO_PHASE_FIXED_BITS_8                   0x08
#define TWO_PHASE_FIXED_BITS_11                  0x30
#define PAGE_RESP_FIXED_BITS_8                   0x09
#define PAGE_RESP_FIXED_BITS_11                  0x31
#define CELL_UPDATE_FIXED_BITS_8                 0x0a
#define CELL_UPDATE_FIXED_BITS_11                0x32
#define MM_PROC_FIXED_BITS_8                     0x0b
#define MM_PROC_FIXED_BITS_11                    0x33
#define SINGLE_BLOCK_FIXED_BITS_8                0x0c
#define SINGLE_BLOCK_FIXED_BITS_11               0x34
  boolean                                        access_mode;
  access_type_e                                  access_type;
  uint8                                          multislot_class;
  uint8                                          priority;
  uint8                                          random_bits;
  uint8                                          no_of_blocks;
} packet_channel_request_t;

typedef struct
{
  global_tfi_t                                   global_tfi;
  uint8                                          establishment_cause;
} packet_cs_request_t;

typedef union
{
  packet_cell_change_failure_t                   pkt_cell_chg_failure;
  packet_control_acknowledge_t                   pkt_ctrl_ack;
  packet_downlink_ack_nack_t                     pkt_dwnlk_ack_nack;
  packet_uplk_dummy_cntrl_blk_t                  pkt_uplk_dummy_cntrl_blk;
  packet_measurement_report_t                    pkt_meas_report;
  packet_resource_request_t                      pkt_resource_req;
  packet_mobile_tbf_status_t                     pkt_mobile_tbf_status;
  packet_channel_request_t                       pkt_channel_request;
  packet_enhanced_measurement_report_t           pkt_enhanced_measurement_report;
  packet_si_status_t                             pkt_si_status;
  egprs_packet_downlink_ack_nack_t               egprs_pkt_dwnlk_ack_nack;
  egprs_pkt_add_rac_info_t                       egprs_pkt_add_rac_info;
  packet_cell_change_notification_t              pkt_cell_change_notification;
  packet_cs_request_t                            pkt_cs_request;
} enc_type_t;

#define PASS                                     0
#define FAIL                                     1
#define UPLINK_MESSAGE_SIZE                      22

typedef struct
{
  /* The format of the structure will depend on the message type. */
  enc_type_t                                     type;
} int_data_t;


/********************************************************************************
 *
 *  Data declarations for the RR CSN1 Decoder
 *
 ********************************************************************************/

typedef struct
{
  boolean                                        nch_position_flag;
  uint8                                          nch_position;
  boolean                                        band_indicator;
} si1_rest_octets_t;

typedef struct
{
  boolean                                        selection_params_flag;
  uint8                                          cbq;
  uint8                                          cell_reselect_offset;
  uint8                                          temporary_offset;
  uint8                                          penalty_time;
  boolean                                        power_offset_flag;
  uint8                                          power_offset;
  uint8                                          si2ter_indicator;
  uint8                                          ecsc;
  boolean                                        siaw_flag;
  uint8                                          where;
  boolean                                        gprs_indicator_flag;
  uint8                                          ra_colour;
  uint8                                          si13_position;
  uint8                                          three_g_ecsr;
  boolean                                        si2quater_indicator_flag;
  uint8                                          si2quater_position;
} si3_rest_octets_t;

/* SI6 rest octets */

typedef struct
{
  boolean                                        pch_and_nch_flag;
  uint8                                          page_ch_restruct;
  uint8                                          nln_sacch;
  boolean                                        call_priority_flag;
  uint8                                          call_priority;
  uint8                                          nln_status;
  boolean                                        vbs_vgcs_flag;
  uint8                                          vbs_vgcs_inband_notif;
  uint8                                          vbs_vgcs_inband_pagings;
  uint8                                          dtm_supp;
  uint8                                          rac;
  uint8                                          max_lapdm;
  uint8                                          band_ind;
  boolean                                        gprs_ms_tmc_flag;
  uint8                                          gprs_ms_tmc;
} si6_rest_octets_t;

typedef struct
{
  boolean                                        short_lsa_id_flag;
  enum
  {
    plmn_significant_number                      = 0,
    universal_lsa                                = 1
  } lsa_id_type;
  uint32                                         lsa_id;
  uint16                                         short_lsa_id;
} lsa_identity_t;

typedef struct
{
#define MAX_LSA_ID_INFO                          7

/* SI 4 Rest Octets_O */
  boolean                                        selection_params_flag;
  uint8                                          cbq;
  uint8                                          cell_reselect_offset;
  uint8                                          temporary_offset;
  uint8                                          penalty_time;
  boolean                                        power_offset_flag;
  uint8                                          power_offset;
  boolean                                        gprs_indicator_flag;
  uint8                                          ra_colour;
  uint8                                          si13_position;
/* SI4 Rest Octets_S */
  boolean                                        si4_rest_octets_s_flag;
  boolean                                        lsa_parameters_flag;
  uint8                                          prio_thr;
  uint8                                          lsa_offset;
  boolean                                        mcc_mnc_flag;
  uint16                                         mcc;
  uint16                                         mnc;
  boolean                                        cell_identity_flag;
  uint16                                         cell_identity;
  uint8                                          lsa_id_info_count;
  lsa_identity_t                                 lsa_id_info[MAX_LSA_ID_INFO];
} si4_rest_octets_t;

struct si7_8_rest_octets_t
{
  boolean                                        si4_acs_flag;
/* SI 4 Rest Octets_O */
  boolean                                        selection_params_flag;
  uint8                                          cbq;
  uint8                                          cell_reselect_offset;
  uint8                                          temporary_offset;
  uint8                                          penalty_time;
  boolean                                        power_offset_flag;
  uint8                                          power_offset;
  boolean                                        gprs_indicator_flag;
  uint8                                          ra_colour;
  uint8                                          si13_position;
/* SI4 Rest Octets_S */
  boolean                                        si4_rest_octets_s_flag;
  boolean                                        lsa_parameters_flag;
  uint8                                          prio_thr;
  uint8                                          lsa_offset;
  boolean                                        mcc_mnc_flag;
  uint16                                         mcc;
  uint16                                         mnc;
  boolean                                        cell_identity_flag;
  uint16                                         cell_identity;
  uint8                                          lsa_id_info_count;
  lsa_identity_t                                 lsa_id_info[MAX_LSA_ID_INFO];
};

typedef struct si7_8_rest_octets_t si7_rest_octets_t;

typedef struct si7_8_rest_octets_t si8_rest_octets_t;

typedef struct
{
  boolean                                        si13_info_included;
  uint8                                          bcch_chng_mrk;
  uint8                                          si_chng_field;
  uint8                                          si13_change_mark;
  boolean                                        si13_cm_gprs_ma_flag;
  gprs_mobile_alloc_t                            gprs_mobile_alloc;
  uint8                                          rac;
  uint8                                          spgc_ccch_sup;
  uint8                                          priority_access_thr;
  uint8                                          network_ctrl_ordr;
  gprs_cell_options_t                            gprs_cell_options;
  uint8                                          alpha;
  uint8                                          t_avg_w;
  uint8                                          t_avg_t;
  uint8                                          pc_meas_chan;
  uint8                                          n_avg_i;
  boolean                                        pbcch_present_flag;
  uint8                                          psi1_repeat_period;
  uint8                                          pb;
  uint8                                          tsc;
  uint8                                          tn;
  CSN_ARFCN_TYPE                                 arfcn;
  uint8                                          maio;
  csn_bcch_arfcn_maio_t                          bcch_arfcn_maio_flag;
  boolean                                        sgsnr_flag;
  uint8                                          sgsnr;
  boolean                                        r4_additions_flag;
  uint8                                          si_status_ind;
  boolean                                        r6_additions_flag;
  uint8                                          lb_ms_txpwr_max_cch;
  uint8                                          si2n_support;
} si13_rest_octets_t;

typedef struct
{
  uint8                                          timeslot_num;
  uint8                                          alpha;
  boolean                                        alpha_gamma_flag;
  uint8                                          gamma_tn;
  boolean                                        dwnlk_pwr_ctrl_flag;
  dwnlk_pwr_ctrl_t                               dwnlk_pwr_ctrl;
} rr_pua_single_block_alloc_t;

typedef struct
{
  uint8                                          ext_dynamic_alloc;
  boolean                                        po_prmode_flag;
  uint8                                          po;
  uint8                                          pr_mode;
  uint8                                          usf_granularity;
  boolean                                        uplk_tfi_assignment_flag;
  uint8                                          uplk_tfi_assignment;
  boolean                                        rlc_data_blks_granted_flag;
  uint8                                          rlc_data_blks_granted;
  timeslot_alloc_flag_e                          timeslot_alloc_flag;
  uint8                                          usf_tn[8];
  uint8                                          usf_bitmap;
  uint8                                          alpha;
  uint8                                          gamma_tn[8];
  uint8                                          gamma_bitmap;
} rr_pua_dynamic_alloc_t;

/*
The spec (04.18, V8.10.0) seems to imply that the Allocation Bitmap can
be of arbitrary length (i.e. greater than 256 bits) but that networks should
be able to handle mobiles that only support a maximum Allocation Bitmap
length of 256 bits.
*/
typedef struct
{
/*  boolean message_escape; */
  uint8                                          block_periods;
  uint16                                         alloc_bitmap_length;
  uint8                                          alloc_bitmap[32];
} rr_allocation_bitmap_t;

typedef struct
{
#define MESSAGE_ESCAPE                           2

  boolean                                        uplink_tfi_assignment_flag;
  uint8                                          uplink_tfi_assignment;
  uint8                                          final_allocation;
  uint8                                          downlink_control_timeslot;
  boolean                                        dwnlk_pwr_ctrl_flag;
  dwnlk_pwr_ctrl_t                               dwnlk_pwr_ctrl;
  ts_alloc_pwr_flag_e                            ts_alloc_pwr_ctrl_flag;
  uint8                                          ts_alloc;
  pwr_ctrl_paramtrs_t                            pwr_ctrl_paramtrs;
  uint8                                          half_duplex_mode;
  rr_allocation_bitmap_t                         allocation_bitmap;
} rr_pua_fixed_alloc_t;

typedef struct
{
  uint8                                          nothing;
} rr_pua_extension_t;

/*
RR Packet Uplink Assignment IE
*/
typedef struct
{
  uint8                                          length_in_octets;
  uint8                                          channel_coding_command;
  uint8                                          tlli_block_channel_coding;
  pkt_timing_adv_t                               pkt_timing_adv;
  uint8                                          allocation_type;
  union
  {
    rr_pua_dynamic_alloc_t                       dynamic_alloc;
    rr_pua_single_block_alloc_t                  single_block_alloc;
    rr_pua_fixed_alloc_t                         fixed_alloc;
    rr_pua_extension_t                           extension;
  } allocation;
  boolean                                        r99_additions_flag;
  boolean                                        egprs_flag;
  uint8                                          egprs_mcs_mode;
  uint8                                          resegment;
  uint8                                          epgrs_win_sz;
  boolean                                        packet_ext_timing_adv_flag;
  uint8                                          packet_ext_timing_adv;
} rr_packet_uplink_assignment_t;

typedef struct
{
  uint8                                          length_in_octets;
  uint8                                          mac_mode;
  uint8                                          rlc_mode;
  uint8                                          timeslot_allocation;
  pkt_timing_adv_t                               packet_timing_advance;
  boolean                                        dwnlk_pwr_ctrl_flag;
  dwnlk_pwr_ctrl_t                               dwnlk_pwr_ctrl;
  boolean                                        pwr_ctrl_paramtrs_flag;
  pwr_ctrl_paramtrs_t                            pwr_ctrl_paramtrs;
  boolean                                        downlink_tfi_assignment_flag;
  uint8                                          downlink_tfi_assignment;
  boolean                                        measurement_flag;
  uint16                                         measurement_starting_time;
  uint8                                          measurement_interval;
  uint8                                          measurement_bitmap;
  boolean                                        egprs_tbf_mode_flag;
  uint8                                          egprs_win_sz;
  uint8                                          link_quality_measurement_mode;
  boolean                                        packet_ext_timing_adv_flag;
  uint8                                          packet_ext_timing_adv;
} rr_packet_downlink_assignment_t;

typedef enum
{
  ma                                             = 0,
  fsl                                            = 1
} ma_or_fsl_e;

typedef struct
{
#define MAX_MA_LENGTH                            255

  uint8                                          chan_type_tdma_offset;
  uint8                                          tn;
  uint8                                          tsc;
  boolean                                        hopping_flag;
  uint8                                          maio;
  uint8                                          hsn;
  CSN_ARFCN_TYPE                                 arfcn;
} chan_desc_t;

typedef struct
{
  chan_desc_t                                    chan_desc;
  boolean                                        hopping_flag;
  ma_or_fsl_e                                    ma_or_fsl;
  uint8                                          fsl[FSL_LENGTH_BYTES];
  uint8                                          ma_length;
  uint8                                          ma[MAX_MA_LENGTH];
} group_chan_desc_t;

/* newer compilers use a packed format */
typedef PACKED struct PACKED_POST
{
  uint64                                         group_call_ref;
  boolean                                        group_chan_desc_flag;
  group_chan_desc_t                              group_chan_desc;
} group_call_info_t;


typedef enum { rr_paging = 0, packet_paging = 1 } packet_page_ind_e;

typedef struct
{
#define MAX_P1_REST_OCTETS_LEN                   11

  boolean                                        nln_flag;
  uint8                                          nln_pch;
  uint8                                          nln_status;
  boolean                                        priority_1_flag;
  uint8                                          priority_1;
  boolean                                        priority_2_flag;
  uint8                                          priority_2;
  boolean                                        group_call_info_flag;
  group_call_info_t                              group_call_info;
  packet_page_ind_e                              packet_page_ind_1;
  packet_page_ind_e                              packet_page_ind_2;
} p1_rest_octets_t;

typedef struct
{
#define MAX_P2_REST_OCTETS_LEN                   11
#define CSN_NLN_STATUS                           1

  boolean                                        cn3_flag;
  uint8                                          cn3;
  boolean                                        nln_flag;
  uint8                                          nln;
  uint8                                          nln_status;
  boolean                                        priority_1_flag;
  uint8                                          priority_1;
  boolean                                        priority_2_flag;
  uint8                                          priority_2;
  boolean                                        priority_3_flag;
  uint8                                          priority_3;
  packet_page_ind_e                              packet_page_ind_3;
} p2_rest_octets_t;

typedef struct
{
#define MAX_P3_REST_OCTETS_LEN                   3
  boolean                                        cn3_cn4_flag;
  uint8                                          cn3;
  uint8                                          cn4;
  boolean                                        nln_flag;
  uint8                                          nln;
  uint8                                          nln_status;
  boolean                                        priority_1_flag;
  uint8                                          priority_1;
  boolean                                        priority_2_flag;
  uint8                                          priority_2;
  boolean                                        priority_3_flag;
  uint8                                          priority_3;
  boolean                                        priority_4_flag;
  uint8                                          priority_4;
} p3_rest_octets_t;

typedef enum
{
  CONFIG_NON_HOPPING,
  CONFIG_INDIRECT_ENCODING,
  CONFIG_DIRECT_ENCODING
} rf_channel_config_e;

typedef struct
{
#define MAX_PACKET_CHANNEL_DESCRIPTION_LEN       4

  uint8                                          channel_type;
  uint8                                          tn;
  uint8                                          tsc;
  rf_channel_config_e                            rf_channel_config;
  CSN_ARFCN_TYPE                                 arfcn;
  uint8                                          maio;
  uint8                                          ma_number_ind;
  boolean                                        change_mark_1_flag;
  uint8                                          change_mark_1;
  uint8                                          hsn;
} packet_channel_description_t;

/*
IA REST OCTETS
*/

typedef struct
{
  uint8                                          usf;
  uint8                                          usf_granularity;
  boolean                                        po_pr_mode_flag;
  uint8                                          po;
  uint8                                          pr_mode;
} ia_rest_octets_dynamic_alloc_t;

typedef struct
{
  uint8                                          alloc_bitmap_length;
  uint32                                         alloc_bitmap;
  boolean                                        dwnlk_pwr_ctrl_flag;
  dwnlk_pwr_ctrl_t                               dwnlk_pwr_ctrl;
} ia_rest_octets_fixed_alloc_t;

typedef struct
{
  uint8                                          tfi_assignment;
  uint8                                          polling;
  alloc_flag_e                                   alloc_flag;
  union
  {
    ia_rest_octets_dynamic_alloc_t               dynamic_alloc;
    ia_rest_octets_fixed_alloc_t                 fixed_alloc;
  } alloc;
  uint8                                          channel_coding_command;
  uint8                                          tlli_block_channel_coding;
  boolean                                        alpha_flag;
  uint8                                          alpha;
  uint8                                          gamma;
  boolean                                        timing_advance_index_flag;
  uint8                                          timing_advance_index;
  boolean                                        tbf_starting_time_flag;
  uint16                                         tbf_starting_time;
} ia_multi_block_alloc_t;

typedef struct
{
  boolean                                        alpha_flag;
  uint8                                          alpha;
  uint8                                          gamma;
  uint16                                         tbf_starting_time;
  boolean                                        dwnlk_pwr_ctrl_flag;
  dwnlk_pwr_ctrl_t                               dwnlk_pwr_ctrl;
} ia_single_block_alloc_t;

typedef enum
{
  BLOCK_ALLOC_FLAG_SINGLE,
  BLOCK_ALLOC_FLAG_MULTI
} block_alloc_flag_e;

typedef struct
{
#define MAX_IA_RO_LENGTH_BITS                    (11*8) /* this length given in bits  */

  block_alloc_flag_e                             block_alloc_flag;
  union
  {
    ia_multi_block_alloc_t                       multi_block_alloc;
    ia_single_block_alloc_t                      single_block_alloc;
  } block_alloc;
  boolean                                        extended_ra_flag;
  uint8                                          extended_ra;
} ia_rest_octets_packet_uplink_ass_t;

typedef struct
{
  uint8                                          tfi_assignment;
  uint8                                          polling;
  alloc_flag_e                                   alloc_flag;
  union
  {
    ia_rest_octets_dynamic_alloc_t               dynamic_alloc;
    ia_rest_octets_fixed_alloc_t                 fixed_alloc;
  } alloc;
  uint8                                          egprs_channel_coding_command;
  uint8                                          tlli_block_channel_coding;
  boolean                                        bep_period2_flag;
  uint8                                          bep_period2;
  uint8                                          resegment;
  uint8                                          egprs_window_size;
  boolean                                        alpha_flag;
  uint8                                          alpha;
  uint8                                          gamma;
  boolean                                        timing_advance_index_flag;
  uint8                                          timing_advance_index;
  boolean                                        tbf_starting_time_flag;
  uint16                                         tbf_starting_time;

} ia_ro_egprs_pua_single_block_alloc_t;

typedef struct {
  uint8                                          num_of_radio_blocks_allocated;
  boolean                                        dwnlk_pwr_ctrl_flag;
  dwnlk_pwr_ctrl_t                               dwnlk_pwr_ctrl;
  boolean                                        alpha_flag;
  uint8                                          alpha;
  uint8                                          gamma;
  uint16                                         tbf_starting_time;
} ia_ro_egprs_pua_multi_block_alloc_t;

typedef struct
{
  uint8                                          extended_ra;
  uint8                                          access_tech_type_count;
  uint8                                          access_tech_type[MAX_ACCESS_TECH_TYPE];
  block_alloc_flag_e                             block_alloc_flag;
  union
  {
    ia_ro_egprs_pua_single_block_alloc_t         single_block_alloc;
    ia_ro_egprs_pua_multi_block_alloc_t          multi_block_alloc;
  } block_alloc;
  boolean                                        alpha_flag;
  uint8                                          alpha;
  uint8                                          gamma;
  uint16                                         tbf_starting_time;
} ia_rest_octets_egprs_packet_uplink_ass_t;

typedef struct
{
  uint32                                         tlli;
  boolean                                        tfi_assignment_flag;
  uint8                                          tfi_assignment;
  uint8                                          rlc_mode;
  boolean                                        alpha_flag;
  uint8                                          alpha;
  uint8                                          gamma;
  uint8                                          polling;
  uint8                                          ta_valid;
  boolean                                        timing_advance_index_flag;
  uint8                                          timing_advance_index;
  boolean                                        tbf_starting_time_flag;
  uint16                                         tbf_starting_time;
  boolean                                        dwnlk_pwr_ctrl_flag;
  dwnlk_pwr_ctrl_t                               dwnlk_pwr_ctrl;
  boolean                                        egprs_tbf_mode_flag;
  uint8                                          egprs_window_size;
  uint8                                          link_quality_measurement_mode;
  boolean                                        bep_period2_flag;
  uint8                                          bep_period2;
} ia_rest_octets_packet_downlink_ass_t;

typedef struct
{
#define MAX_MOBILE_ALLOCATION_BYTES              62

  uint8                                          maio;
  uint8                                          mobile_allocation_length;
  uint8                                          mobile_allocation[MAX_MOBILE_ALLOCATION_BYTES];
  boolean                                        compressed_irat_ho_info;
} ia_rest_octets_freq_params_before_time_t;

typedef struct
{
  boolean                                        additions_for_r99;
  boolean                                        extended_ra_flag;
  uint8                                          extended_ra;
} ia_rest_octets_second_part_packet_ass_t;

typedef struct
{
  enum
  {
    egprs_packet_uplink_ass,
    freq_params_before_time,
    packet_uplink_ass,
    packet_downlink_ass,
    second_part_packet_ass,
    ia_rest_octets_unknown,
    compressed_interRAT_ho_info
  } contents_type;
  union
  {
    boolean                                      compressed_irat_ho_info;
    ia_rest_octets_egprs_packet_uplink_ass_t     egprs_packet_uplink_ass;
    ia_rest_octets_freq_params_before_time_t     freq_params_before_time;
    ia_rest_octets_packet_uplink_ass_t           packet_uplink_ass;
    ia_rest_octets_packet_downlink_ass_t         packet_downlink_ass;
    ia_rest_octets_second_part_packet_ass_t      second_part_packet_ass;
  } contents;
  uint8                                          length_of_freq_params;
} ia_rest_octets_t;

typedef struct
{
  boolean                                        ext_ra_present;
  uint8                                          ext_ra;
} iar_ext_ra_t;

typedef struct
{
#define NUM_OF_EXT_RA                            4
  iar_ext_ra_t                                   ext_ra[NUM_OF_EXT_RA];
} iar_rest_octets_t;

/*
Mobile Station Classmark 3
*/

typedef struct
{
  uint8                                          a5_7;
  uint8                                          a5_6;
  uint8                                          a5_5;
  uint8                                          a5_4;
} a5_bits_t;

typedef struct
{
  uint8                                          sms_value;
  uint8                                          sm_value;
} ms_measurement_cap_t;

typedef struct
{
  uint8                                          modulation_cap;
  boolean                                        edge_rf_power_cap_1_flag;
  uint8                                          edge_rf_power_cap_1;
  boolean                                        edge_rf_power_cap_2_flag;
  uint8                                          edge_rf_power_cap_2;
} edge_struct_t;

typedef struct
{
#define MAX_MOBILE_STATION_CLASSMARK_3_LEN       14
#define CLASS_P_GSM                              (1 << 0)
#define CLASS_E_GSM_OR_R_GSM                     (1 << 1)
#define CLASS_DCS_1800                           (1 << 2)
#define CSN_ASSOCIATED_RADIO_CAP_1               4
#define CSN_ASSOCIATED_RADIO_CAP_2               4

  uint8                                          multiband_supported;
  a5_bits_t                                      a5_bits;
  uint8                                          associated_radio_cap_1;
  uint8                                          associated_radio_cap_2;
  boolean                                        r_support_flag;
  uint8                                          r_support;
  boolean                                        multi_slot_cap_flag;
  uint8                                          multi_slot_cap;
  uint8                                          ucs2_treatment;
  uint8                                          extended_measurement_cap;

  boolean                                        ms_measurement_cap_flag;
  ms_measurement_cap_t                           ms_measurement_cap;

  boolean                                        ms_positioning_method_cap_flag;
  uint8                                          ms_positioning_method;

  boolean                                        edge_multi_slot_cap_flag;
  uint8                                          edge_multi_slot_class;

  boolean                                        edge_struct_flag;
  edge_struct_t                                  edge_struct;

  boolean                                        gsm400_bands_supported_flag;
  uint8                                          gsm400_bands_supported;
  uint8                                          gsm400_associated_radio_cap;

  boolean                                        gsm850_associated_radio_cap_flag;
  uint8                                          gsm850_associated_radio_cap;

  boolean                                        pcs1900_associated_radio_cap_flag;
  uint8                                          pcs1900_associated_radio_cap;

  uint8                                          umts_fdd_radio_access_tech_cap;
  uint8                                          umts_tdd_radio_access_tech_cap;
  uint8                                          cdma2000_radio_access_tech_cap;

  boolean                                        dtm_gprs_multi_slot_sub_class_flag;
  uint8                                          dtm_gprs_multi_slot_sub_class;
  uint8                                          mac_mode_support;
  boolean                                        dtm_egprs_multi_slot_sub_class_flag;
  uint8                                          dtm_egprs_multi_slot_sub_class;
  boolean                                        single_band_support_flag;
  uint8                                          gsm_band;
} mobile_station_classmark_3_t;

/* SI9 Rest Octets (and friends) */
typedef struct
{
  uint8                                          modulus;
  uint16                                         relative_position;
  uint8                                          bcch_type;
} position_t;

typedef struct
{
#define MAX_POSITIONS                            22

  enum
  {
    info_type_4                                  = 0,
    info_type_5                                  = 2,
    info_type_6                                  = 6
  } info_type_flag;
  uint8                                          info_type;
  uint8                                          position_count;
  position_t                                     positions[MAX_POSITIONS];
} scheduling_info_t;

typedef struct
{
#define MAX_SCHEDULING_INFO                      14
  boolean                                        scheduling_info_flag;
  uint8                                          scheduling_info_count;
  scheduling_info_t                              scheduling_info[MAX_SCHEDULING_INFO];
} si9_rest_octets_t;

typedef struct
{
  gprs_cell_options_t                            gprs_cell_options;
  gprs_power_control_params_t                    gprs_power_control_params;
} gprs_broadcast_information_t;

typedef struct
{
  uint8                                          max_LAPDm;
  uint8                                          gprs_ms_txpwr_max_cch;
  uint16                                         cell_identity;
} dtm_information_details_t;

/*
End of data declarations for the RR Decoder
*/

#endif /* CSN_APT_H */

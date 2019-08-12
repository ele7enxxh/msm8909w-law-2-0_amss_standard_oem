/*****************************************************************************
***
*** TITLE
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_defs.h#1 $
***
*** DESCRIPTION
***
*** This module is the header file for common gprs definitions. This covers
*** typedef's, #defines, and variables global to RR/GRR.
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/

#ifndef RR_GPRS_DEFS_H
#define RR_GPRS_DEFS_H

#include "geran_variation.h"
#include "rr_defs.h"
#include "rr_public_store.h"
#include "rr_l2.h"
#include "mac_grr.h"
#include "mm_rr.h"
#include "string.h"  /*for all the memsets in GRR*/
#include "sys_stru.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "rr_sys_info_3g.h"
#include "ms_timer.h"
#include "gmacgrrsig.h"

/***********************************
  #defines global to RR
***********************************/

/* Macros to set and clear a particular bit e.g. SET_BIT_NUM(var,2) => var=0000 0100 b */
#define SET_BIT_NUM(v,b)      SET_BIT(1<<(b),v)
#define CLR_BIT_NUM(v,b)      CLEAR_BIT(1U<<(b),(v))

/* Returns TRUE/FALSE as to if all bits in mask 'm' are set in variable 'v' */
#define ARE_ALL_BITS_SET(v,m)     ( ((v) & (m)) == (m) )

/* Returns TRUE if any bits set in mask 'm' are set in variable 'v'; FALSE otherwise */
#define ARE_ANY_BITS_SET(v,m)     ( ((v) & (m)) != 0 )

/* Returns TRUE if none of the bits set in mask 'm' are set in variable 'v' */
#define ARE_NO_BITS_SET(v,m)      ( ((v) & (m)) == 0 )

/* Returns TRUE is the bit number 'b' is set in 'v'; FALSE otherwise */
#define IS_BIT_NUM_SET(v,b)       ARE_ANY_BITS_SET(v,1<<(b))

/* Macros to convert between an ARFCN value and an ARFCN structure */
/* Use: CONVERT_TO_ARFCN( arfcn_struct, arfcn_val ); */
#define CONVERT_TO_ARFCN_T(s,v)  {s = v;}
/* Use: arfcn_val = CONVERT_FROM_ARFCN( arfcn_struct ); */
#define CONVERT_FROM_ARFCN_T(s) (s.num)

/*Macro to covert a RXlev to a dB value*/
#define CONVERT_TO_RXLEVdB(x) (x - 110)

/*MACRO to convert a BSIC_T to a 6 bit value*/
#define CONVERT_BSIC_TO_BYTE(bsic) \
(bsic.BS_colour_code + ((byte)(bsic.PLMN_colour_code<<3)))

/* Macros to convert between a BSIC value and a BSIC_T structure */
/* Use: CONVERT_TO_BSIC_T( bsic_struct, bsic_val ); */
#define CONVERT_TO_BSIC_T(s,v)  { s.BS_colour_code = v & 0x07; s.PLMN_colour_code = ( v >> 3 ) & 0x07; }
/* Use: bsic_val = CONVERT_FROM_BSIC_T( bsic_struct ); */
#define CONVERT_FROM_BSIC_T(s)  ( (uint8) ( ( s.PLMN_colour_code << 3 ) | s.BS_colour_code ) )

/* Extract the NCC from a pointer to the SCH data block */
#define DECODE_NCC(x)  (byte)((((x->SCH_block[0] & 0xfc) >> 2) & 0x38) >> 3)

/* Extract the BCC from a pointer to the SCH data block */
#define DECODE_BCC(x)  (byte)(((x->SCH_block[0] & 0xfc) >> 2) & 0x07)

/* Compare two BSICs and return FALSE if different; TRUE is the same */
#define SAME_BSIC(bsic1, bsic2)  ((bsic1.BS_colour_code == bsic2.BS_colour_code) \
                                  && (bsic1.PLMN_colour_code == bsic2.PLMN_colour_code))

#define NC_MEAS_NC0    0   /* 00b  See GSM 04.60 11.2.23 Packet Sys Info 5 */
#define NC_MEAS_NC1    1   /* 01b */
#define NC_MEAS_NC2    2   /* 10b */
#define NC_MEAS_RESET  3   /* 11b */

/* Default values for NC measurement parameters - see GSM 04.60 11.2.23 Packet Sys Info 5 */
#define DEFAULT_NC_REPORTING_PERIOD_T  3
#define DEFAULT_NC_REPORTING_PERIOD_I  7
#define DEFAULT_NON_DRX_PERIOD         2   /* Not used by GRR - sent to MAC with meas report */
#define MAX_NC_REPORTING_PERIOD_INDEX  8   /* indicates this parameter has not been received */
#define INITIAL_NC_REPORTING_PERIOD_T  4   /* used for T3158 in NC2 if SI2quater unavailable */

#define DEFAULT_FDD_RSCPMIN            6
#define MIN_FDD_RSCPMIN_DBM           -114
#define DEFAULT_FDD_QMIN_OFFSET       0

/* Default value for FDD_REPORTING_THRESHOLD_2, to be used if this parameter
is not sent by the network. */
#define DEFAULT_FDD_REPORTING_THRESHOLD_2 0

/* Converts a TN (timeslot number, 0-7) to the format of a TIMESLOT_ALLOCTION (8 bits) */
/* A TIMESLOT_ALLOCATION has bit 7 set for timeslot number 0, to bit 0 for timeslot 7 */
#define CONVERT_TIMESLOT_NUM_TO_TIMESLOT_ALLOC( tsn )  ( (byte) ( 0x80 >> tsn ) )

/* For storage of mobile allocations */
#define MA_NUMBER_14  14
#define MA_NUMBER_15  15
#define MAP_MA_NUMBER_TO_OFFSET(x)  (x-14)  // MA_NUMBER={14,15} stored as [0],[1]

/* Returns TRUE/FALSE if mobile allocation is valid for a particular MA_NUMBER */
/* mask: ma_valid_bitmap for pcell or scell; ma_num: mobile alloc number, e.g. MA_NUMBER_14 */
#define IS_MOBILE_ALLOC_VALID( mask, ma_num )  ( IS_BIT_NUM_SET( mask, ma_num ) )

/* Value for an invalid change-mark value */
#define CHANGE_MARK_NOT_VALID  0x0f

#define UNINITIALISED_SYS_INFO_COUNT 0xffff
#define SYSTEM_INFO_30_SEC_RETRY 3
#define SYSTEM_INFO_20_SEC_RETRY 2
#define SYSTEM_INFO_10_SEC_RETRY 1
#define MAX_SYS_INFO_COUNT_VALUE 30 /* decremented every 10secs so will be 0 at 5mins */

/*
TMULTIFRM: Length of two multi-frames.
Assuming a value of 4.615ms/frame, and 51 frames/multiframe:
4.615 * 51 = 235.37   x 2 multi-frames   => 470.74ms
*/
#define TMULTIFRM_value 471

/************************************
  Logging
************************************/
#define GRR_LOG_MASK_GPRS_CELL_OPTIONS       (1<<0)
#define GRR_LOG_MASK_GPRS_POWER_CTRL_PARAMS  (1<<1)
#define GRR_LOG_MASK_GPRS_GENERAL_PARAMS     (1<<2)
#define GRR_LOG_MASK_GPRS_INDICATORS         (1<<3)
#define GRR_LOG_MASK_SI13_MOBILE_ALLOC       (1<<4)

/***********************************
  Typedefs global to RR
***********************************/

/* Frequency lists to be reprocessed by rr_gprs_reassign_band()*/

#define  NCELL_FREQ_LIST                     (1 << 0)
#define  CBCH_FREQ                           (1 << 1)
#define  CELL_ALLOC_FREQ_LIST                (1 << 3)
#define  MOBILE_ALLOC_FREQ_LIST              (1 << 4)
#define  IDLE_BA_FREQ_LIST                   (1 << 8)
#define  DED_BA_FREQ_LIST                    (1 << 9)

/***********************************
  NACC support flags
***********************************/
#define GTOG_NACC_SUPPORTED 0x00
#define WTOG_NACC_SUPPORTED 0x01
#define GTOW_NACC_SUPPORTED 0x02
#define FULL_NACC_SUPPORT   (GTOG_NACC_SUPPORTED | WTOG_NACC_SUPPORTED | GTOW_NACC_SUPPORTED)

#define RR_GPRS_RESELECTION_STRUCTURE_SIZE 6

/*Holds the reason for reselection. as diferent algorithms will apply for each*/
typedef enum{
  NORMAL_PATH_LOSS_RESELECTION,
  NORMAL_BETTER_NCELL_RESELECTION,
  OTHER_REASON_FOR_RESELECTION,
  PACKET_CELL_CHANGE_ORDERED,
  IMM_REL_PACKET_CELL_CHANGE_ORDERED
#ifdef FEATURE_INTERRAT_PCCO_GTOW
  ,INTERRAT_PCCO_GTOW_ORDERED
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
}gprs_reselection_reason_t;

typedef struct
{
  ARFCN_T  BCCH_ARFCN;
  byte     RXLEV_average;
  int      C1;
  int      C2;
  int      C31;
  int      C32;
} gprs_reselection_values_T;

typedef struct
{
  ARFCN_T  BCCH_ARFCN;
  byte  RXLEV_instant;
  int   C1_instant;
  int   C2_instant;
  int   C31_instant;
  int   C32_instant;
} gprs_instant_reselection_values_T;

typedef struct gprs_reselection_structure
{
  byte                               no_of_entries;
  gprs_reselection_values_T          surrounding_values[RR_GPRS_RESELECTION_STRUCTURE_SIZE];
  boolean                            serving_values_valid;
  gprs_reselection_values_T          serving_values;
  gprs_instant_reselection_values_T  instant_surrounding_values[RR_GPRS_RESELECTION_STRUCTURE_SIZE];
  gprs_instant_reselection_values_T  instant_serving_values;
  boolean                            idle_serving_snr_is_bad;
  boolean                            transfer_serving_snr_is_bad;
} gprs_reselection_structure_T;

typedef struct
{
  boolean     cell_reselection_attempted;
  gprs_reselection_values_T reselection_data;
  gprs_reselection_values_T reselection_data_with_CRH;
}better_cell_entry_T;

typedef struct
{
  byte no_of_entries;
  better_cell_entry_T candidate_cell[MAX_NUMBER_OF_BEST_CELLS];
}gprs_better_cell_list_T;

typedef enum
{
  IE_NOT_PRESENT,
  IE_PRESENT,
  IE_DECODED
} ie_decode_status_t;

/* Indication whether ncell data originated from a ncell bcch scan, or from PMO/PCCO */
typedef enum
{
  BCCH_FORMAT,
  PMO_FORMAT
} bcch_or_pmo_format_t;

/*
  Cell Identification
*/
typedef struct
{
  LAI_T   lai;
  byte    rac;
  uint16  cell_identity;

} cell_identification_t;

/*
  NC Measurement Parameters
*/
typedef struct
{
  byte  network_control_order;
  byte  nc_non_drx_period;
  byte  nc_reporting_period_i;
  byte  nc_reporting_period_t;

} nc_measurement_params_t;

/*
  GPRS Measurement Parameters (this includes the GPRS specific Enhanced Measurement Parameters)
*/
typedef struct
{
  boolean pmo_ind_valid;
  byte pmo_ind;                 /* This is only valid if a PMO has been received with Enhanced Measurement Parameters */
  report_type_T report_type;             /* BCCH: SI2quater GPRS MEASUREMENT PARAMETERS, PBCCH: PSI5 Enhanced Reporting Parameters */
  byte reporting_rate;          /* BCCH: SI2quater GPRS MEASUREMENT PARAMETERS, PBCCH: PSI5 Enhanced Reporting Parameters */
  byte invalid_bsic_reporting;  /* BCCH: SI2quater GPRS MEASUREMENT PARAMETERS, PBCCH: PSI5 Enhanced Reporting Parameters */

  byte multiband_reporting;
  byte serving_band_reporting;

  byte scale_ord;

  byte reporting_900_offset;
  byte reporting_900_threshold;

  byte reporting_1800_offset;
  byte reporting_1800_threshold;

  byte reporting_450_offset;
  byte reporting_450_threshold;

  byte reporting_1900_offset;
  byte reporting_1900_threshold;

  byte reporting_850_offset;
  byte reporting_850_threshold;
} gprs_meas_params_t;

/*
  GPRS 3G Measurement Parameters
*/
typedef struct
{
  uint8 Qsearch_P;
  uint8 _3G_SEARCH_PRIO;
  uint8 FDD_GPRS_Qoffset;
  uint8 FDD_Qmin;
  uint8 TDD_GPRS_Qoffset;
  uint8 FDD_REP_QUANT;
  uint8 FDD_MULTIRAT_REPORTING;
  uint8 FDD_REPORTING_OFFSET;
  uint8 FDD_REPORTING_THRESHOLD;
  uint8 TDD_MULTIRAT_REPORTING;
  uint8 TDD_REPORTING_OFFSET;
  uint8 TDD_REPORTING_THRESHOLD;
  uint8 CDMA2000_MULTIRAT_REPORTING;
  uint8 CDMA2000_REPORTING_OFFSET;
  uint8 CDMA2000_REPORTING_THRESHOLD;
  boolean FDD_additional_MPs_valid; /* need it, see 45.008 or rr_wcdma_cell_rscp_is_suitable() */
  uint8 FDD_Qmin_offset;
  uint8 FDD_RSCPmin;
} gprs_3g_meas_params_t;

/*
  Extended Measurement Parameters
*/
typedef struct
{
  byte                        ext_measurement_order;
  byte                        ext_reporting_type;
  byte                        ncc_permitted;
  boolean                     int_frequency_present;
  byte                        int_frequency;
  byte                        ext_reporting_period;
  cell_channel_description_T  frequencies;

} ext_measurement_params_t;

/*
  Serving Cell Parameters
*/
typedef struct
{
  boolean       cell_bar_access_2;
  boolean       exc_access;
  byte          gprs_rxlev_access_min;
  byte          gprs_ms_txpwr_max_cch;
  boolean       hcs_params_present;
  hcs_params_t  hcs_params;
  byte          multiband_reporting;

} gprs_serving_cell_params_t;

/*
  General Cell Selection Parameters
*/
typedef struct
{
  byte     gprs_cell_reselect_hysteresis;
  boolean  c31_hyst;
  boolean  c32_qual;
  boolean  random_access_entry;
  byte     t_resel;
  boolean  ra_reselect_hysteresis_present;
  byte     ra_reselect_hysteresis;

} general_cell_selection_params_t;

/*
  Global Power Control Params (from PSI1 / SI13 / PSI13)
*/
typedef struct
{
  uint8    alpha;
  uint8    t_avg_w;
  uint8    t_avg_t;
  uint8    pb;
  boolean  pc_meas_chan;
  boolean  int_meas_chan_list_avail;
  byte     n_avg_i;

} global_power_control_params_t;

typedef struct
{
  ARFCN_T arfcn;
  byte    tn;

} non_hopping_carrier_t;

typedef struct
{
  uint8  ma_number;
  uint8  maio;
  byte   tn;

} hopping_carrier_t;

#define MAX_NUM_OF_GPRS_MA_ARFCNS  64
/*
  GPRS Mobile Allocations
*/
typedef struct
{
  uint8   hsn;
  uint8   num_of_arfcns;
  ARFCN_T arfcns[MAX_NUM_OF_GPRS_MA_ARFCNS];

} gprs_mobile_allocation_t;

/* N.B. rrps_frequency_list_t contains the same elements as gprs_mobile_allocation_t, */
/* but uses the ARFCN_T instead of uint16 for the ARFCN. The 15 mobile allocations are */
/* stored using rrps_frequency_list_t instead of gprs_mobile_allocation_t to make */
/* their transfer to MAC easier, but all mobile allocation related RR functions use */
/* gprs_mobile_allocation_t */

#define MAX_NUM_OF_GPRS_MA   2   /* MA_NUMBER={14,15} */

typedef struct
{
  uint8                        si13_change_mark;
  uint16                       ma_valid_bitmap;
  rrps_frequency_list_t        gprs_mobile_allocation[MAX_NUM_OF_GPRS_MA];

} gprs_mobile_allocations_t;

/*
gprs_mobile_allocation_ie_t: contains the mobile allocation data received in SI13 / per PSI2
message instance
*/

/* newer compilers use a packed format */
typedef PACKED struct PACKED_POST
{
  uint8    hsn;
  uint8    num_of_rfl_numbers;    /* 0->15. If zero, MA references Cell Allocation */
  uint8    rfl_numbers[16];
  boolean  ma_bitmap_valid;
  uint64   ma_bitmap;             /* upto 64 bits: 1-freq included, 0-freq excluded */
  uint8    num_of_arfcn_indexes;  /* 0->15. If zero, complete RFL (or CA if no RFL) used */
  uint8    arfcn_indexes[16];     /* ARFCNs to exclude */

} gprs_mobile_allocation_ie_t;


typedef struct
{
  uint8                        change_mark;
  gprs_mobile_allocation_ie_t  mobile_allocation_ie;

} mobile_allocation_description_t;


/* values for 'nmo' in gprs_cell_options */
#define NMO_I         0
#define NMO_II        1
#define NMO_III       2
#define NMO_RESERVED  3
#define NMO_UNKNOWN   0xff

/* SGSNR: This is the SGSN software revision number. Although it's a boolean value, it can */
/* indicate three revisions, by not being present. */
typedef enum
{
  SGSNR_REV_98 = 0x00,        /* These values are chosen to match QXDM logging, where */
  SGSNR_REV_99 = 0x01,        /* 0xff is used to indicate 'not present'               */
  SGSNR_NOT_PRESENT = 0xff
} sgsnr_t;

/* MSCR: This is the MSC software revision number. Although it's a boolean value, it can */
/* indicate three revisions, by not being present. */
typedef enum
{
  MSCR_REV_98 = 0x00,        /* These values are chosen to match QXDM logging, where */
  MSCR_REV_99 = 0x01,        /* 0xff is used to indicate 'not present'               */
  MSCR_NOT_PRESENT = 0xff
} mscr_t;

/* GPRS Indicator IE */
typedef struct
{
  byte               ra_colour;
  normal_extended_t  si13_position;

} gprs_indicator_t;



/*****************************************************************

   Neighbour cell related defines & structures

*****************************************************************/

/*
  Neighbour Cell GPRS System Information, gathered from SI data (3,4 & 13)
  Not all data from SI3/4/13 is stored, only that necessary for cell reselection
  decisions
*/
typedef struct
{
  byte                             ra_colour;              /* SI3 */
  normal_extended_t                si13_position;          /* SI3 */
  boolean                          si13_expected;          /* TRUE when waiting for SI13 to be received */
  boolean                          si13_data_valid_flag;   /* TRUE when SI13 has been received */
  boolean                          mobile_allocation_present;
  mobile_allocation_description_t  mobile_allocation_description;
  union
  {
    struct
    {
      byte                             routing_area_code;
      boolean                          spgc_ccch_sup;
      byte                             priority_access_thr;
      byte                             network_control_order;
      gprs_cell_options_t              gprs_cell_options;
      global_power_control_params_t    power_control_params;

    }                                no_pbcch;

  }                                pbcch_or_no_pbcch;
  byte                             bcch_change_mark;
  sgsnr_t                          sgsnr;
  byte                             si_status_ind;
  two_quater_position_T            two_quater_position;
  boolean                          ccn_supported_valid;
  boolean                          ccn_supported;
} gprs_bcch_ncell_info_t;

/*
  GSM+GPRS Neighbour Cell information gathered from SI 3,4 & 13
  (when serving cell is 51-frame)
*/
typedef struct
{
  information_T                gsm;                   /* Link to GSM surround cell database */
  boolean                      gprs_supported;    /* Indicates GPRS support, derived from presence SI3 GPRS_INDICATOR IE */
  gprs_bcch_ncell_info_t       gprs;              /* GPRS-related sys info */
  uint16                       system_info_timer_count;  /* 5 min sys info timer */
  uint8                        fast_snr_candidate_marker;
  uint8                        slow_snr_candidate_marker;
  gsm_bsic_description_entry_T bsic_description;  /* Optionally received in SI2quater */
} bcch_ncell_info_t;

typedef struct
{
  boolean                cell_bar_access_2;
  boolean                exc_acc;
  boolean                same_ra_as_serving_cell;
  uint8                  gprs_rxlev_access_min;
  uint8                  gprs_ms_txpwr_max_cch;
  uint8                  gprs_temporary_offset;
  uint8                  gprs_penalty_time;
  uint8                  gprs_reselect_offset;
  boolean                hcs_params_present;            /* Indicates validity of 'hcs_params' */
  hcs_params_t           hcs_params;
  boolean                si13_location_present;         /* Indicates validity of 'si13_location' */
  normal_extended_t      si13_location;
  boolean                ccn_supported_valid;
  boolean                ccn_supported;

} gprs_cell_selection_params_t;

/*
  GPRS Neighbour Cell information broadcast in PSI3, 3bis
*/
typedef struct
{
  BSIC_T                       bsic;
  gprs_cell_selection_params_t gprs_cell_selection_params;
} pmo_ncell_info_t;

/*
  All Neighbour information, regardless of method of obtaining it.
  'bcch_ncells' contains ncell data received whilst on a '51 scell, by scanning SI3,4 & 13
  of a ncell.
  'pbcch_ncells' contains ncell data received either
    a) whilst on a '51 scell and having received a PMO with ncell parameters
    b) whilst on a '52 scell and having received PSI3

  If a PMO is received which contains ncell parameters for a ncell already present in
  'bcch_ncells', that ncell is removed from 'bcch_ncells' and a new entry created in
  'pbcch_ncells' (penalty timer information is copied over).
*/

#define MAX_SIZE_OF_GSM_NEIGHBOUR_CELL_LIST   96

#define MAX_SIZE_OF_BA_GPRS_LIST_FOR_L1       BA_MAX_LEN

#define INVALID_HCS_THRESHOLD                 (110 >> 1)
/*
   GSM Neighbour Cell List
*/

typedef enum
{
  NCELL_UNALLOCATED,
  NCELL_ORIGINAL,
  NCELL_DUPLICATE,
  NCELL_REMOVED,
  NCELL_ADDED,
  NCELL_ADDED_NC2_ONLY

} ncell_list_member_status_t;

typedef struct
{
  ncell_list_member_status_t  status;
  ARFCN_T                     arfcn;
  boolean                     bsic_valid;
  BSIC_T                      bsic;
  bcch_or_pmo_format_t        bcch_or_pmo;
  union
  {
    pmo_ncell_info_t              pmo_pcco;
    bcch_ncell_info_t             bcch;

  }                             ncell_data;

  uint8                       rxlev; /*updated by MPH_SURROUND_MEAS_IND*/
} gsm_ncell_list_member_t;

typedef struct
{
  uint8                    num_of_ncells;
  gsm_ncell_list_member_t  ncells[MAX_SIZE_OF_GSM_NEIGHBOUR_CELL_LIST];

} gsm_neighbour_cell_list_t;

typedef struct
{
  inter_task_BCCH_allocation_T   ba_gprs_list;                          /* BA(GPRS) list for L1 */
  gsm_neighbour_cell_list_t      gsm_neighbour_cell_list;               /* GSM Neighbour Cell List */
} gprs_ncells_info_t;

/*
  GPRS System Information, gathered from SI data (3,4 & 13) broadcast
  on a 51-frame serving cell.
  This also includes storage for any parameters passed to GRR from MAC during
  transfer whilst on a '51 scell.
*/

typedef struct
{
#ifdef FEATURE_FAKE_BTS_CELL_INFO
  boolean                        si_refresh_ongoing;           /*FR33173 - Fake Cell Detection*/
#endif
  boolean                        si13_expected_flag;           /* When TRUE event generated when SI13 received */
  boolean                        si13_data_valid_flag;         /* TRUE when SI13 has been received */
  boolean                        generate_si_received_event;
  uint16                         si_messages_required;         /* See rr_sys_info_si.c */
  uint16                         si_messages_low_priority;     /* See rr_sys_info_si.c */
  uint16                         si_messages_on_bcch_ext;      /* See rr_sys_info_si.c */
  uint16                         si_messages_not_broadcast;    /* See rr_sys_info_si.c */
  rr_si_type_t                   last_si_message_received;
  boolean                        single_l1_si_request_only;    /* See rr_sys_info_si.c */
  byte                           ra_colour;                    /* SI3/4 */
  normal_extended_t              si13_position;                /* SI3/4 */
  byte                           bcch_change_mark;             /* (P)SI13 */
  byte                           routing_area_code;            /* (P)SI13 */
  boolean                        spgc_ccch_sup;                /* (P)SI13 */
  byte                           priority_access_thr;          /* (P)SI13 */
  gprs_cell_options_t            gprs_cell_options;            /* (P)SI13 */
  uint8                          non_drx_timer;
  global_power_control_params_t  power_control_params;         /* (P)SI13; MAC */
  sgsnr_t                        sgsnr;                        /* (P)SI13 */
  byte                           si_status_ind;                /* (P)SI13 */
  boolean                        pmo_scell_params_valid;       /* TRUE when a PMO has been received with scell params */
  gprs_serving_cell_params_t     pmo_gprs_serving_cell_params; /* scell params received in a pmo */
  nc_meas_params_mods_t          nc_meas_params_si13;
  uint8                          three_g_ccn_active;           /* SI2quater */
  boolean                        LB_MS_TXPWR_MAX_CCH_valid;    /*SI13, PSI13, PSI14*/
  byte                           LB_MS_TXPWR_MAX_CCH;          /*SI13, PSI13, PSI14*/
} gprs_bcch_scell_info_t;

/*
  Global System Information Databse
  This contains System Information covering GSM, GPRS on 51 and GPRS on 52.
  This includes data covering for serving cell (scell), pending cell (pcell),
  and the neighbour cells (ncells)
*/

typedef enum
{
  GRR_L1_IDLE_MODE_NONE,
  GRR_L1_IDLE_MODE_GSM,
  GRR_L1_IDLE_MODE_CCCH,
  GRR_L1_IDLE_MODE_MAX

} grr_l1_idle_mode_t;

typedef struct
{
  uint8  max_LAPDm;
  boolean  gprs_ms_txpwr_max_cch_valid;
  uint8  gprs_ms_txpwr_max_cch;

} dtm_sys_info_t;

/*
  gprs_scell_info_t:
  Structure containing system information relating to a serving or pending
  cell, either GSM (bcch), GPRS-51 (bcch) or GPRS-52 (pbcch).
  The GPRS mobile allocations are common to both cell types. For GPRS-51,
  the SI13 mobile allocation can be found at MA_NUMBER=14
*/
typedef struct
{
  system_information_database_T    gsm;                    /* GSM SI database */
  grr_l1_idle_mode_t               l1_idle_mode;           /* Current L1 idle mode */
  gprs_bcch_scell_info_t           bcch_sys_info;          /* Sys Info from (P)SI13 when camped on BCCH */
  boolean                          dtm_sys_info_valid;     /* Indicates DTM support */
  dtm_sys_info_t                   dtm_sys_info;           /* from SI6ro / DTM Information / Packet Assignment */
  gprs_mobile_allocations_t        mobile_allocations;     /* from (P)SI13 / PSI2 */

  nc_measurement_params_t          nc_meas_params;
  gprs_meas_params_t               gprs_meas_params;
  gprs_3g_meas_params_t            gprs_3g_meas_params;
  rr_gsm_bsic_description_T        gprs_bsic_description;  /* From SI2quater */

  boolean                          serving_cell_priority_params_valid;
  csn_gsm_priority_params_ie_t     serving_cell_priority_params;
  csn_utran_priority_params_ie_t   utran_priority_params;
  rr_eutran_params_t               eutran_params;

#ifdef FEATURE_LTE
  /* Current consolidated LTE neighbour list, based on parameters broadcast
  on the BCCH (SI2quater) and the PACCH (Packet Measurement Order). */
  rr_lte_neighbour_list_t          lte_neighbour_list;
#endif /* FEATURE_LTE */

  boolean                          pwr_reduction_valid;    /* From SI2quater */
  uint8                            int_pwr_red_value;      /* 0 = apply power reduction to all calls*/
                                                           /* 1 = Don't apply power reduction to emergency calls*/
  boolean                          nc2_csg_pccn_permitted;

} gprs_scell_info_t;

/*
  global_system_info_database_t:
  Structure pulling all the system information for scell, pcell & ncells
  together
*/
typedef struct
{
  gprs_scell_info_t    scell;    /* Serving cell system info */
  gprs_scell_info_t    pcell;    /* Pending cell system info */
  gprs_ncells_info_t   ncells;   /* Neighbour cells system info */

} gprs_system_info_database_t;

typedef struct
{
  uint16    length_bits;
  uint8   * ms_ra_cap_ie;

} gmm_ms_ra_cap_ie;

/*
  RR Public Store update
  Note: It is not necessary to provide IDs for anything that is set to a NV value during
  initialisation, and will not be subsequently changed due to a change in system
  information, etc.
*/
typedef uint32 rrps_store_update_bitmap_t;

#define RRPS_UPD_CHANGE_MARK_SI13                 (1 << 0)
#define RRPS_UPD_VALID_MA_BIT_MAP                 (1 << 2)
#define RRPS_UPD_CELL_OPTIONS                     (1 << 3)
#define RRPS_UPD_POWER_CTRL                       (1 << 5)
#define RRPS_UPD_SIM_IMSI                         (1 << 6)
#define RRPS_UPD_SIM_TMSI                         (1 << 7)
#define RRPS_UPD_SIM_PTMSI                        (1 << 8)
#define RRPS_UPD_GMM_STATUS                       (1 << 10)
#define RRPS_UPD_GMM_TLLI                         (1 << 11)
#define RRPS_UPD_GMM_READY_TIMER                  (1 << 12)
#define RRPS_UPD_GMM_MS_RA_CAP_IE                 (1 << 13)
#define RRPS_UPD_MS_NON_DRX_TIMER                 (1 << 14)
#define RRPS_UPD_MS_BCCH_BAND                     (1 << 15)
#define RRPS_UPD_PRIORITY_ACCESS_THR              (1 << 16)
#define RRPS_UPD_NW_BAND_IND                      (1 << 17)

typedef struct
{
  uint8                          change_mark_si13;
  uint16                         ma_valid_bitmap;
  gprs_cell_options_t            gprs_cell_options;
  global_power_control_params_t  power_control_params;
  byte                           imsi[MM_SIM_IMSI_SIZE];
  byte                           tmsi[MM_SIM_TMSI_SIZE];
  byte                           ptmsi[MM_SIM_PTMSI_SIZE];
  uint32                         new_tlli;
  uint32                         old_tlli;
  gmm_llc_if_state_type          gmm_ready_state;
  boolean                        negotiated_ready_timer_zero;
  gmm_ms_ra_cap_ie               ms_ra_cap_ie;
  uint8                          non_drx_timer;
  uint8                          ms_bcch_band;
  uint8                          priority_access_thr;
  uint8                          nw_band_ind;
} rrps_store_update_data_t;

typedef enum
{
  GRR_PS_DOMAIN_CHANGE_DUE_TO_UNKNOWN,
  GRR_PS_DOMAIN_CHANGE_DUE_TO_SERVICE_DOMAIN,
  GRR_PS_DOMAIN_CHANGE_DUE_TO_GMM

} grr_ps_domain_change_reason_t;

#ifdef FEATURE_GSM_DTM

typedef enum
{
  DTM_PS_ASSIGNMENT_REQUIRED = 0,
  DTM_CS_ASSIGNMENT_REQUIRED = 1
} dtm_validation_t;

typedef struct
{
  uint8                            power_level;
  channel_information_T            channel_description;
  boolean                          channel_mode_present;
  channel_mode_T                   channel_mode;
  boolean                          multi_rate_config_provided;
} dtm_channel_assignment_t;

#endif /* FEATURE_GSM_DTM */

/***********************************
  Functions global to RR
***********************************/

/* In rr_public_store.c */
extern void rr_public_store_lock_init(void);
extern void rr_process_mac_grr_store_update(mac_grr_store_update_t *, const gas_id_t gas_id);
extern void rr_public_store_update_multislot_class(
  uint8 egprs_mulislot_class,
#ifdef FEATURE_GSM_DTM
  uint8 dtm_egprs_multislot_subclass,
#endif
  uint8 gprs_multislot_class,
  const gas_id_t gas_id
);
extern void rr_public_store_update_nv_data(const gas_id_t gas_id);
extern void rr_write_public_store(rrps_store_update_bitmap_t, rrps_store_update_data_t *, const gas_id_t gas_id);
extern void rr_public_store_lock_init(void);
extern void rr_public_store_task_start_init(void);

/* In rr_sys_info_main.c */
extern void rr_init_gprs_sys_info_database(const gas_id_t gas_id);
extern rr_event_T rr_gprs_sys_info_timer_handler(rr_timer_id_T, const gas_id_t gas_id);
extern void rr_gprs_sys_info_new_paging_mode(paging_mode_T, paging_mode_T, const gas_id_t gas_id);
extern void rr_gprs_activate_camped_sys_info_handling(const gas_id_t gas_id);
extern void rr_gprs_suspend_camped_sys_info_handling(boolean, const gas_id_t gas_id);
extern void rr_gprs_resume_camped_sys_info_handling(const gas_id_t gas_id);
extern void rr_gprs_deactivate_timed_sys_info_refresh(const gas_id_t gas_id);
extern void rr_gprs_setup_sys_info_after_handover(ARFCN_T arfcn, BSIC_T bsic, const gas_id_t gas_id);
extern void rr_gprs_invalidate_sys_info_during_cs_conn(rr_invalidate_si_reason_t invalidate_reason, const gas_id_t gas_id);
extern boolean grr_verify_sys_info_is_valid_for_l1_camp(const gas_id_t gas_id);
extern rr_event_T rr_gprs_sys_info_acquisition_control(rr_event_T rr_event_in, void * event_in_data, const gas_id_t gas_id);
extern boolean rr_gprs_scell_sys_info_acquisition_in_progress(const gas_id_t gas_id);
extern boolean rr_is_this_the_scell(ARFCN_T, const gas_id_t gas_id);
extern gprs_scell_info_t * rr_gprs_get_scell_info(const gas_id_t gas_id);
extern ARFCN_T rr_gprs_get_scell_arfcn(const gas_id_t gas_id);
extern BSIC_T rr_gprs_get_scell_bsic(const gas_id_t gas_id);
extern PLMN_id_T rr_gprs_get_scell_plmn_id(const gas_id_t gas_id);
extern gprs_cell_options_t *rr_si_get_gprs_cell_options(const gas_id_t gas_id);
extern global_power_control_params_t * rr_si_get_power_control_params(const gas_id_t gas_id);
extern boolean rr_si_diff_gprs_cell_options(gprs_cell_options_t *gprs_cell_options_ptr, const gas_id_t gas_id);
extern boolean rr_gprs_camped_on_ccch(const gas_id_t gas_id);
extern boolean rr_gsm_camped_on_ccch(const gas_id_t gas_id);
extern boolean rr_cell_supports_gprs(const gas_id_t gas_id);
extern boolean rr_cell_supports_egprs(const gas_id_t gas_id);
#ifdef FEATURE_GSM_DTM
extern boolean rr_cell_supports_dtm(const gas_id_t gas_id);
#endif // FEATURE_GSM_DTM
extern boolean rr_cell_si_valid(const gas_id_t gas_id);
extern boolean rr_gprs_get_sys_info_for_mm(mm_gprs_sys_info_T *, const gas_id_t gas_id);
extern network_op_mode_type rr_si_get_nmo(const gas_id_t gas_id);
extern uint8 rr_gprs_decode_drx_timer(uint8);
extern uint8 rr_gprs_calc_non_drx_timer(uint8, const gas_id_t gas_id);
extern boolean rr_gprs_l1_sys_info_update_allowed(const gas_id_t gas_id);
extern boolean rr_gprs_mm_sys_info_update_allowed(const gas_id_t gas_id);
extern boolean rr_gprs_mac_sys_info_update_allowed(const gas_id_t gas_id);
extern void rr_gprs_sys_info_allow_all_updates(const gas_id_t gas_id);
extern void rr_gprs_sys_info_block_all_updates(const gas_id_t gas_id);
extern void grr_update_mm_with_rr_service_ind(const gas_id_t gas_id);
extern void rr_si_update_public_store(const gas_id_t gas_id);

/* In rr_gprs_freq_lists.c */
extern void rr_gprs_decode_freq_list_ie(byte *, uint8, cell_channel_description_T *, const gas_id_t gas_id);
extern void rr_gprs_decode_mobile_alloc_freq_list(uint8 *, uint8, gprs_mobile_allocation_t *, const gas_id_t gas_id);
extern void rr_gprs_decode_mobile_alloc(
  const gprs_mobile_alloc_t *   ma_data,
  gprs_mobile_allocation_ie_t * ma_store,
  cell_channel_description_T *  cell_alloc,
  gprs_mobile_allocation_t *    mobile_alloc
);

/* In rr_gprs_general.c */
extern void rr_gprs_reset_software(const gas_id_t gas_id);
extern void rr_gprs_get_l1_params(ccch_pccch_params_t *, const gas_id_t gas_id);
extern void rr_gprs_start_gsm_idle_mode(gprs_scell_info_t *, boolean, const gas_id_t gas_id);
extern void rr_start_ccch51_idle_mode(gprs_scell_info_t *, const gas_id_t gas_id);
extern void rr_gprs_update_l1_params(const gas_id_t gas_id);
extern void rr_gprs_perform_logging(uint16 logging_mask, gprs_scell_info_t *, const gas_id_t gas_id);
extern boolean rr_gprs_general_timer_expiry_handler(rr_timer_id_T, rr_event_T *, const gas_id_t gas_id);
extern void grr_start_gprs_recent_activity_timer(const gas_id_t gas_id);
extern void grr_stop_gprs_recent_activity_timer(const gas_id_t gas_id);
extern boolean grr_has_there_been_recent_gprs_activity(const gas_id_t gas_id);
extern void grr_start_guard_timer(dword, rr_event_T, const gas_id_t gas_id);
extern void grr_stop_guard_timer(const gas_id_t gas_id);
extern void grr_throwaway_saved_message(const gas_id_t gas_id);
extern boolean grr_is_ps_allowed_by_service_domain(const gas_id_t gas_id);
extern void rr_gprs_odm_info_rach_attempt(const gas_id_t gas_id);
extern void rr_gprs_odm_info_rach_success(const gas_id_t gas_id);
extern void rr_gprs_odm_update_cell_type(boolean in_service, const gas_id_t gas_id);
extern rrps_store_update_data_t *rr_get_rrps_store_update(const gas_id_t gas_id);
extern void rr_free_rrps_store_update(rrps_store_update_data_t *, const gas_id_t gas_id);

/**
  @brief Process a timer expiration message received during cell reselection.

  @param timer_expiry: The timer expiry message.
  @returns rr_event_T: The output of the cell reselection state machine.
*/
extern rr_event_T rr_gprs_reselection_timer_handler(timer_expiry_T *timer_expiry, const gas_id_t gas_id);

/* In rr_gprs_meas_rpt_control.c */
extern uint8 rr_gprs_get_nc_mode(const gas_id_t gas_id);
extern rr_event_T rr_gprs_nc_measurement_control(
  rr_event_T nc_meas_event,
  void* new_message,
  const gas_id_t gas_id
);
extern rr_l1_freq_power_T rr_gprs_get_serving_measurements(const gas_id_t gas_id);
extern void rr_gprs_merge_nc_nacc_into_cell_nacc_params(
  gprs_cell_options_t*,
  pmo_pcco_nacc_params_t*,
  const gas_id_t gas_id
);
extern void rr_gprs_merge_ccn_support_into_ba_list(
  csn_ccn_support_desc_t *ccn_support_desc,
  const gas_id_t gas_id
);
extern void rr_gprs_nacc_check_pcco_reject(ARFCN_T, ARFCN_T, const gas_id_t gas_id);

extern byte rr_gprs_get_network_control_order(const gas_id_t gas_id);

extern byte rr_gprs_3g_meas_get_rep_quant(int, const gas_id_t gas_id);
extern boolean rr_gprs_3g_meas_report_encode(three_g_measurement_report_t *, const gas_id_t gas_id);

/* In rr_shared_cell_selection.c */
extern void rr_get_penalty_timer_id(rr_timer_id_T *, const gas_id_t gas_id);

/* In rr_main.c */
#ifdef PERLUTF
#error code not present
#endif // PERLUTF

/***********************************
  Variables global to RR
***********************************/

/****************************************************************************
 *   Function name:  rr_get_gprs_system_info_database_valid
 *   --------------------------
 *
 *   Description: Checks the validity of the sys info database.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  TRUE - if valid, FALSE - otherwise
 *   --------
 *
 ****************************************************************************/
boolean rr_get_gprs_system_info_database_valid(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_get_gprs_system_info_database_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to gprs_system_info_database.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  gprs_system_info_database_t *
 *   --------
 *
 ****************************************************************************/
gprs_system_info_database_t *rr_get_gprs_system_info_database_ptr(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_clear_gprs_cell_info
 *   --------------------------
 *
 *   Description: Clears the GPRS cell info.  Note: leaves gsm si db untouched.
 *
 *   Parameters: gprs_scell_info_t cell info to be cleared.
 *   -----------
 *
 *   Returns:
 *   --------
 *
 ****************************************************************************/
void rr_clear_gprs_cell_info(gprs_scell_info_t *cell_info);

/****************************************************************************
 *   Function name:  rr_get_gprs_reselection_list_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to gprs_reselection_list.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  gprs_reselection_structure_T *
 *   --------
 *
 ****************************************************************************/
gprs_reselection_structure_T *rr_get_gprs_reselection_list_ptr(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_data_free_gprs_system_info_database_data
 *   --------------------------
 *
 *   Description: De allocate the heap assigned for GPRS scell info.
 *
 *   Parameters: gas_id_t
 *   -----------
 *
 *   Returns:
 *   --------
 *
 ****************************************************************************/
void rr_data_free_gprs_system_info_database_data(const gas_id_t gas_id);

extern boolean grr_can_process_rr_resel_abort_req(const gas_id_t gas_id);
extern boolean grr_can_process_rr_abort_req(const gas_id_t gas_id);
extern boolean grr_can_process_rr_grr_gprs_ia_ind(const gas_id_t gas_id);
extern boolean grr_can_process_rr_event_ind(const gas_id_t gas_id);
extern boolean grr_can_process_mm_message(const gas_id_t gas_id);
extern boolean grr_can_process_timer_expiry(const gas_id_t gas_id);
extern boolean grr_can_process_serving_meas_ind(const gas_id_t gas_id);
extern boolean grr_can_process_surround_meas_ind(const gas_id_t gas_id);
extern boolean grr_can_process_change_mode_ind(const gas_id_t gas_id);
#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
extern boolean rr_is_read_pch_during_transfer_allowed(const gas_id_t gas_id);
#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */
#endif  /* #ifndef RR_GPRS_DEFS_H */

/* EOF */


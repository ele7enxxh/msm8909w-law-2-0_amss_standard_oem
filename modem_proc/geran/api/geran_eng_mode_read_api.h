/*****************************************************************************
***
*** TITLE
***
***  GERAN  ENG MODE READ API INTERFACE
***
***  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/geran_eng_mode_read_api.h#1 $
***
*** DESCRIPTION
***
***  Provides APIs to extract information from the GSM/GPRS Access Stratum
***
*** FUNCTION SUMMARY
***
***
***
*** Copyright (c) 2005-2014 Qualcomm Technologies, Inc.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/
#ifndef GERAN_ENG_MODE_READ_API_H
#define GERAN_ENG_MODE_READ_API_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "time_svc.h"
#include "sys_v.h"
#include "sys_type.h"
#include "geran_dual_sim.h"

/*****************************************************************************
***
***     Type Definitions
***
*****************************************************************************/
#define MAX_NUM_CALLBACK_REGISTRATIONS 1 /*max number of callback registrations per geran_eng_mode_info_type_t*/
#define GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER        0xffffffff

typedef enum
{                                                        /*geran_modem_stats_mask bit position*/
  ENG_MODE_TIMING_ADVANCE,                               /* Bit 0 -  0x01*/
  ENG_MODE_NMR_INFO,                                     /* Bit 1 -  0x02*/
  ENG_MODE_RX_QUAL,                                      /* Bit 2 -  0x04*/
  ENG_MODE_TX_PWR,                                       /* Bit 3 -  0x08*/
  ENG_MODE_SCELL_INFO,                                   /* Bit 4 -  0x10*/
  ENG_MODE_SCELL_MEASUREMENTS,                           /* Bit 5 -  0x20*/
  ENG_MODE_AMR_INFO,                                     /* Bit 6 -  0x40*/
  ENG_MODE_DTX_UL_STATUS,                                /* Bit 7 -  0x80*/
  ENG_MODE_TBF_INFO,                                     /* Bit 8 -  0x100*/
  ENG_MODE_NW_PARAMS,                                    /* Bit 9 -  0x200*/
  ENG_MODE_GPRS_UL_CS,                                   /* Bit 10 - 0x400*/
  ENG_MODE_GPRS_DL_CS,                                   /* Bit 11 - 0x800*/
  ENG_MODE_EGPRS_UL_MCS,                                 /* Bit 12 - 0x1000*/
  ENG_MODE_EGPRS_DL_MCS,                                 /* Bit 13 - 0x2000*/
  ENG_MODE_EDGE_MEAS,                                    /* Bit 14 - 0x4000*/
  ENG_MODE_SCELL_CONFIG,                                 /* Bit 15 - 0x8000*/
  ENG_MODE_HOPPING_STATUS,                               /* Bit 16 - 0x10000*/
  ENG_MODE_CURRENT_L1_TIMESLOT,                          /* Bit 17 - 0x20000*/
                                                         /* Do NOT change the order of the above bitmapped enums.
                                                            Add new entries here for geran_modem_stats_mask bit position reporting.
                                                            Up to a max of 64 bit positions*/
  ENG_MODE_MAX_BITMAPPED_INFO_TYPE_ELEMENTS,             /*No Bit Pos - Marker for end of bitmapped enums, Warning this will overflow GERAN_ENG_MODE_INFO_TYPE_TO_BITMASK when 64*/

  ENG_MODE_RADIO_LINK_TIMEOUT_VALUE,                     /* No Bit Pos*/
  ENG_MODE_TOTAL_UPLINK_LLC_PDUS_TRANSMITTED,            /* No Bit Pos*/
  ENG_MODE_TOTAL_DOWNLINK_LLC_PDUS_RECEIVED,             /* No Bit Pos*/
  ENG_MODE_TOTAL_UPLINK_LLC_PDUS_RETRANSMITTED,          /* No Bit Pos*/
  ENG_MODE_TOTAL_DOWNLINK_LLC_PDUS_RETRANSMITTED,        /* No Bit Pos*/
  ENG_MODE_TOTAL_UPLINK_RLC_BLOCKS_TRANSMITTED,          /* No Bit Pos*/
  ENG_MODE_TOTAL_DOWNNLINK_RLC_BLOCKS_RECEIVED,          /* No Bit Pos*/
  ENG_MODE_TOTAL_UPLINK_RLC_BLOCKS_RETRANSMITTED,        /* No Bit Pos*/
  ENG_MODE_TOTAL_DOWNLINK_REPEATED_RLC_BLOCKS_RECEIVED,  /* No Bit Pos*/
  ENG_MODE_HOPPING_FREQUENCY_LIST,                       /* No Bit Pos*/
  ENG_MODE_GMSK_CV_VALUE,                                /* No Bit Pos*/
  ENG_MODE_PSK_CV_VALUE,                                 /* No Bit Pos*/
  ENG_MODE_GERAN_MODEM_STATS_INFO,                       /* No Bit pos*/
  ENG_MODE_SIGNAL_INFO,                                  /* No Bit Pos*/
  ENG_MODE_TX_PWR_PA_INFO,                               /* No Bit Pos  New for HTC PA gain and pwr info request */
  ENG_MODE_TX_BAND,                                      /* No Bit Pos  New for HTC PA gain and pwr info request */
  ENG_MODE_CODEC_HO_INFO,                                /* No Bit Pos*/
  ENG_MODE_CANDIDATE_PLMN_ID,                            /* No Bit Pos*/
  ENG_MODE_LLC_GEA_ALGORITHM,                            /* No Bit Pos */
  ENG_MODE_GSM_CIPHER_INFO,                              /* No Bit Pos*/
  ENG_MODE_CODEC_HO_SAMPLE_RATE,                         /* No Bit Pos*/
                                                         /* Add new entries here if they are not required for geran_modem_stats_mask */
  ENG_MODE_ALL_INFORMATION,                              /* Obsolete */
  ENG_MODE_MAX_NUM_INFO_TYPE_ELEMENTS                    /* No Bit pos*/
} geran_eng_mode_info_type_t;

/*bitmap type definition for ENG_MODE_GERAN_MODEM_STATS_INFO*/
typedef struct
{
  uint64 geran_modem_stats_mask;
}geran_eng_mode_info_bitmask_t;

#define GERAN_ENG_MODE_INFO_TYPE_TO_BITMASK(x) ((uint64)(1ULL << x))

/***********************************************************************/
/************** ENG_MODE_TIMING_ADVANCE ********************************/
/***********************************************************************/
/* type: uint32, range 0..63, 0xffffffff - invalid*/

/***********************************************************************/
/************** ENG_MODE_RADIO_LINK_TIMEOUT_VALUE **********************/
/***********************************************************************/
/* type: uint32, range 0..64, 0xffffffff - invalid*/

/***********************************************************************/
/************** ENG_MODE_TOTAL_UPLINK_LLC_PDUS_TRANSMITTED *************/
/***********************************************************************/
/* type: uint32, 0xffffffff - invalid */

/***********************************************************************/
/************** ENG_MODE_TOTAL_DOWNLINK_LLC_PDUS_RECEIVED **************/
/***********************************************************************/
/* type: uint32, 0xffffffff - invalid */

/***********************************************************************/
/************** ENG_MODE_TOTAL_UPLINK_LLC_PDUS_RETRANSMITTED ***********/
/***********************************************************************/
/* type: uint32, 0xffffffff - invalid */

/***********************************************************************/
/************** ENG_MODE_TOTAL_DOWNLINK_LLC_PDUS_RETRANSMITTED *********/
/***********************************************************************/
/* type: uint32, 0xffffffff - invalid */

/***********************************************************************/
/************** ENG_MODE_TOTAL_UPLINK_RLC_BLOCKS_TRANSMITTED ***********/
/***********************************************************************/
/* type: uint32, 0xffffffff - invalid */

/***********************************************************************/
/************** ENG_MODE_TOTAL_DOWNNLINK_RLC_BLOCKS_RECEIVED ***********/
/***********************************************************************/
/* type: uint32, 0xffffffff - invalid */

/***********************************************************************/
/************** ENG_MODE_TOTAL_UPLINK_RLC_BLOCKS_RETRANSMITTED *********/
/***********************************************************************/
/* type: uint32, 0xffffffff - invalid */

/***********************************************************************/
/************** ENG_MODE_TOTAL_DOWNLINK_REPEATED_RLC_BLOCKS_RECEIVED ***/
/***********************************************************************/
/* type: uint32, 0xffffffff - invalid */

/***********************************************************************/
/************** ENG_MODE_RX_QUAL ***************************************/
/***********************************************************************/
/* type: uint32, range 0..7, 0xffffffff - invalid */

/************** ENG_MODE_GMSK_CV_VALUE *********************************/
/************** ENG_MODE_PSK_CV_VALUE **********************************/
/***********************************************************************/
/* type: uint32, range 0..7, 0xffffffff - invalid */

/***********************************************************************/
/************** ENG_MODE_TX_PWR ****************************************/
/***********************************************************************/
#define ENG_MODE_TX_PWR_NUM_TIMESLOTS 8
/* type: int16 tx_pwr_in_use[ENG_MODE_TX_PWR_NUM_TIMESLOTS] */
/* The array is populated from the number of timeslots assigned. eg.
   if four TX timeslots are assigned then array indexes 0-3 are populated
   (lowest array index is lowest timeslot number). All other non assigned
   indexes should be ignored.
   The array indexes do NOT correspond to timeslot numbers.
   Valid range of TX power - 0..31. See Etsi 45.005 sect 4.1.1 */

/***********************************************************************/
/************** ENG_MODE_DTX_UL_STATUS *********************************/
/***********************************************************************/
/* type: uint8, 0 - Off, 1 - On, 0xff - Invalid*/

/***********************************************************************/
/************** ENG_MODE_GPRS_UL_CS ************************************/
/************** ENG_MODE_GPRS_DL_CS ************************************/
/***********************************************************************/
/* type: uint8, Range 0-3, 0xff - invalid */

/***********************************************************************/
/************** ENG_MODE_EGPRS_UL_MCS **********************************/
/************** ENG_MODE_EGPRS_DL_MCS **********************************/
/***********************************************************************/
/* type uint8, Range 0-8, 0xff - invalid*/

/***********************************************************************/
/************** ENG_MODE_HOPPING_STATUS ********************************/
/***********************************************************************/
typedef struct
{
  uint8 hopping_status; /*Range 0=no, 1=yes, 0xff - invalid*/
} eng_mode_hopping_status_t;

/***********************************************************************/
/************** ENG_MODE_HOPPING_FREQUENCY_LIST ************************/
/***********************************************************************/

typedef enum
{
  ENG_MODE_SYS_BAND_EGSM_900,
  ENG_MODE_SYS_BAND_PGSM_900,
  ENG_MODE_SYS_BAND_PCS_1900,
  ENG_MODE_SYS_BAND_DCS_1800,
  ENG_MODE_SYS_BAND_CELL_850,
  ENG_MODE_INVALID_BAND,
  ENG_MODE_SYS_BAND_NONE = 0xFF
} eng_mode_sys_band_T;

/* ARFCN type, includes band if quadband is supported */
typedef struct
{
  uint16              num;
  eng_mode_sys_band_T band;

} ENG_MODE_ARFCN_T;

typedef struct
{
   byte                 no_of_items; /*Default to 0 items when frequency list is invalid*/
   ENG_MODE_ARFCN_T     channel[64];
} eng_mode_frequency_list_T;

/***********************************************************************/
/********************** ENG_MODE_SCELL_INFO ****************************/
/***********************************************************************/

#define ENG_MODE_MAX_PLMN_ID_SIZE 3

typedef enum
{
  ENG_MODE_NO_CAUSE = 1
} eng_mode_cell_cb_cause_t;

typedef struct
{
  uint8 plmn_id[ENG_MODE_MAX_PLMN_ID_SIZE]; /*see 24.008 10.5.1.3 for representation*/
} eng_mode_plmn_id_t;

typedef enum
{
  ENG_MODE_SCELL_BAND_EGSM       = SYS_BAND_EGSM_900,
  ENG_MODE_SCELL_BAND_PGSM       = SYS_BAND_PGSM_900,
  ENG_MODE_SCELL_BAND_PCS        = SYS_BAND_PCS_1900,
  ENG_MODE_SCELL_BAND_DCS        = SYS_BAND_DCS_1800,
  ENG_MODE_SCELL_BAND_GSM_850    = SYS_BAND_CELL_850,
  ENG_MODE_SCELL_MAXNO_BANDS     = INVALID_BAND,
  ENG_MODE_SCELL_BAND_NONE       = SYS_BAND_NONE
} eng_mode_band_t;

typedef struct
{
  uint32                   cell_id; /* range 0..65535, cell id not available: 0xffffffff
                                       (all data in eng_mode_scell_info_t invalid when cell_id invalid) */
  uint16                   lac;     /* range 0..65535, see 24.008 10.5.1.3 */
  eng_mode_plmn_id_t       plmn;    /* see 24.008 10.5.1.3 */
  uint16                   arfcn;   /*range 0..1023 */
  uint8                    bsic;    /* range 0..63, see 23.003 4.3.2 */
  eng_mode_cell_cb_cause_t cell_info_cause;
  uint16                   rac;     /*range 0..255, invalid=0xffff (i.e. GPRS not supported)*/
  time_type                time_stamp;
  eng_mode_band_t          band;
} eng_mode_scell_info_t;


/***********************************************************************/
/*********************** ENG_MODE_NMR_INFO *****************************/
/*********Neighbour Cell Info (NMR - Network Measurement Report) *******/
/***********************************************************************/

#define ENG_MODE_MAX_NMR_ITEMS  6   /* Requirement: The maximum number of valid
                                     * measurements will be limited to 6
                                     */
typedef struct
{
  uint32             cell_id; /* range 0..65535, cell id not available: 0xffffffff
                                 (all data in eng_mode_ncell_id_info_t invalid when cell_id invalid) */
  uint16             lac;     /* range 0..65535, see 24.008 10.5.1.3 */
  eng_mode_plmn_id_t plmn;    /* see 24.008 10.5.1.3 */
} eng_mode_nmr_optional_info_t;

typedef struct
{
  uint16                       arfcn; /*range 0..1023 */
  uint8                        bsic;  /* range 0..63, see 23.003 4.3.2 */
  uint8                        rxlev; /* RX_LEV range 0..63, See 45.008 sect 8.1.4, default: 0*/
  eng_mode_nmr_optional_info_t optional_info; /*A neighbour cell entry can be present without this info*/
  eng_mode_band_t              band;
  int32                        C1; /*C1 as defined in 45.008 6.4, default: 0*/
  int32                        C2; /*C1 as defined in 45.008 6.4, default: 0*/
  int32                        C31; /*C31 as defined in 45.008 10.1.2, default: 0*/
  int32                        C32; /*C32 as defined in 45.008 10.1.2, default: 0*/
} nmr_element_t;

typedef struct
{
  uint8                    num_of_items; /* Set to zero if nmr info is invalid or no neighbours to report */
  nmr_element_t            nmr[ENG_MODE_MAX_NMR_ITEMS];
  time_type                time_stamp;
} eng_mode_nmr_info_t;

/***********************************************************************/
/********************** ENG_MODE_SCELL_MEASUREMENTS ********************/
/***********************************************************************/

typedef enum
{
  ENG_MODE_RX_LEVEL_TCH_PDTCH,
  ENG_MODE_RX_LEVEL_BCCH
} eng_mode_rx_lev_chan_type_t;

typedef struct
{
  int32                       C1; /*C1 as defined in 45.008 6.4, default: 0*/
  int32                       C2; /*C1 as defined in 45.008 6.4, default: 0*/
  uint16                      rx_lev; /* RX_LEV/RSSI range 0..63, See 45.008 sect 8.1.4, default: 0*/
  uint16                      rx_qual_sub; /*RX_QUAL_SUB range 0..7, 0xffff - invalid. Only valid in dedicated mode*/
  eng_mode_rx_lev_chan_type_t channel_type; /*Indicates channel measurements taken on*/
  time_type                   time_stamp;
  uint16                      rx_lev_sub; /* RX_LEV_SUB 0..63, See 45.008 sect 8.1.4, 0xffff - invalid. Only valid in dedicated mode*/
  uint16                      rx_lev_full; /* RX_LEV_FULL 0..63, See 45.008 sect 8.1.4, 0xffff - invalid. Only valid in dedicated mode*/
  uint16                      rx_qual_full; /*RX_QUAL_FULL range 0..7, 0xffff - invalid. Only valid in dedicated mode*/
  int32                       C31; /*C31 as defined in 45.008 10.1.2, default: 0*/
  int32                       C32; /*C32 as defined in 45.008 10.1.2, default: 0*/
} eng_mode_scell_measurements_t;

/***********************************************************************/
/************************ ENG_MODE_AMR_INFO ****************************/
/***********************************************************************/

typedef enum
{
  ENG_MODE_FULL_RATE,
  ENG_MODE_HALF_RATE,
  ENG_MODE_ENHANCED_FULL_RATE,
  ENG_MODE_AMR_FULL_RATE,
  ENG_MODE_AMR_HALF_RATE,
  ENG_MODE_INVALID_CHAN_MODE = 0xFF
}eng_mode_chan_type_t;

typedef struct
{
  uint8   ul_chan_mode;
  uint8   dl_chan_mode; /*  0 - FULL RATE
                            1 - HALF RATE
                            2 - ENHANCED FULL RATE
                            3 - AMR FULL RATE
                            4 - AMR HALF RATE
                            0xFF - INVALID MODE */
  uint8   ul_acs[4];
  uint8   dl_acs[4];    /* 0 - 4.75 kbit /s
                           1 - 5.15 kbit /s
                           2 - 5.90 kbit /s
                           3 - 6.70 kbit /s
                           4 - 7.40 kbit /s
                           5 - 7.95 kbit /s
                           6 - 10.2 kbit /s
                           7 - 12.2 kbit /s
                           0xFF - UNDEFINED  */
  boolean dl_dtx;       /* 1 - On, 0 - Off, default - 0 */
  uint16  dl_c_over_i;  /* range 0..63, 0xFFFF - Invalid */
} eng_mode_amr_info_t;


/***********************************************************************/
/************************ ENG_MODE_TBF_INFO ****************************/
/***********************************************************************/

typedef struct
{
  uint8 mac_mode;       /* As specified in 4.060 chapter 11.2.7.2
                           0    Dynamic Allocation
                           1    Extended Dynamic Allocation
                           0xff Invalid */
  uint8  tbf_mode;      /* 1  GPRS
                           2  EGPRS
                           0xff  Invalid */
}eng_mode_tbf_info_t;


/***********************************************************************/
/************************ ENG_MODE_NW_PARAMS ***************************/
/***********************************************************************/

typedef struct
{
  uint8 nc_mode;        /*    0  NC1 mode
                              1  NC2 mode
                              0xff Invalid */

  uint8 nmo;            /*    0  NMO I
                              1  NMO II
                              2  NMO III
                              0xff  invalid */

  uint8 msc_rev;        /* 0  REL 98 or older
                           1  REL 99 onwards
                           0xff invalid */  

  uint8 sgsn_rev;       /* 0  REL 98 or older
                           1  REL 99 onwards
                           0xff invalid */
} eng_mode_nw_params_t;

/***********************************************************************/
/************************ ENG_MODE_EDGE_MEAS ***************************/
/***********************************************************************/

typedef struct
{
  uint8 lq_meas_mode;   /* spec 4.60 chap 11.2.7.2 ref for detail
                           values 0 to 3. 0xff = invalid */
  uint8 bep_period;     /* spec ref 4.60 cha 12.24.1 and 5.08  chap 10.2.3.2.1
                           0xff = invalid */
}eng_mode_edge_meas_t;

/***********************************************************************/
/************************ ENG_MODE_SCELL_CONFIG ************************/
/***********************************************************************/

typedef struct
{
  uint8     pbcch_present;    /* 0=no, 1=yes, 0xff - invalid */
  uint8     gprs_rxlev_access_min; /*0..63, 0xff - invalid, see 45.008*/
  uint8     gprs_ms_txpwr_max_cch; /*0..31, 0xff - invalid, see 45.005, 45.008*/
} eng_mode_scell_config_t;

/********************************************************************************/
/************************ ENG_MODE_SIGNAL_INFO ************************/
/********************************************************************************/

/* These info structures are mirrored across all technlogoies, must be consistent with
   the structure to be defined in CM, hence the inclusion of non-GSM related fields. The
   GSM Eng Mode API is being used simply due it's existence as an exported API */


/* Max number of Rx chains for GSM*/
#define ENG_MODE_RX_CHAIN_NUM_MAX   1

/* The invalid signal info value */
#define ENG_MODE_SIGNAL_INFO_INVALID_VALUE   0x80000000

/* The invalid band and channel value */
#define ENG_MODE_BAND_INVALID_VALUE 0xFFFFFFFF
#define ENG_MODE_CHANNEL_INVALID_VALUE 0xFFFFFFFF

/* Macro for creating a bitmask by shifting '1' left by a
    Specified number indicated by 'val'.
*/
#define ENG_MODE_BM( val ) ( 1<< ((int)(val)) )

/* Enumeration of RX chains supported */
typedef enum
{
  ENG_MODE_GSM_RX_CHAIN_0 = 0,
  ENG_MODE_GSM_RX_CHAIN_1,
  ENG_MODE_GSM_RX_CHAIN_NUM_MAX
}eng_mode_rx_chain_t;

/* Enumeration of RX chain diversity info - for GSM only
   GSM_CHAIN_NUM_0 is applicable*/
typedef enum
{
  /* Signal info are not available on all RX chains */
  ENG_MODE_RX_CHAIN_UNAVAIL = 0,
  /* Signal info on ENG_MODE_RX_CHAIN_0 is available */
  ENG_MODE_RX_CHAIN_0_AVAIL = ENG_MODE_BM(ENG_MODE_GSM_RX_CHAIN_0),
  /* Signal info on ENG_MODE_RX_CHAIN_1 is available */
  ENG_MODE_RX_CHAIN_1_AVAIL = ENG_MODE_BM(ENG_MODE_GSM_RX_CHAIN_1),
  ENG_MODE_RX_CHAIN_ALL_AVAIL = ENG_MODE_BM(ENG_MODE_GSM_RX_CHAIN_0) |
                                ENG_MODE_BM(ENG_MODE_GSM_RX_CHAIN_1)
}eng_mode_rx_diversity_bit_mask_e;

/* Structure of RX signal info parameters per each RX chain */
typedef struct eng_mode_rx_chain_info_s
{
  /* Whether RX is tuned to a channel. If radio is tuned,
     instantenuous values will be set for below signal info
     fields.If radio is not tuned, delayed or invalid values
     will be set depending on each technology */
  boolean             is_radio_tuned;
  /* RX power value in 1/10 dbm resolution */
  int32               rx_pwr;
  /* not applicable for GSM*/
  int32               ecio;
  /* not applicable for GSM*/
  int32               rscp;
  /* not applicable for GSM*/
  int32               rsrp;
}eng_mode_rx_chain_info_t;

/* Structure of RX power related info */
typedef struct eng_mode_rx_pwr_info_s
{
  /* Diversity indicator to show which RX chain has valid
     signal info. Used to indicate RX power available info
     of below 2 fields */
  eng_mode_rx_diversity_bit_mask_e rx_diversity_indicator;
  /* Rx signal info */
  eng_mode_rx_chain_info_t    rx_pwr_chain[ENG_MODE_GSM_RX_CHAIN_NUM_MAX];
}eng_mode_rx_pwr_info_t;


/*
** Structure of TX power and Pa info
*/
typedef struct eng_mode_tx_pwr_pa_info_s {

  boolean             is_valid;
  eng_mode_sys_band_T band;          /* need to know band to calculate power in dBm */
  uint8               tx_lev;        /* Tx lev            31..0 */
  uint8               tx_pa;         /* Tx PA gain state        */
  uint8               tx_pwr;        /* Tx pwr is in dBm  0..33 */

/*
** CgC included only to allow backwards compatiblity until CMAPI release their changes
*/
  boolean             is_in_traffic;  /* CgC is nolonger used */

}eng_mode_tx_pwr_pa_info_t;


/* Structure to hold Rx/Tx signal information */
typedef struct eng_mode_signal_info_s{
  /* Active band */
  int32                       band;
  /* Active ARFCN */
  int32                       channel;
  /* Rx Power info */
  eng_mode_rx_pwr_info_t      rx_power;
  /* Tx Power info */
  eng_mode_tx_pwr_pa_info_t   tx_power;
}geran_eng_mode_signal_info_t;

/***********************************************************************/
/************************ ENG_MODE_CODEC_HO_INFO ***********************/
/***********************************************************************/

/*sys_codec_ho_info defined in sys_v.h*/

/***********************************************************************/
/************************ ENG_MODE_CODEC_HO_SAMPLE_RATE ****************/
/***********************************************************************/
typedef struct
{
  uint32 speech_enc_samp_freq; /* 8000: narrow-band or 16000: wide-band, 0 - unknown: ignore */
}geran_eng_mode_codec_ho_sample_rate_t;

/***********************************************************************/
/************************ ENG_MODE_CURRENT_L1_TIMESLOT *****************/
/***********************************************************************/
typedef struct
{
  uint8 timeslot; /*timeslot number 0..7, 0xff - invalid*/
} eng_mode_curr_l1_ts_t;

/***********************************************************************/
/************************ ENG_MODE_GSM_CIPHER_INFO *********************/
/***********************************************************************/
typedef enum
{
  ENG_MODE_GSM_CIPHER_NONE,
  ENG_MODE_GSM_CIPHER_A5_1,
  ENG_MODE_GSM_CIPHER_A5_2,
  ENG_MODE_GSM_CIPHER_A5_3,
  ENG_MODE_GSM_CIPHER_A5_4,
  ENG_MODE_GSM_CIPHER_A5_5,
  ENG_MODE_GSM_CIPHER_A5_6,
  ENG_MODE_GSM_CIPHER_A5_7
} geran_eng_mode_gsm_cipher_alg_t;

typedef struct
{
  geran_eng_mode_gsm_cipher_alg_t cipher_alg;
} geran_eng_mode_gsm_cipher_info_t;

/***********************************************************************/
/***************** ENG_MODE_GERAN_MODEM_STATS_INFO *********************/
/***********************************************************************/
typedef struct
{
  geran_eng_mode_info_bitmask_t  updated_info;

  /*ENG_MODE_TIMING_ADVANCE*/
  uint32                     timing_advance_value; /*0xffffffff - Invalid*/

  /*ENG_MODE_NMR_INFO*/
  eng_mode_nmr_info_t        nmr_info;

  /*ENG_MODE_RX_QUAL*/
  uint32                     rx_qual; /*0xffffffff - Invalid*/

  /*ENG_MODE_TX_PWR*/
  int16                      tx_pwr_in_use[ENG_MODE_TX_PWR_NUM_TIMESLOTS]; /*0 - Invalid*/

  /*ENG_MODE_SCELL_INFO*/
  eng_mode_scell_info_t      scell_info;

  /*ENG_MODE_SCELL_MEASUREMENTS*/
  eng_mode_scell_measurements_t scell_measurements;

  /*ENG_MODE_AMR_INFO*/
  eng_mode_amr_info_t        amr_info;

  /*ENG_MODE_DTX_UL_STATUS*/
  uint8                      dtx_ul_status; /*0 - Off, 1 - On, 0xff - Invalid*/

  /*ENG_MODE_TBF_INFO*/
  eng_mode_tbf_info_t        tbf_info;

  /*ENG_MODE_NW_PARAMS*/
  eng_mode_nw_params_t       nw_params;

  /*ENG_MODE_GPRS_UL_CS*/
  uint8                      gprs_ul_cs;  /* Range 0-3, 0xff - invalid */

  /*ENG_MODE_GPRS_DL_CS*/
  uint8                      gprs_dl_cs;  /* Range 0-3, 0xff - invalid */

  /*ENG_MODE_EGPRS_UL_MCS*/
  uint8                      egprs_ul_mcs; /* Range 0-8, 0xff - invalid */

  /*ENG_MODE_EGPRS_DL_MCS*/
  uint8                      egprs_dl_mcs; /* Range 0-8 , 0xff - invalid */

  /*ENG_MODE_EDGE_MEAS*/
  eng_mode_edge_meas_t       edge_meas;

  /*ENG_MODE_SCELL_CONFIG*/
  eng_mode_scell_config_t    scell_config;

  /*ENG_MODE_HOPPING_STATUS*/
  eng_mode_hopping_status_t  hopping_status;

  /*ENG_MODE_CURRENT_L1_TIMESLOT*/
  eng_mode_curr_l1_ts_t      current_timeslot;
} geran_eng_mode_modem_stats_t;

/***********************************************************************/
/********************* ENG_MODE_ALL_INFORMATION ************************/
/***********************************************************************/
/* Obsolete Feature*/
typedef struct
{
  geran_eng_mode_info_bitmask_t  updated_info;

  /*ENG_MODE_TIMING_ADVANCE*/
  uint32                     timing_advance_value;
  boolean                    timing_advance_valid_flag;

  /*ENG_MODE_RADIO_LINK_TIMEOUT_VALUE*/
  uint32                     radio_link_timeout_value;
  boolean                    radio_link_timeout_valid_flag;

  /*ENG_MODE_TOTAL_UPLINK_LLC_PDUS_TRANSMITTED*/
  uint32                     total_uplink_llc_pdus_transmitted;
  boolean                    total_uplink_llc_pdus_transmitted_valid_flag;

  /*ENG_MODE_TOTAL_DOWNLINK_LLC_PDUS_RECEIVED*/
  uint32                     total_downlink_llc_pdus_received;
  boolean                    total_downlink_llc_pdus_received_valid_flag;

  /*ENG_MODE_TOTAL_UPLINK_LLC_PDUS_RETRANSMITTED*/
  uint32                     total_uplink_llc_pdus_retransmitted;
  boolean                    total_uplink_llc_pdus_retransmitted_valid_flag;

  /*ENG_MODE_TOTAL_DOWNLINK_LLC_PDUS_RETRANSMITTED*/
  uint32                     total_downlink_llc_pdus_retransmitted;
  boolean                    total_downlink_llc_pdus_retransmitted_valid_flag;

  /*ENG_MODE_TOTAL_UPLINK_RLC_BLOCKS_TRANSMITTED*/
  uint32                     total_uplink_rlc_blocks_transmitted;
  boolean                    total_uplink_rlc_blocks_transmitted_valid_flag;

  /*ENG_MODE_TOTAL_DOWNNLINK_RLC_BLOCKS_RECEIVED*/
  uint32                     total_downlink_rlc_blocks_received;
  boolean                    total_downlink_rlc_blocks_received_valid_flag;

  /*ENG_MODE_TOTAL_UPLINK_RLC_BLOCKS_RETRANSMITTED*/
  uint32                     total_uplink_rlc_blocks_retransmitted;
  boolean                    total_uplink_rlc_blocks_retransmitted_valid_flag;

  /*ENG_MODE_TOTAL_DOWNLINK_REPEATED_RLC_BLOCKS_RECEIVED*/
  uint32                     total_downlink_repeated_rlc_blocks_received;
  boolean                    total_downlink_repeated_rlc_blocks_received_valid_flag;

  /*ENG_MODE_HOPPING_FREQUENCY_LIST*/
  eng_mode_frequency_list_T  hopping_freq_list;
  boolean                    hopping_freq_list_valid_flag;

  /*ENG_MODE_RX_QUAL*/
  uint32                     rx_qual;
  boolean                    rx_qual_valid_flag;

  /*ENG_MODE_GMSK_CV_VALUE*/
  uint32                     gmsk_cv_value;
  boolean                    gmsk_cv_value_valid_flag;

  /*ENG_MODE_PSK_CV_VALUE*/
  uint32                     psk_cv_value;
  boolean                    psk_cv_value_valid_flag;

  /*ENG_MODE_SCELL_INFO*/
  eng_mode_scell_info_t      scell_info;
  boolean                    scell_info_valid_flag;

  /*ENG_MODE_SCELL_MEASUREMENTS*/
  eng_mode_scell_measurements_t scell_measurements;
  boolean                       scell_measurements_valid_flag;

  /*ENG_MODE_TX_PWR*/
  int16                      tx_pwr_in_use[ENG_MODE_TX_PWR_NUM_TIMESLOTS];
  boolean                    tx_pwr_in_use_valid_flag;

  /*ENG_MODE_NMR_INFO*/
  eng_mode_nmr_info_t        nmr_info;
  boolean                    nmr_info_valid_flag;

  /*ENG_MODE_AMR_INFO*/
  eng_mode_amr_info_t        amr_info;
  boolean                    amr_info_valid_flag;

  /*ENG_MODE_DTX_UL_STATUS*/
  uint8                      dtx_ul_status;
  boolean                    dtx_ul_status_valid_flag;

  /*ENG_MODE_TBF_INFO*/
  eng_mode_tbf_info_t        tbf_info;
  boolean                    tbf_info_valid_flag;

  /*ENG_MODE_NW_PARAMS*/
  eng_mode_nw_params_t       nw_params;
  boolean                    nw_params_valid_flag;

  /*ENG_MODE_GPRS_UL_CS*/
  uint8                      gprs_ul_cs;
  boolean                    gprs_ul_cs_valid_flag;

  /*ENG_MODE_GPRS_DL_CS*/
  uint8                      gprs_dl_cs;
  boolean                    gprs_dl_cs_valid_flag;

  /*ENG_MODE_EGPRS_UL_MCS*/
  uint8                      egprs_ul_mcs;
  boolean                    egprs_ul_mcs_valid_flag;

  /*ENG_MODE_EGPRS_DL_MCS*/
  uint8                      egprs_dl_mcs;
  boolean                    egprs_dl_mcs_valid_flag;

  /*ENG_MODE_EDGE_MEAS*/
  eng_mode_edge_meas_t       edge_meas;
  boolean                    edge_meas_valid_flag;

  /*ENG_MODE_SCELL_CONFIG*/
  eng_mode_scell_config_t    scell_config;
  boolean                    scell_config_valid_flag;

  /*ENG_MODE_HOPPING_STATUS*/
  eng_mode_hopping_status_t  hopping_status;
  boolean                    hopping_status_valid_flag;

  /* ENG_MODE_SIGNAL_INFO*/
  geran_eng_mode_signal_info_t signal_info;
  boolean                      signal_info_valid_flag;

  /* ENG_MODE_TX_PWR_PA_INFO */
  eng_mode_tx_pwr_pa_info_t   tx_pwr_pa_info;
  boolean                     tx_pwr_pa_info_valid_flag;

  /*ENG_MODE_CODEC_HO_INFO*/
  sys_codec_ho_info          ho_codec;
  boolean                    ho_codec_valid_flag;

  /*ENG_MODE_CURRENT_L1_TIMESLOT*/
  eng_mode_curr_l1_ts_t      current_timeslot;
  boolean                    current_timeslot_valid_flag;

  /* ENG_MODE_CANDIDATE_PLMN_ID */
  eng_mode_plmn_id_t         candidate_plmn_id;
  boolean                    candidate_plmn_valid_flag;

  /*ENG_MODE_GSM_CIPHER_INFO*/
  geran_eng_mode_gsm_cipher_info_t gsm_cipher;
  boolean                          gsm_cipher_valid_flag;
}geran_eng_mode_info_t;

/***********************************************************************/
/********************** Call back types ********************************/
/***********************************************************************/

/*function ptr type for callback functions used in geran_eng_mode_register_callback*/

/* ENG_MODE_SCELL_INFO */
typedef void (*scell_info_cb_fnc_ptr)(eng_mode_scell_info_t scell_info);

/* ENG_MODE_NMR_INFO */
typedef void (*nmr_info_cb_fnc_ptr)(eng_mode_nmr_info_t nmr_info);

/* ENG_MODE_CODEC_HO_INFO*/
typedef void (*codec_ho_cb_fnc_ptr)(sys_codec_ho_info codec_ho_info);

/* ENG_MODE_SCELL_MEASUREMENTS */
typedef void (*scell_meas_cb_fnc_ptr)(eng_mode_scell_measurements_t scell_meas);

/* ENG_MODE_CANDIDATE_PLMN_ID */
typedef void (*candidate_plmn_cb_fnc_ptr)(eng_mode_plmn_id_t plmn_id);

typedef struct
{
  boolean always_report;
} geran_eng_mode_cb_behaviour_t;

/***********************************************************************/
/********************** Public Function Definitions** ******************/
/***********************************************************************/

/*==========================================================================

  FUNCTION    geran_eng_mode_data_read

  DESCRIPTION

    This function is intended to read the eng mode value from public store. This function will give
    requested eng mode info value if it is valid otherwise it will return the invalid value.


  PARAMETERS

    geran_eng_mode_info_type:  geran_eng_mode_info_type_t type
    param_copy            :   void

  DEPENDENCIES
    None.

  RETURN VALUE
    void

  SIDE EFFECTS
    None.

===========================================================================*/
void geran_eng_mode_data_read(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                              void *param_copy);

/*==========================================================================

  FUNCTION    geran_multisim_eng_mode_data_read

  DESCRIPTION

    This function is intended to read the eng mode value from public store using the as_subsid. This function will give
    requested eng mode info value if it is valid otherwise it will return the invalid value.


  PARAMETERS

    geran_eng_mode_info_type:  geran_eng_mode_info_type_t type
    param_copy            :   void
    asubs_id              :   Access Stratum ID
 

  DEPENDENCIES
    None.

  RETURN VALUE
    void

  SIDE EFFECTS
    None.

===========================================================================*/
void geran_multisim_eng_mode_data_read(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                       void *param_copy, 
                                       sys_modem_as_id_e_type asubs_id);

/*===========================================================================

FUNCTION geran_eng_mode_register_callback

DESCRIPTION

  This function enables upper layers to register a callback function.
  Once the function is registered, it will be called whenever there is a change in value.
  MAX_NUM_CALLBACK_REGISTRATIONS indicates the maximum number of callbacks per info type.


PARAMETERS

  geran_eng_mode_info_type - a callback is registered per info type.
  callback_func_ptr - pointer to function that requires registration, see Call back Function prototypes.

RETURN VALUE
  Boolean - TRUE - registration worked
            FALSE - registration failed.

SIDE EFFECTS
  reads callback_supported
  write to eng_mode_cb_funcs

===========================================================================*/

boolean geran_eng_mode_register_callback(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                         void *callback_func_ptr);
									   
									   
/*===========================================================================

FUNCTION geran_multisim_eng_mode_register_callback

DESCRIPTION

  This function enables upper layers to register a callback function.
  Once the function is registered, it will be called whenever there is a change in value.
  MAX_NUM_CALLBACK_REGISTRATIONS indicates the maximum number of callbacks per info type.


PARAMETERS

  geran_eng_mode_info_type - a callback is registered per info type.
  callback_func_ptr - pointer to function that requires registration, see Call back Function prototypes.
  asubs_id - Access Stratum ID


RETURN VALUE
  Boolean - TRUE - registration worked
            FALSE - registration failed.

SIDE EFFECTS
  reads callback_supported
  write to eng_mode_cb_funcs

===========================================================================*/
boolean geran_multisim_eng_mode_register_callback(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                                  void *callback_func_ptr,
                                                  sys_modem_as_id_e_type asubs_id);

/*===========================================================================

FUNCTION geran_eng_mode_deregister_callback

DESCRIPTION

  This function enables upper layers to deregister a callback function.


PARAMETERS

  geran_eng_mode_info_type - a callback is registered per info type.
  callback_func_ptr - pointer to function that requires deregistration, see Call back Function prototypes.

RETURN VALUE
  Boolean - TRUE - deregistration worked
            FALSE - deregistration failed.

SIDE EFFECTS
  write to eng_mode_cb_funcs

===========================================================================*/
boolean geran_eng_mode_deregister_callback(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                           void *callback_func_ptr);
										   
/*===========================================================================

FUNCTION geran_multisim_eng_mode_deregister_callback

DESCRIPTION

  This function enables upper layers to deregister a callback function.


PARAMETERS

  geran_eng_mode_info_type - a callback is registered per info type.
  callback_func_ptr - pointer to function that requires deregistration, see Call back Function prototypes.
  asubs_id - Access stratum subs ID


RETURN VALUE
  Boolean - TRUE - deregistration worked
            FALSE - deregistration failed.

SIDE EFFECTS
  write to eng_mode_cb_funcs

===========================================================================*/

boolean geran_multisim_eng_mode_deregister_callback(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                                    void *callback_func_ptr,
                                                    sys_modem_as_id_e_type asubs_id);

/*===========================================================================

FUNCTION geran_eng_mode_modify_callback_behaviour

DESCRIPTION

  This function enables upper layers to alter the behaviour of a registered callback.


PARAMETERS

  geran_eng_mode_info_type - info type used for registration
  callback_func_ptr - pointer to registered function
  cb_behaviour - updated cb behaviour

RETURN VALUE
  Boolean - TRUE - cb behaviour update worked
            FALSE - cb behaviour update failed.

SIDE EFFECTS

===========================================================================*/
boolean geran_eng_mode_modify_callback_behaviour(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                                 void *callback_func_ptr,
                                                 geran_eng_mode_cb_behaviour_t cb_behaviour);
													
													
/*===========================================================================

FUNCTION geran_multisim_eng_mode_modify_callback_behaviour

DESCRIPTION

  This function enables upper layers to alter the behaviour of a registered callback.


PARAMETERS

  geran_eng_mode_info_type - info type used for registration
  callback_func_ptr - pointer to registered function
  cb_behaviour - updated cb behaviour
  asubs_id - Access Stratum ID         


RETURN VALUE
  Boolean - TRUE - cb behaviour update worked
            FALSE - cb behaviour update failed.

SIDE EFFECTS

===========================================================================*/
boolean geran_multisim_eng_mode_modify_callback_behaviour(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                                          void *callback_func_ptr,
                                                          geran_eng_mode_cb_behaviour_t cb_behaviour,
                                                          sys_modem_as_id_e_type asubs_id);

/*===========================================================================

FUNCTION geran_eng_mode_set_modem_stats_mask

DESCRIPTION

  Sets the current bitmask for reporting data changes (bitmask of zero means that no changes
  are reported) for ENG_MODE_GERAN_MODEM_STATS_INFO.  The bitmask positions are defined in
  geran_eng_mode_info_type_t.


PARAMETERS

  bitmask
  asubs_id


RETURN VALUE
  None

SIDE EFFECTS
  writes to internal bitmask stored variable.

===========================================================================*/
void geran_eng_mode_set_modem_stats_mask(geran_eng_mode_info_bitmask_t bitmask);

														  
/*===========================================================================

FUNCTION geran_multisim_eng_mode_set_modem_stats_mask

DESCRIPTION

  Sets the current bitmask for reporting data changes (bitmask of zero means that no changes
  are reported) for ENG_MODE_GERAN_MODEM_STATS_INFO.  The bitmask positions are defined in
  geran_eng_mode_info_type_t.


PARAMETERS

  bitmask
  asubs_id


RETURN VALUE
  None

SIDE EFFECTS
  writes to internal bitmask stored variable.

===========================================================================*/
void geran_multisim_eng_mode_set_modem_stats_mask(geran_eng_mode_info_bitmask_t bitmask, sys_modem_as_id_e_type asubs_id);


#endif /* GERAN_ENG_MODE_READ_API_H */

/* EOF */


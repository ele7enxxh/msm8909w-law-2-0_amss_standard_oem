#ifndef RR_PUBLIC_STORE_H
#define RR_PUBLIC_STORE_H

/*! \file rr_public_store.h
 
  This is the header file for rr_public_store.c.
  The Public Store  This module contains SIM data communicated from NAS.
 
                Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/inc/rr_public_store.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_l2_l1.h"
#include "csn.h"
#include "geran_dual_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*
**  Change Mark
*/

#define RRPS_CHANGE_MARK_NOT_VALID  0x0f

typedef uint8 rrps_change_mark_t;  /* RRPS_CHANGE_MARK */


/*
**  Mobile Allocations
*/

/* Mobile Allocation frequency list valid bitmap */
/* bit 0->15 references MA_NUMBER 0->15. 1-list present; 0-list not present */
typedef uint16 rrps_ma_bitmap_t;  /* RRPS_VALID_MA_BIT_MAP */

/* Mobile Allocation frequency lists */
/* N.B. Accessed using dedicated function: rr_read_public_store_freq_list() */
typedef struct
{
  ARFCN_T  ma_number[64];
  byte     hsn;
  byte     list_length;

} rrps_frequency_list_t;


/*
**  GPRS Cell Options
*/

typedef enum
{           /* ref 04.60 12.24   */
  EIGHT_BIT = 0,    /* 0 = 8-bit format  */
  ELEVEN_BIT = 1    /* 1 = 11 bit format */

} access_burst_t;

typedef enum
{                      /* ref 04.60 12.24           */
  ACCESS_BURST = 0,    /* 0 = four access bursts    */
  NORMAL_BURST = 1     /* 1 = RLC/MAC control block */

} control_ack_type_t;

typedef struct
{                              /* RRPS_CELL_OPTIONS */
  uint8  t3168;                /* RRPS_CELL_OPTIONS_T3168 */
  uint8  t3192;                /* RRPS_CELL_OPTIONS_T3192 */
  uint8  bs_cv_max;            /* RRPS_CELL_OPTIONS_BS_CV_MAX */
  uint8  pan_dec;              /* RRPS_CELL_OPTIONS_PAN_DEC */
  uint8  pan_inc;              /* RRPS_CELL_OPTIONS_PAN_INC */
  uint8  pan_max;              /* RRPS_CELL_OPTIONS_PAN_MAX */
  uint8  access_burst_type;    /* RRPS_CELL_OPTIONS_ACCESS_BURST_TYPE */
  uint8  control_ack_type;     /* RRPS_CELL_OPTIONS_CONTROL_ACK_TYPE */

  boolean egprs_capable_cell;     /* RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL */
  boolean egprs_pkt_chan_req;     /* RRPS_CELL_OPTIONS_EGPRS_PKT_CHAN_REQ */

  boolean nw_ext_ul_tbf;
  boolean ext_utbf_nodata;

  boolean pfc_feature_mode; /* RRPS_CELL_OPTIONS_PFC_FEATURE_MODE */

} rrps_gprs_cell_options_t;


/*
**  PRACH Control Parameters
*/

typedef struct
{                                /* RRPS_PRACH_CTRL */
  uint16  acc_contr_class;       /* RRPS_PRACH_CTRL_ACC_CONTR_CLASS */
  uint8   max_retrans[4];        /* RRPS_PRACH_CTRL_MAX_RETRANS */
  uint8   s;                     /* RRPS_PRACH_CTRL_S */
  uint8   tx_int;                /* RRPS_PRACH_CTRL_TX_INT */
  uint8   persistence_level[4];  /* RRPS_PRACH_CTRL_PERSISTANCE_LEVEL */

} rrps_prach_control_params_t;


/*
**  Global Power Control Parameters
*/

typedef struct
{                                       /* RRPS_POWER_CTRL */
  uint8    alpha;                       /* RRPS_POWER_CTRL_ALPHA */
  uint8    t_avg_w;                     /* RRPS_POWER_CTRL_T_AVG_W */
  uint8    t_avg_t;                     /* RRPS_POWER_CTRL_T_AVG_T */
  uint8    pb;                          /* RRPS_POWER_CTRL_PB */
  boolean  pc_meas_chan;                /* RRPS_POWER_CTRL_PC_MEAS_CHAN */
  boolean  int_meas_channel_list_avail; /* RRPS_POWER_CTRL_INT_MEAS_CHAN_LIST_AV */
  byte     n_avg_i;                     /* RRPS_POWER_CTRL_N_AVG_I */

} rrps_global_power_ctrl_params_t;


/*
**  Data from GMM
*/

typedef enum
{
  GMM_IDLE,
  GMM_STANDBY,
  GMM_READY

} gmm_state_t;

#define TMSI_PTMSI_INVALID    0xffffffff  /* All 32 bits '1' */

typedef struct
{
  uint16  ms_ra_cap_ie_len_bits;
  uint8   ms_ra_cap_ie[MAX_MS_RA_CAP_IE_LEN];

} rrps_ms_ra_cap_ie_t;

typedef struct
{                                                      /* RRPS_GMM_DATA */
  gmm_state_t          gmm_state;                      /* RRPS_GMM_STATE */
  boolean              ready_timer_force_to_standby;   /* RRPS_READY_TIMER_FORCE_TO_STANDBY */
  uint32               tmsi;                           /* RRPS_TMSI */
  uint32               ptmsi;                          /* RRPS_PTMSI */
  uint32               tlli;                           /* RRPS_TLLI */
  uint32               tlli_old;                       /* RRPS_TLLI_OLD */
  rrps_ms_ra_cap_ie_t  ms_ra_cap_ie;                   /* RRPS_MS_RA_CAP_IE */

} rrps_gmm_data_t;


/*
**  Data from the SIM
*/

/* N.B. IMSI formatted as 04.08 10.5.1.4 (using IMSI) */
#define RRPS_SIM_IMSI_SIZE  8
typedef struct
{                              /* RRPS_SIM_DATA */
  byte    imsi[RRPS_SIM_IMSI_SIZE];   /* RRPS_SIM_IMSI */

} rrps_sim_data_t;


/*
**  MS-specific data (including NV data)
*/

#define RRPS_MULTISLOT_CLASS_1   1
#define RRPS_MULTISLOT_CLASS_2   2
#define RRPS_MULTISLOT_CLASS_3   3
#define RRPS_MULTISLOT_CLASS_4   4
#define RRPS_MULTISLOT_CLASS_5   5
#define RRPS_MULTISLOT_CLASS_6   6
#define RRPS_MULTISLOT_CLASS_7   7
#define RRPS_MULTISLOT_CLASS_8   8
#define RRPS_MULTISLOT_CLASS_9   9
#define RRPS_MULTISLOT_CLASS_10  10
#define RRPS_MULTISLOT_CLASS_11  11
#define RRPS_MULTISLOT_CLASS_12  12

#ifdef FEATURE_GSM_DTM
#define RRPS_DTM_EGPRS_MULTISLOT_SUBCLASS_UNUSED   0
#define RRPS_DTM_EGPRS_MULTISLOT_SUBCLASS_5        1
#define RRPS_DTM_EGPRS_MULTISLOT_SUBCLASS_9        2
#define RRPS_DTM_EGPRS_MULTISLOT_SUBCLASS_11       3
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GSM_GPRS_MSC33
#define RRPS_MULTISLOT_CLASS_30  30
#define RRPS_MULTISLOT_CLASS_31  31
#define RRPS_MULTISLOT_CLASS_32  32
#define RRPS_MULTISLOT_CLASS_33  33
#endif /* FEATURE_GSM_GPRS_MSC33 */

typedef struct
{
  uint8    non_drx_timer;
  uint16   split_page_cycle;
  uint8    multislot_class;
  uint8    anite_gcf_flag;
  uint8    ms_bcch_band;
  uint8    egprs_multislot_class;
  boolean  egprs_8psk_ul_enabled;
#ifdef FEATURE_GSM_DTM
  boolean  dtm_enabled;
  uint8    dtm_egprs_multislot_subclass;
#ifdef FEATURE_GSM_EDTM
  boolean  edtm_enabled;
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
  uint8    gea_supported;
#ifdef FEATURE_GSM_GPRS_MSC33
  uint8    hmsc_gprs_coded_ms_class;
  uint8    hmsc_egprs_coded_ms_class;
#endif /* FEATURE_GSM_GPRS_MSC33 */
  uint8  egprs_8psk_power_capability;
} rrps_ms_data_t;


/*
**  Parameter Identifiers
*/

/* The identifier are used to retrieve structures or individual members of structures */
/* from the Public Store (except the Mobile Allocation frequency lists) */
typedef enum
{
/* Change Marks */
  RRPS_CHANGE_MARK_SI13,

/* Mobile Allocation valid bitmap */
  RRPS_VALID_MA_BIT_MAP,

/* GPRS Cell Options */
  RRPS_CELL_OPTIONS_T3168,
  RRPS_CELL_OPTIONS_T3192,
  RRPS_CELL_OPTIONS_BS_CV_MAX,
  RRPS_CELL_OPTIONS_PAN_DEC,
  RRPS_CELL_OPTIONS_PAN_INC,
  RRPS_CELL_OPTIONS_PAN_MAX,
  RRPS_CELL_OPTIONS_ACCESS_BURST_TYPE,
  RRPS_CELL_OPTIONS_CONTROL_ACK_TYPE,

  RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL,
  RRPS_CELL_OPTIONS_EGPRS_PKT_CHAN_REQ,

  RRPS_CELL_OPTIONS_NW_EXT_UL_TBF,
  RRPS_CELL_OPTIONS_EXT_UTBF_NODATA,

  RRPS_CELL_OPTIONS_PFC_FEATURE_MODE,

/* Global Power Control Params */
  RRPS_POWER_CTRL_ALPHA,
  RRPS_POWER_CTRL_T_AVG_W,
  RRPS_POWER_CTRL_T_AVG_T,
  RRPS_POWER_CTRL_PB,
  RRPS_POWER_CTRL_PC_MEAS_CHAN,
  RRPS_POWER_CTRL_INT_MEAS_CHAN_LIST_AV,
  RRPS_POWER_CTRL_N_AVG_I,

/* SIM Data */
  RRPS_SIM_IMSI,

/* GMM Data */
  RRPS_GMM_STATE,
  RRPS_READY_TIMER_FORCE_TO_STANDBY,
  RRPS_TMSI,
  RRPS_PTMSI,
  RRPS_TLLI,
  RRPS_TLLI_OLD,
  RRPS_MS_RA_CAP_IE,

/* MS Data */
  RRPS_MS_NON_DRX_TIMER,
  RRPS_MS_SPLIT_PAGE_CYCLE,
  RRPS_MS_MULTISLOT_CLASS,
  RRPS_MS_ANITE_GCF_FLAG,
  RRPS_MS_BCCH_BAND,
  RRPS_MS_EGPRS_MULTISLOT_CLASS,
  RRPS_MS_EGPRS_8PSK_UL_ENABLED,
#ifdef FEATURE_GSM_DTM
  RRPS_MS_DTM_ENABLED,
  RRPS_MS_DTM_EGPRS_MULTISLOT_SUBCLASS,
#ifdef FEATURE_GSM_EDTM
  RRPS_MS_EDTM_ENABLED,
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
  RRPS_MS_GPRS_GEA_SUPPORT,
#ifdef FEATURE_GSM_GPRS_MSC33
  RRPS_MS_HMSC_GPRS_CODED_MS_CLASS,
  RRPS_MS_HMSC_EGPRS_CODED_MS_CLASS,
#endif /* FEATURE_GSM_GPRS_MSC33 */

  RRPS_PRIORITY_ACCESS_THR,

  RRPS_NW_BAND_IND,

  NUMBER_OF_RRPS_PARAMS,

  RRPS_CELL_OPTIONS,
  RRPS_POWER_CTRL,
  RRPS_SIM_DATA,
  RRPS_GMM_DATA,
  RRPS_MS_DATA,
  RRPS_MS_EGPRS_8PSK_POWER_CAP,

  OVERFLOW_CHECK_FOR_RRPS_ID

} rr_public_store_id_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief This function is used to access parameters in the Public Store. 
 *  
 * \param param_id (in) - parameter identifier 
 * \param param_copy (out) - updated with the value of the parameter
 * \param gas_id
 */
extern void rr_read_public_store(
  rr_public_store_id_t param_id,
  void *param_copy,
  gas_id_t gas_id
);

/*!
 * \brief This function is used to access the Mobile Allocation frequency lists.
 *  
 * \param ma_number (in) - MA_NUMBER_14 or MA_NUMBER_15
 * \param gas_id
 *
 * \return rrps_frequency_list_t* 
 */
extern rrps_frequency_list_t *rr_read_public_store_freq_list(
  uint8 ma_number,
  gas_id_t gas_id
);

/*!
 * \brief This function decodes a Mobile Allocation freq list from a packet assignment received by MAC. 
 *  
 * This freq list is stored and can be accessed using MA_NUMBER=15. 
 * An optional comparison can be performed against the last freq list stored by this method. 
 * If a comparison is not required, 'compare_ptr' can be set to NULL, or the flag it points to FALSE. 
 * If a comparison is required, 'compare_ptr' should pointer to a flag set TRUE. In this case, the result 
 * of the comparison is written to this flag: TRUE if the freq list is the same; FALSE if they are different.
 *  
 * \param type (in) - format of the supplied data. Should be 'direct1_valid' or 'direct2_valid'
 * \param data (in) - data structure of type either 'direct1_enc_t' or 'direct2_enc_t'
 * \param compare_ptr (in) - comparison required flag / comparison result
 * \param gas_id
 *  
 * \return rrps_frequency_list_t* - decoded freq list (NULL if an error occurred during decoding)
 */
extern rrps_frequency_list_t *rr_packet_assignment_mobile_alloc(
  encoding_type_e type,
  void *data,
  boolean *compare_ptr,
  gas_id_t gas_id
);

/*!
 * \brief Outputs Public Store data content to diag.
 *
 * \param gas_id
 */
extern void rr_public_store_dump_to_diag(
  const gas_id_t gas_id
);

#ifdef DEBUG_NO_RR
#error code not present
#endif // DEBUG_NO_RR

#endif  /* RR_PUBLIC_STORE_H */

/* EOF */


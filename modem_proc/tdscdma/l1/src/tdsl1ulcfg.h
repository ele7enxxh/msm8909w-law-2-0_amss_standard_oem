#ifndef TDSL1_UL_CFG_H
#define TDSL1_UL_CFG_H

/*============================================================================
              U P L I N K   C F G   F I L E

DESCRIPTION
  This files contains all necessary macros and definitions for maintainaing
  physical channel configuration database and CCTrCH configuration database.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) 2000-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsl1ulcfg.h#1 $ 
  $DateTime: 2016/12/13 07:58:19 $ 
  $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/23/00    cdf     file created.

===========================================================================*/

/* <EJECT>^L */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "queue.h"
#include "tdsl1sapcommon.h"
#include "tdsmacl1rrcif.h"
#include "tdsl1rrcif.h"
#include "tfw_sw_intf_msg.h"
#ifdef FEATURE_TDSCDMA_HSUPA
#include "tdseulcfg.h"
#endif


/*===========================================================================

                     MACRO & CONST DEFINITIONS

===========================================================================*/
/* stats bin upper bounds*/
#define TDSL1_UL_PWR_BIN_1_UPPER_BOUND  0   
#define TDSL1_UL_PWR_BIN_2_UPPER_BOUND  (8 <<4)
#define TDSL1_UL_PWR_BIN_3_UPPER_BOUND  (15<<4)
#define TDSL1_UL_PWR_BIN_4_UPPER_BOUND  (20<<4)
/* cvt from ts to ms: 1 ts =  864(5ms/6400) = 0.675ms, cvt TS to ms */
#define TDSL1_UL_TS_TO_MS 0.675

/* maximum number of CCTrCH configurations maintained */
#define TDS_UL_CFG_MAX                          2

/* Define the actions that performed for updating uplink slot usage via MEAS's API.
 * Some of those actions may be performed at one time, so they are defined as bitmask 
 */
#define TDSL1_UL_MEAS_SU_NONE                   0x0000  /* Don't need update slotusage */
#define TDSL1_UL_MEAS_SU_DPCH_SET               0x0001  /* To set or update DPCH slotusage */
#define TDSL1_UL_MEAS_SU_UPPCH_FPACH_SET        0x0002  /* To set or update UPPCH_FPACH slotusage */
#define TDSL1_UL_MEAS_SU_PRACH_SET              0x0004  /* To set or update PRACH slotusage */
#define TDSL1_UL_MEAS_SU_ERUCCH_SET             0x0008  /* To set or update ERUCCH slotusage */
#define TDSL1_UL_MEAS_SU_DPCH_CLEAR             0x0010  /* To clear DPCH slotusage */
#define TDSL1_UL_MEAS_SU_UPPCH_FPACH_CLEAR      0x0020  /* To clear UPPCH_FPACH slotusage */
#define TDSL1_UL_MEAS_SU_PRACH_CLEAR            0x0040  /* To clear PRACH slotusage */
#define TDSL1_UL_MEAS_SU_ERUCCH_CLEAR           0x0080  /* To clear ERUCCH slotusage */



/* --------------------------------------------- */
/* HSDPA UL physical channel related information */
/* --------------------------------------------- */


#ifdef FEATURE_HSDPA_PERFORMANCE_OPTIMIZATION

#define HS_DPCCH_BETA_HS_INFO_PRESENT_BITMASK 0x01
#define HS_DPCCH_MEAS_FEEDBACK_INFO_PRESENT_BITMASK 0x02

/* + defang 02/11/2011: commented from original code */
#if 0
/* This structure defines the information related to
   BetaHS fields of DPCCH channel */
typedef struct
{
  /* delta CQI (0..8) */
  uint8 delta_cqi;
  /* delta ACK (0..8) */
  uint8 delta_ack;
  /* delta NACK (0..8) */
  uint8 delta_nack;
} tdsl1ulcfg_hs_dpcch_beta_hs_info_struct_type;

/* This structure defines the information related to
   measurement feedback fields of UL HS DPCCH channel */
typedef struct
{
  /* Power offset between CPICH and HS PDSCH
     Range is from -6 to 13 in units of 0.5 dB
     Values ranges from -12 to 26 */
  int8 po_hs_dsch;
  /* CQI feedback cycle count (0..160 ms) */
  uint8 cqi_feedback_cycle;
  /* CQI repetition factor (1..4) */
  uint8 cqi_rep_factor;
  /* ACK-NACK repetition factor (1..4) */
  uint8 ack_nack_rep_factor;
 #ifdef FEATURE_HSDPA_HARQ_PREAMBLE
  /* HARQ Preamble Mode (on/off) */
  boolean harq_pmbl_mode;
 #endif
} tdsl1ulcfg_hs_dpcch_meas_feedback_info_struct_type;
 
/* This structure defines the information related to
   UL HS DPCCH channel */
typedef struct
{
  /* Bit mask indicating which information is present */
  uint8 info_present_bitmask;
 
  /* Beta HS information */
  tdsl1ulcfg_hs_dpcch_beta_hs_info_struct_type beta_hs_info;
 
  /* Measurement fedback related information */
  tdsl1ulcfg_hs_dpcch_meas_feedback_info_struct_type meas_feedback_info;
} tdsl1ulcfg_hs_dpcch_info_struct_type;
#endif
/* - defang 02/11/2011 */

#else

/* + defang 02/11/2011: commented from original code */
#if 0
/* This structure defines the information related to
   UL HS DPCCH channel */
typedef struct
{
  /* Power offset between CPICH and HS PDSCH
     Range is from -6 to 13 in units of 0.5 dB
     Values ranges from -12 to 26 */
  int8 po_hs_dsch;
  /* CQI feedback cycle count (0..160 ms) */
  uint8 cqi_feedback_cycle;
  /* CQI repetition factor (1..4) */
  uint8 cqi_rep_factor;
  /* delta CQI (0..8) */
  uint8 delta_cqi;
  /* delta ACK (0..8) */
  uint8 delta_ack;
  /* delta NACK (0..8) */
  uint8 delta_nack;
  /* ACK-NACK repetition factor (1..4) */
  uint8 ack_nack_rep_factor;
 #ifdef FEATURE_HSDPA_HARQ_PREAMBLE
  /* HARQ Preamble Mode (on/off) */
  boolean harq_pmbl_mode;
 #endif
} tdsl1ulcfg_hs_dpcch_info_struct_type;
#endif
/* - defang 02/11/2011 */

#endif /* FEATURE_HSDPA_PERFORMANCE_OPTIMIZATION */


/* Semistatic parameters */
/* TTI */
#define TDS_UL_CCTRCH_TTI(trch) (tdsl1_ul_tfcs_ptr->tfs_info[trch].tti_info)
/* CRC */
#define TDS_UL_CCTRCH_CRC(trch) (tdsl1_ul_curr_cctrch_ptr->semi_static_parms_ptr[trch]->crc)
/* Gain factor */
#define TDS_UL_CCTRCH_GF_BETAD(tfc_id) (tdsl1_ul_curr_cctrch_ptr->gain_factor_parms_ptr[tfc_id]->BetaD)

/* Some useful macros to access CCTrCH items */
/* Semistatic parameters */
  /* Coding Type */
#define TDS_UL_CCTRCH_CODING_TYPE(trch) \
  (tdsl1_ul_curr_cctrch_ptr->semi_static_parms_ptr[trch]->coding)
  /* Static RM */
#define TDS_UL_CCTRCH_STATIC_RM(trch) \
  (tdsl1_ul_curr_cctrch_ptr->semi_static_parms_ptr[trch]->staticRM)

/* Gain Factors */
  /* GF calculation type */
#define TDS_UL_CCTRCH_GF_CALC_TYPE(tfc_id) \
  (tdsl1_ul_curr_cctrch_ptr->gain_factor_parms_ptr[tfc_id]->algo)

/*===========================================================================

                     STRUCTURE DEFINITIONS

===========================================================================*/

/* TrCH state in a TFC */
typedef enum {
  /* TrCH active */
  TDSL1_UL_TRCH_ACTIVE,
  /* TrCH not active */
  TDSL1_UL_TRCH_INACTIVE
} tdsl1_ul_trch_state_enum_type;

typedef enum
{
  TDS_UL_DPCH_STATE_INACTIVE,
  TDS_UL_DPCH_STATE_READY,
  TDS_UL_DPCH_STATE_SB,
  TDS_UL_DPCH_STATE_TX,
  TDS_UL_DPCH_STATE_NO_TX,
  TDS_UL_DPCH_STATE_DISABLED
} tdsl1_ul_dpch_state_enum_type;

/* Bit masks for available SYNC_UL codes and sub-channels */
typedef struct {
  /* bit mask of available SYNC_UL codes */
  uint8     avail_sync_code;
  /* sub-channel size */
  uint8     sub_chan_size;
  /* bit mask of available sub-channels */
  uint8     avail_sub_chan;
} tdsl1_ul_asc_avail_mask_struct_type;

/* Pre-RM bits and Filler bits for the <TrCH Semi-Static parms, TFC>
 * will be computed every time there is data to be transmitted. This
 * can be optimized so that we store them for every TFC in which case
 * we do not need to re-calculate them each time. It is FFS if we need
 * this optimization.
 */ 
typedef struct {
  /* number of code segments */
  uint8  num_code_segs;
  /* number of bits per code segment */
  uint32 num_bits_per_code_seg;
  /* number of pre-rate matched bits for a given TFCI */
  uint32 pre_rm_bits;
  /* number of filler bits to be used in the first code segment */
  uint32 num_filler_bits;
  /* Bit stream length after transport block concatenation */
  uint32 len_tb_concat;
} tdsl1_ul_trch_coding_struct_type;


/* Turbo Interleaving parameters */
typedef struct {
  /**** TX_TURBO_INTLV_PARAM0_TRCHn parameters ****/
  /* C - 1, where C is the #columns */
  uint8  num_columns_minus1;
  /* last column in the last row */
  uint8  last_column;
  /* #rows in the interleaver table */
  uint8  num_row_index;
  /* exchange indicator (1st column cell with last column cell in last row */
  uint8  last_row_bit_exch;
  /* index of last row with valid data */
  uint8  last_row;

  /**** TX_TURBO_INTLV_PARAM1_TRCHn parameters ****/
  /* index of prime number p used in the index table */
  uint8  prime_num_index;
  /* prime number p */
  uint16 prime_number;
} tdsl1_ul_trch_turbo_ileav_parms_type;

/* RM error parameters */
typedef struct {
  /* flag to indicate if puncturing is required */
  boolean punct_flag;
  /* error variable eini1 */
  uint16  eini1;
  /* error variable eini2 */
  uint16  eini2;
  /* error variable eminus1 */
  uint16  eminus1;
  /* error variable eminus2 */
  uint16  eminus2;
  /* error variable eplus1 */
  uint16  eplus1;
  /* error variable eplus2 */
  uint16  eplus2;
} tdsl1_ul_rm_parms_struct_type;

#ifdef FEATURE_TDSCDMA_HSUPA

typedef struct
{
  /* SFN at which DPCH CONFIG is saved */
  uint16                    sfn;

  /* content of DPCH config which is sent to FW */
  tfw_ul_dpch_config_cmd_t  dpch_config;
} tdsl1_ul_dpch_config_struct_type;

/* Information including configuration and data buffer that would be used 
 * for E-RUCCH transmission */
typedef struct
{
  /* E-RUCCH configuration pointer */
  tdsl1_e_rucch_info_struct_type     *erucch_cfg_ptr;

  /* the latest two DPCH_CONFIG which are kept for HSUPA to calculate
   * power headroom for EPUCH transmission
   */
  tdsl1_ul_dpch_config_struct_type    hist_dpch_config[2];

  tfw_erucch_config_cmd_t             tfw_erucch_config;

  /* The ASC cfg that selected for E-RUCCH transmission */
  tdsl1_ul_asc_avail_mask_struct_type asc_mask;

  /* Stores E-RUCCH data. The size of E-RUCCH data is always 39 bits */
  uint8                               erucch_data[5];
} tdsl1_ul_erucch_info_struct_type;
#endif

#ifdef FEATURE_TDSCDMA_DSDA

/*Data Structure of Tx Power Reporting for RF Coexistence and SAR*/
typedef struct
{
  int16 filtered_tx_power_rf_coex_dbm_q4;
  int16 filtered_tx_power_sar_dbm_q4;
  int16 tx_power_during_period_sum_rf_coex_dbm_q4;
  int16 tx_power_during_period_sum_sar_dbm_q4;
  uint8 filter_period_count_rf_coex;
  uint8 filter_period_count_sar;
  uint16 active_timeslot_during_period_count;
}tdsl1_ul_dsda_tx_power_reporting_struct_type;

/*20ms for RF COEX Reproting*/
#define TDSL1_DSDA_TX_POWER_REPORTING_PERIOD_RF_COEX 4

/*100MS for SAR reporting*/
#define TDSL1_DSDA_TX_POWER_REPORTING_PERIOD_SAR 20

#endif /*FEATURE_TDSCDMA_DSDA*/

/*Data Structure of Tx Power Reporting for modem power logging */
typedef struct
{
  int16 filtered_tx_power_dbm_q4;
  int16 tx_power_during_period_sum_dbm_q4;
  uint8 filter_period_count;
  uint16 active_timeslot_during_period_count;
}tdsl1_ul_tx_power_reporting_struct_type;

/*20ms for Tx Pwr Reproting*/
#define TDSL1_TX_POWER_REPORTING_PERIOD 4

#ifdef FEATURE_QSH_EVENT_METRIC
/*100 ms for Tx Pwr Reporting*/
#define TDSL1_QSH_TX_POWER_REPORTING_PERIOD 20
#endif

/*===========================================================================

                  VARIABLE STATEMENTS

===========================================================================*/

   
/* uplink CCTrCH data base */
extern tdsl1_ul_cctrch_cfg_struct_type          tdsl1_ul_cctrch_buf[TDS_UL_CFG_MAX];

/*extern tdsmac_ul_tfcs_type                        tdsl1_ul_local_tfcs_buf[TDS_UL_CFG_MAX];*/
extern tdsmac_ul_tfcs_type                       *tdsl1_ul_local_tfcs_buf;

/* RRC may modify the pointer values so in order to avoid that
 * we maintain a local copy of cctrch cfg parameters 
 */
extern tdsl1_ul_semi_static_parms_struct_type   tdsl1_ul_semi_static_local_parms_buf[TDSUE_MAX_TRCH][TDS_UL_CFG_MAX];
extern tdsl1_ul_gain_factor_parms_struct_type   tdsl1_ul_gain_factor_local_parms_buf[TDSL1_UL_TFC_MAX][TDS_UL_CFG_MAX];

/* The current CCTrCH pointer will always point to the initial 
 * configuration at channel set up. It will be updated whenever a 
 * re-configuration happens at the desired CFN.
 */
extern tdsl1_ul_cctrch_cfg_struct_type         *tdsl1_ul_curr_cctrch_ptr;

/* uplink physical channel configuration data buffer */
extern tdsl1_ul_phych_cfg_struct_type         tdsl1_ul_phychan_buf[TDS_UL_CFG_MAX];

/* current uplink physical channel configuration pointer */
extern tdsl1_ul_phych_cfg_struct_type        *tdsl1_ul_curr_phychan_ptr;

/* UL TFCS table */
extern tdsmac_ul_tfcs_type                       *tdsl1_ul_tfcs_ptr;

/* Bit masks for available SYNC_UL codes and sub-channels */
extern tdsl1_ul_asc_avail_mask_struct_type        tdsl1_ul_asc_mask[TDSL1_UL_ASC_MAX];

/* Transport Channel coding parameters */
extern tdsl1_ul_trch_coding_struct_type           tdsl1_ul_trch_coding_parms[TDSUE_MAX_TRCH];

extern tdsl1_ul_trch_turbo_ileav_parms_type       tdsl1_ul_trch_turbo_ileav_parms[TDSUE_MAX_TRCH];

extern tdsl1_ul_rm_parms_struct_type              tdsl1_ul_rm_parms[TDSUE_MAX_TRCH];

/* table of uplink filler enumeration values */
extern uint8                                      tdsl1_ul_filler_val_tab[TDSL1_UL_NUM_FILLER_TYPE];

#ifdef FEATURE_TDSCDMA_HSUPA
/* Information including configuration and data buffer that would be used 
 * for E-RUCCH transmission */
extern tdsl1_ul_erucch_info_struct_type           tdsl1_ul_erucch_info;
#endif

#ifdef FEATURE_TDSCDMA_DSDA
extern tdsl1_ul_dsda_tx_power_reporting_struct_type tdsl1_ul_dsda_tx_power_reporting;
#endif /*FEATURE_TDSCDMA_DSDA*/
/*===========================================================================

                  FUNCTION STATEMENTS

===========================================================================*/
extern void tdsl1ulcfg_copy_phychan_cfg_to_local
(
  /* pointer to UL physical channel db */
  tdsl1_ul_phych_cfg_struct_type      *cmd_ptr
);

#ifdef FEATURE_TDSCDMA_DSDA
/*===========================================================================
FUNCTION        tdsulcmd_init_dsda_tx_power_reporting

DESCRIPTION     This function initialize the DSDA TX power reporting

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void tdsl1ulcfg_init_dsda_tx_power_reporting(void);

/*=========================================================================
FUNCTION tdsl1ulcfg_dsda_tx_power_reporting_process

DESCRIPTION
  process UL TX power in FACH state for DSDA TX power reporting purpose

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=========================================================================*/
void tdsl1ulcfg_dsda_tx_power_reporting_process(void);

#endif /*FEATURE_TDSCDMA_DSDA*/

/*===========================================================================
FUNCTION        tdsulcmd_init_dsda_tx_power_reporting

DESCRIPTION     This function initialize the DSDA TX power reporting

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void tdsl1ulcfg_init_modem_tx_power_reporting(void);

/*=========================================================================
FUNCTION tdsl1ulcfg_tx_power_reporting_process

DESCRIPTION
  process UL TX power in modem TX power reporting purpose

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=========================================================================*/
void tdsl1ulcfg_modem_tx_power_reporting_process(uint8 report_period);

/*===========================================================================
FUNCTION  tdsl1ulcfg_reset_modem_power_stats_tx

DESCRIPTION
Clears and resets power stats

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void tdsl1ulcfg_reset_modem_power_stats_tx(void);


/*=========================================================================
FUNCTION tdsl1ulcfg_modem_tx_power_statistics_process

DESCRIPTION
  process UL TX power in modem TX power reporting purpose

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=========================================================================*/
void tdsl1ulcfg_modem_tx_power_statistics_process(void);

#endif /* TDSL1_UL_CFG_H */


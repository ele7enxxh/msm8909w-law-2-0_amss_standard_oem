#ifndef TDSCDMA_LOCAL_DEFINES_H
#define TDSCDMA_LOCAL_DEFINES_H

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <tdscdma> */
#define FEATURE_CM_GSM_SEARCH_GAP_USE_OPTIMIZE_STATISTICS
#define FEATURE_CM_SEARCH
#define FEATURE_CM_SUPPORTED
#define FEATURE_DATA_PDCP
#define FEATURE_DL_BLER_QUAL_CTL
#define FEATURE_ENHANCED_CELL_RESELECTION
#define FEATURE_EVENTS_SUPPORTED
#define FEATURE_GSM_ARFCN_STRUCT
#define FEATURE_HSDPA_L1_STATS
#define FEATURE_HS_LOG_DECODE_STATUS_W_DATA
#define FEATURE_L1_CELL_FACH_INTRA_RESELECTION
#define FEATURE_L1_DETECTED_CELLS_FACH
#define FEATURE_L1_DETECTED_CELLS_IDLE
//#define FEATURE_MCAL_WCDMA    //split as it is W's feature
#define FEATURE_MINI_BSIC_RECONF_IN_DRX
#define FEATURE_Q6_MT
#define FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU

#define FEATURE_TB_IND_DETAIL_INFO
#define FEATURE_UL_TB0_PATCH
#define FEATURE_W2G_FREQ_ERR
//#define FEATURE_WCDMADIAG_INIT_IN_RRC

//#define FEATURE_WCDMA_BAND_PRIORITIZATION //mainlined
//#define FEATURE_WCDMA_COMP_GF   //no code
// #define FEATURE_WCDMA_CONTINUE_MPLMN //mainlined
//#define FEATURE_TDSCDMA_DC_HSDPA_DIFF_TXD_MODE  //should not define after rename

//#define FEATURE_WCDMA_HSUPA_CM
//#define FEATURE_WCDMA_IRAT_RF_BUF_SUPPORT

//#define FEATURE_WCDMA_L1_DETECTED_CELLS_DCH

//#define FEATURE_WCDMA_L1_FORCE_RECONF_ABORT_SKIP_FIRST_BSIC_ID_CELL

//#define FEATURE_WCDMA_L1_HW_SUPPORT_TM_CIPHER

//#define FEATURE_WCDMA_L1_LOCK_GSM_CELL_POS_UNDER_BSIC_ID
// #define FEATURE_WCDMA_PRACH_SELECTION //TDS is not using it
// #define FEATURE_WCDMA_QUALITY_MEAS  //TDS is not using it

// #define FEATURE_WCDMA_RRC_ZI_OPT_PHASE_2  //TDS is not using it
//#define FEATURE_WCDMA_RX_TX

//#define FEATURE_WCDMA_TXAGC_DB10

//#define FEATURE_WCDMA_UL_RACH_EVT_PWR_FIX
//#define FEATURE_WCDMA_UL_TFC_PL_REV_2
#define L1_UL_USE_DSM


/* includes for local cust files
 * Note - these files are only visible to tdscdma.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */
#include "custtdscdma.h"

#endif /* TDSCDMA_LOCAL_DEFINES_H */

/*****************************************************************************
***
*** TITLE
***
***  MAC Common definitions
***
***
*** DESCRIPTION
***
***  Internal common definitions used throughout the MAC task.
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacdef.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 06/27/01   TLX     Created
***
*****************************************************************************/

#ifndef INC_GMACDEF_H
#define INC_GMACDEF_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "ms.h"
#include "gmacsig.h"
#include "gmactask.h"
#include "csn.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/
typedef enum
{
  NULL_MODE,
  IDLE_MODE,
  TRANSFER_MODE
} mac_modes_t;

typedef enum
{
  NOT_SET,
  SET
} set_not_set_t;

typedef enum
{
  NO_RE_EST,
  RE_EST_WAIT_FOR_SB_ALLOC,
  RE_EST_SEND_PRR
} idle_mode_ul_tbf_release_action_t;

/* Miscellaneous error values */
typedef enum
{
  NO_ERRORS,
  FREQ_ERROR_ACCESS_RETRY,
  FREQ_ERROR_PARTIAL_SYS_ACQ,
  MULTISLOT_ERROR_ACCESS_RETRY,
  TBF_STARTING_TIME_ERROR_ACCESS_RETRY,
  ASSIGNMENT_ADDRESSING_ERROR,
  ASSIGNMENT_DATA_ERROR,
  FREQ_ERROR_DIFFERENT_PARAMS,
  FREQ_CONFIG_FAILED_CHECK
} pua_data_error_t;

typedef enum
{
  NONE,
  UL_DATA_EST,
  PCCF_EST,
  PMR_EST
}gprs_mac_action_t;

/*used when TFI in the MAC control header is used to address MS */
typedef enum
{
 MAC_HEADER_TFI_NOT_VALID,
 MAC_HEADER_TFI_MATCH_FOUND,
 MAC_HEADER_TFI_MATCH_NOT_FOUND
} mac_header_tfi_t;

#ifdef  FEATURE_GSM_DTM
typedef enum
{
  NO_DTM_ASS_ERROR,
  DTM_ASS_REJ_FREQ_NOT_IMPLEMENTED,
  DTM_ASS_REJ_PROTOCOL_ERR,
  DTM_ASS_REJ_CHAN_MODE_UNACCEPTABLE,
  DTM_ASS_REJ_NO_CELL_ALLOC_AVAILABLE
} dtm_ass_status_t;
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GSM_EDTM
typedef struct
{
  uint8   csn_encoded_edtm_pcr[SIZE_OF_CONTROL_RADIO_BLOCK];
  boolean edtm_pcr_pending;
  uint8   edtm_pcr_cause;
} gmac_edtm_pcr_params_t;
#endif /* FEATURE_GSM_DTM */


typedef enum
{
  GMAC_PRR_ON_PACCH_AWAIT_REQ,     /*default state, waiting for RM_UL_REALLOC_REQ*/
  GMAC_PRR_ON_PACCH_AWAIT_QUAL,    /*MAC has sent MAC_L1_QUALITY_RPT and is waitng for L1_MAC_QUAL_MEASURMENTS*/
  GMAC_PRR_ON_PACCH_AWAIT_USF      /*waiting for confirmation that PRR has been TX-ed, ie L1_MAC_SINGLE_BLOCK_SENT*/

} gmac_prr_on_pacch_state_type;


typedef struct
{
  /***** wait_for_ul_rel_confirm ****************************************************************
   *
   *
   ************************************************************************************************/
  boolean wait_for_ul_rel_confirm;


  /***** egprs_rac_retran_req ****************************************************************
   *
   *
   ************************************************************************************************/
  boolean egprs_rac_retran_req;

  /***** egprs_arac_sent ****************************************************************
   *
   *
   ************************************************************************************************/
  boolean egprs_arac_sent;


  /***** mac_ctrl_header_r_bit ****************************************************************
   *
   *  used to control the r bit in the RLC/MAC control header
   *
   ************************************************************************************************/
  set_not_set_t  mac_ctrl_header_r_bit;

  /***** tbf_release_pending_flg ****************************************************************
   *
   *  Indicates that MAC is waiting for a release confirm from L1
   *  used during release of a tbf to indicate that L1 has
   *  physically released the tbf and returned to idle
   *
   *
   ************************************************************************************************/
   boolean tbf_release_pending_flg;

   /***** egprs_prr_sent **************************************************************************
   *
   *
   ************************************************************************************************/
   boolean egprs_prr_sent;
   
   /***** gmac_prr_on_pacch_state *******************************************************
   *
   *
   *************************************************************************************/
   gmac_prr_on_pacch_state_type gmac_prr_on_pacch_state;

   /***** waiting_for_dl_sb_control_msg ************************************************
   *
   *
   *************************************************************************************/
   boolean waiting_for_dl_sb_control_msg;

}common_flags_t;

typedef struct
{
  boolean dl_tfi_valid; /* DL in progress */
  boolean ul_tfi_valid; /* UL in progress */
  uint8   dl_tfi;
  uint8   ul_tfi;
} tfi_store_t;

typedef struct
{
  uint8 nco;
  uint8 nc_non_drx_period;
  uint8 csn_encoded_pmr[SIZE_OF_CONTROL_RADIO_BLOCK];
  boolean pmr_pending;
  boolean msg_type_is_pmr;
} gmac_pmr_params_t;


typedef enum
{
 NO_PENDING_PCCN,
 PCCN_PENDING_UL,
 PCCN_PENDING_DL
}pccn_pending_t;

typedef struct
{
  uint8 csn_encoded_pccn[SIZE_OF_CONTROL_RADIO_BLOCK];
  pccn_pending_t  pccn_pending;
} gmac_pccn_params_t;

typedef struct
{
  uint8 csn_encoded_pss[SIZE_OF_CONTROL_RADIO_BLOCK];
  boolean pss_pending;
} gmac_pss_params_t;

typedef struct
{
  uint8 csn_encoded_pps[SIZE_OF_CONTROL_RADIO_BLOCK];
  boolean pps_pending;
} gmac_pps_params_t;

typedef enum
{
    NO_PCCO_ACTIVE,
    PCCO_ACTIVE
} gmac_pcco_state_t;

typedef enum
   {
  GMAC_IDLE,
  GMAC_IDLEUTIL,
  GMAC_NULL,
  GMAC_PROC,
  GMAC_TASK,
  GMAC_TRAN,
  GMAC_TRAN_C,
  GMAC_TRAN_UL,
  GMAC_TRAN_DL,
  GMAC_UTIL
} gmac_filenames_t;

/*changed 2p rac*/
typedef enum
{
  GPRS_SB_ALLOC,
  EGPRS_SB_ALLOC,
  MB_ALLOC_OF_1_BLOCK,
  MB_ALLOC_OF_2_BLOCKS
}egprs_2p_sb_mb_alloc_t;


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/*============================================*/
/* rlc/mac ul control block header definition */
/* \payload type \ spare  \ r \               */
/*============================================*/
#define UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET  0x40
#define UL_CNTL_BLOCK_HEADER_R_BIT_SET      0x41

/* Access Burst format for PCA msg */
#define PCA_8_BIT_AB_FORMAT                 0x7c    /*   \ msg type \ctrl_ack\ */
                                                    /*   \ 0111 11  \ xx     \ */

#define PCA_11_BIT_AB_FORMAT_BITS_9_TO_2    0xfc    /*   \ spare    \msg type\ */
                                                    /*   \ 1111     \ 1100   \ */

#define PCA_11_BIT_AB_FORMAT_BITS_1         0x04    /*   \ msg type \ctrl_ack\ */
                                                    /*   \        1  \  xx   \ */

/* Release causes for PACKET TBF RELEASE */
#define GMAC_PACKET_TBF_REL_NORMAL_CAUSE      0x00
#define GMAC_PACKET_TBF_REL_ABNORMAL_CAUSE    0x02


/* MAX number of retry attempts for UL TBF prior to failure */
#define MAX_RETRY_ATTEMPTS                  4

/* Logging-on-demand channel type definitions */
#define PRACH_11BIT_CHANNEL                 0x01
#define PRACH_8BIT_CHANNEL                  0x02
#define PACCH_RRBP_CHANNEL                  0x03
#define UL_PACCH_CHANNEL                    0x04

#define PCCCH_CHANNEL                       0x81
#define PBCCH_CHANNEL                       0x82
#define DL_PACCH_CHANNEL                    0x83

#define DL_CHANNEL_UNDEFINED                0xFF

#ifdef FEATURE_GSM_GPRS_MSC33
#define GMAC_SHIFTED_USF_NUM_OF_UL_TS       5
#endif /*FEATURE_GSM_GPRS_MSC33*/

#define  GPRS_MAC_MASTER_SIG         (0x01)
#define  GMAC_DOG_RPT_TMR_SIG        (0x02)

#define PWR_CTRL_ALPHA_MAX_VALUE     0x0a

#ifdef FEATURE_GPRS_PS_HANDOVER
#define PSHO_FAILURE_OTHERS                          4
#define PSHO_NO_RESPONSE_ON_TARGET_CELL              1
#define PSHO_TBF_MODE_EGPRS                          1
#define PSHO_TBF_MODE_GPRS                           0
#define PSHO_RLC_RESET                               1
#define PSHO_RLC_NOT_RESET                           0
#endif /*FEATURE_GPRS_PS_HANDOVER*/

/* Defines GAMMA position in bitmap received from CSN.1 decoder */
#define GAMMA_TN0_POS 0x01
#define GAMMA_TN1_POS 0x02
#define GAMMA_TN2_POS 0x04
#define GAMMA_TN3_POS 0x08
#define GAMMA_TN4_POS 0x10
#define GAMMA_TN5_POS 0x20
#define GAMMA_TN6_POS 0x40
#define GAMMA_TN7_POS 0x80

#define T_AVG_W_MAX_VALUE 0x19  /* t_avg_w cannot exceed value of 25 */
#define T_AVG_T_MAX_VALUE 0x19  /* t_avg_w cannot exceed value of 25 */

#define FREQ_PARAMS_NO_ERRORS                     0
#define FREQ_PARAMS_FREQ_ERROR_ACCESS_RETRY       1
#define FREQ_PARAMS_FREQ_ERROR_PARTIAL_SYS_ACQ    2
#define FREQ_PARAMS_ASSIGNMENT_ADDRESSING_ERROR   3
#define FREQ_PARAMS_ASSIGNMENT_DATA_ERROR         4

#define GMAC_RR_PUA_EXTENSION                0x00
#define GMAC_RR_PUA_DYNAMIC_ALLOCATION       0x01
#define GMAC_RR_PUA_RESERVED_1               0x02
#define GMAC_RR_PUA_RESERVED_2               0x03

#define GMAC_EDA_IND_DYNAMIC_ALLOC           0x00

#define GMAC_MAC_MODE_DYNAMIC_ALLOC          0x00
#define GMAC_MAC_MODE_EXTENDED_DYNAMIC_ALLOC 0x01

#define GMAC_NO_TS_ALLOCATED_IN_BITMAP       0x00

#define GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC  0x01

#define GMAC_PRR_ACC_TYPE_TWO_PHASE          0x00
#define GMAC_PRR_ACC_TYPE_PAGE_RESP          0x01
#define GMAC_PRR_ACC_TYPE_CELL_UPD           0x02
#define GMAC_PRR_ACC_TYPE_MM_PROC            0x03

/*****************************************************************************
* buffer used to hold the data part of the rlc/mac dl control block( ie the  *
* logcical channel msg) and is the buffer passed to csn1                     *
*****************************************************************************/

#ifdef FEATURE_GSM_EDTM
  /*there will be a minimum of 1 header and 1 optional octet to be*/
  /*deducted from the max size of 23 octet for Dl ctrl msg (21*9 segments)*/
  #define SIZE_OF_DL_MSG 189
#else
  /*there will be a minimum of 1 header and 1 optional octet to be*/
  /*deducted from the max size of 23 octet for Dl ctrl msg (21*2 segments)*/
  #define SIZE_OF_DL_MSG 42
#endif /*FEATURE_GSM_EDTM*/

/*ref 24.008  table 10.5.146 MS radio access capability IE*/
#define SIZE_OF_RA_CAP_LEN 8                    /* 8 bits */
#define SIZE_OF_ACCESS_TYPE 4                   /* 4 bits */
#define SIZE_OF_ACCESS_TYPE_LEN_IND 7           /* 7 bits */
#define PRESENT_NOT_PRESENT_BIT 1

#define SIZE_OF_ARAC_MESSAGE_TYPE 6
#define SIZE_OF_ARAC_TLLI 32

#define WS_ALLOCATION_INVALID 0xffff

#endif /* INC_GMACDEF_H */
/*** EOF: don't remove! ***/

#ifndef TDS_RRC_RR_IF_H
#define TDS_RRC_RR_IF_H
/*===========================================================================
                    TD-SCDMA RRC  -  RR   INTERFACE

DESCRIPTION

  This module is the header module that contains the type definitions for
  the primitives sent to TD-SCDMA RRC from GERAN RR. The module is only used for dual-mode.
  
Copyright (c) 2002-2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tdsrrc_rr_if.h#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
24/02/11   rmn     Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "comdef.h"
#include "sys.h"
#include "sys_stru.h"
#include "sys_plmn_selection.h"
#include "queue.h"

#include "tdsrrccmd.h"

#include "lte_irat_types.h"


#include "tds_rrc_irat_types.h"

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/***************************************************************************/
/****************  MSG ID's Rxd by TDSRRC   ********************************/
/***************************************************************************/



/* Set ts0_pos as invalid if the cell is not able to measure */
#define TDS_TS0_POSITION_INVALID  0xFFFF

/*
typedef enum           
{                      
} tdsrrc_rr_cmd_e_type;
*/

/***************************************************************************/
/******************  GENERAL  **********************************************/
/***************************************************************************/
typedef enum
{
  TDSRRC_RR_SUCCESS,
  TDSRRC_RR_FAILURE,
  TDSRRC_RR_MAX_STATUS
} tdsrrc_rr_status_e_type;


/*temporary keep the UMTS terms to pass the compile*/
typedef enum
{
  TDSRRC_NOT_BARRED,
  UMTS_NOT_BARRED = TDSRRC_NOT_BARRED,
  TDSRRC_CPID_BARRED,
  UMTS_PSC_BARRED = TDSRRC_CPID_BARRED,
  TDSRRC_FREQ_BARRED,
  UMTS_FREQ_BARRED = TDSRRC_FREQ_BARRED
}tdsrrc_cell_bar_info_e_type;

/***************************************************************************/
/****************  RR -> TD HANDOVER  **************************************/
/***************************************************************************/

typedef byte TDSRRC_SAPI_T_TEMP;

/*update for naming conflict*/
/*
typedef enum
{
  TDSRRC_SUCCESS,
  TDSRRC_FAILURE,
  TDSRRC_MAX_STATUS
} tdsrrc_rr_status_e_type;
*/

/* Others moved to tds_rrc_irat_types.h */

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_HANDOVER_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  /* Cipher Enable Indicator */
  boolean cipher_enabled;

  /* Inter-RAT measurement data */
  tdsrrc_interrat_measurement_data_type interrat_measurement_data;

  /* Ptr to byte stream containing handOverToUTRANCommand*/
  byte ho_cmd[MAX_OCTETS_L3_MESSAGE];

  /* TRK_LO_ADJ */
  uint16 trk_lo_adj;

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  /*temporoary mainline the feature FEATURE_MCC_AND_850_800_BAND_COMBINATION for feature clean-up
    in API folder.*/
  /* PLMNId of the PLMN UE is camped on the GSM side */
  PLMN_id_T  plmn_id;


  /*temporoary mainline the feature FEATURE_RAT_PRIORITY_LIST for feature clean-up
    in API folder.*/

  sys_rat_pri_list_info_s_type   rat_pri_list_info;

  lte_irat_dedicated_priority_info_s dedicated_priority_info;


} tdsrrc_interrat_handover_req_type;

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_HANDOVER_ABORT_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_rr_status_e_type abort_status;
} tdsrrc_interrat_handover_abort_req_type;

/*----------------------------------------------------------------------------
                         TDSRRC_NAS_DATA_IND
This primitive is sent from RR to RRC to forward bufferred NAS signaling 
message. If there is not message left to send, RR will send the primitive to 
RR with the message length field set to 0. 
----------------------------------------------------------------------------*/
typedef struct
{
  /* NAS signalling data */
  TDSRRC_SAPI_T_TEMP SAPI;
  byte               l3_message_length;
  byte               l3_message[MAX_OCTETS_L3_MESSAGE];

} tdsrrc_nas_data_ind_type;

/***************************************************************************/
/****************  TD -> RR HANDOVER  **************************************/
/***************************************************************************/

/* Maximum length of the GSM Command */
#define TDSRR_MAX_GSM_MESSAGE_LENGTH 256

/* Maximum number of the GSM commands */
#define TDSRR_MAX_GSM_COMMANDS 4 /* define as 4 for now, it should only be one */

typedef struct
{
  /* Length of the command */
  uint8 message_length;
 
  /* Message payload */
  uint8 payload[TDSRR_MAX_GSM_MESSAGE_LENGTH];
} tdsrrc_rr_gsm_command_type;

typedef struct
{
  /* Number of the GSM Commands */
  uint8 number_of_commands;
 
  /* GSM command list */
  tdsrrc_rr_gsm_command_type gsm_commands[TDSRR_MAX_GSM_COMMANDS];
} tdsrrc_rr_gsm_command_list_type;

typedef enum
{
  TDSRRC_RR_INVALID_STATE,
  TDSRRC_RR_GSM_PROTOCOL_ERROR,
  TDSRRC_RR_INVALID_CONFIGURATION,
  TDSRRC_RR_PHYSICAL_CHANNEL_FAILURE,
  TDSRRC_RR_MAX_FAILURE_CAUSE,
  TDSRRC_RR_CIPHER_KEY_NOT_AVAILABLE
} tdsrrc_rr_failure_cause_e_type;

/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_HANDOVER_CNF
----------------------------------------------------------------------------*/
typedef struct
{
  /* Handover status */
  tdsrrc_rr_status_e_type handover_status;

  /* Handover failure cause: this field is only valid if handover status is
     failure */
  tdsrrc_rr_failure_cause_e_type failure_cause;

  /* GSM commands: this field is only filled if handover is failure */
  tdsrrc_rr_gsm_command_list_type gsm_command_list;
} tdsrr_interrat_handover_cnf_type;

/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_HANDOVER_ABORT_CNF
----------------------------------------------------------------------------*/
typedef struct
{
  /* Handvoer Abort status */
  tdsrrc_rr_status_e_type abort_status;
} tdsrr_interrat_handover_abort_cnf_type;

/***************************************************************************/
/****************  TD -> RR PLMN SEARCH  ***********************************/
/***************************************************************************/

/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_HANDOVER_ABORT_CNF
----------------------------------------------------------------------------*/
typedef struct
{
  /* Indicates if the BPLMN search is done or not */
  boolean srch_done;

  /* Indicates if the highest priority PLMN was found or not */
  boolean hplmn_found;

  /* List of higher priority PLMNs that were found on 3G */
  sys_detailed_plmn_list_s_type available_plmn_list;
} tdsrr_interrat_plmn_srch_cnf_type;

/***************************************************************************/
/****************  RR -> TD PLMN SEARCH  ***********************************/
/***************************************************************************/
typedef enum
{
  TDSRRC_PLMN_LIST_REQ_RCVD,
  TDSRRC_PLMN_SELECT_REQ_RCVD,
  TDSRRC_EST_REQ_RCVD,
  TDSRRC_GTOW_RESELECT_REQ_RCVD,
  TDSRRC_ABORT_MAX_STATUS
} tdsrrc_abort_cause_e_type;

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_PLMN_SRCH_REQ
----------------------------------------------------------------------------*/
typedef struct
{
  uint32       num_plmns;
  /* PLMN Ids to be searched on 3G with 0th element being
   * the highest priority PLMN */
  PLMN_id_T  plmn[SYS_PRIORITY_PLMN_LIST_MAX_LENGTH];

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

   /* Time allowed by GL1 for UTRAN TDD to be active */
  uint32 timer_val;

  /* Used by RRC to determine if the plmn search request
   * is for "full service search" or "plmn list search"
   */
  sys_service_search_e_type service_search;

  boolean camped_suitably;

  detected_frequency_list_type detected_frequency_list;

#ifdef FEATURE_TDSCDMA_SCAN_SCOPE
  /* Sent by RR so that TRRC can determine if the plmn search request
   * is for "full band search" or "ACQ_DB Search"
   *
   * RR originally receives the enum value from NAS
   */
  sys_scan_scope_e_type  scan_scope;
#endif
} tdsrrc_interrat_plmn_srch_req_type;

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_PLMN_SRCH_ABORT_REQ
----------------------------------------------------------------------------*/
typedef struct
{
  /* Indicates why the search is being aborted */
  tdsrrc_abort_cause_e_type abort_cause;
} tdsrrc_interrat_plmn_srch_abort_req_type;

/***************************************************************************/
/****************  TD -> RR RESELECTION  ***********************************/
/***************************************************************************/
typedef enum
{
  TDSRR_GSM_OTHER_REASON,
  TDSRR_GSM_CELL_BARRED,
  TDSRR_GSM_CELL_FORBIDDEN,
  TDSRR_GSM_PLMN_MISMATCH,
  TDSRR_GSM_CELL_LOW_C1,
  TDSRR_GSM_BAND_NOT_SUPPORTED,
  TDSRR_GSM_BSIC_NOT_FOUND,
  TDSRR_GSM_BCCH_NOT_FOUND,
  TDSRR_GSM_BCCH_DECODE_FAIL,
  TDSRR_GSM_INVALID_STATE
} tdsrr_interrat_reselect_fail_reason_type;


/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_RESELECT_REJ
----------------------------------------------------------------------------*/
typedef struct
{
   /* Cause of InterRAT Reselection Failure */
   tdsrr_interrat_reselect_fail_reason_type failure_reason;

   /* Time ( in Sec ) when the same cell should be considered */
   word                        time_to_reselect;

   /* Expected RXLev Average of the GSM Target Cell */
   byte                        Expected_RXLEV_average;

} tdsrr_interrat_reselect_rej_type;

/***************************************************************************/
/****************  RR -> TD RESELECTION  ***********************************/
/***************************************************************************/

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_RESELECT_REQ
----------------------------------------------------------------------------*/
typedef struct
{
   #ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type     as_id;
   #endif

   /* The Frequency of the Target UTRAN Cell */
   uint32                               UARFCN;

   /* The Cell Parameter ID code of the target UTRAN TDD Cell */
   uint8                                cpid;

   /* Indicates how a PLMN is to be selected. */
   sys_network_selection_mode_e_type    network_select_mode;

   /* PLMNId of the requested PLMN */
   PLMN_id_T                            requested_PLMN;

   /* TRK_LO_ADJ */
   uint16                               trk_lo_adj;

   sys_rat_pri_list_info_s_type         rat_pri_list_info;

   lte_irat_dedicated_priority_info_s dedicated_priority_info;
} tdsrrc_interrat_reselect_req_type;


/***************************************************************************/
/****************  TD -> RR CC ORDER  **************************************/
/***************************************************************************/
typedef enum
{
  TDSRR_GSM_CCO_OTHER_REASON,
  TDSRR_GSM_CCO_CELL_BARRED,
  TDSRR_GSM_CCO_CELL_FORBIDDEN,
  TDSRR_GSM_CCO_CELL_LOW_C1,
  TDSRR_GSM_CCO_BAND_NOT_SUPPORTED,
  TDSRR_GSM_CCO_BSIC_NOT_FOUND,
  TDSRR_GSM_CCO_BCCH_NOT_FOUND,
  TDSRR_GSM_CCO_BCCH_DECODE_FAIL,
  TDSRR_GSM_CCO_T309_EXPIRED,
  TDSRR_GSM_CCO_INVALID_STATE
} tdsrr_interrat_cc_order_fail_reason_type;

typedef enum
{
  TDSCCO_SUCCESS,
  TDSCCO_FAILURE_BEFORE_ACT_IND,
  TDSCCO_FAILURE_AFTER_ACT_IND,
  TDSCCO_ABORTED_DUE_TO_HIGH_PRI_REQ 
} tdsrr_interrat_cc_order_status_e_type;


/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_CC_ORDER_REJ
----------------------------------------------------------------------------*/
typedef struct
{
   /* Cause of InterRAT CC Order Failure */
   tdsrr_interrat_cc_order_fail_reason_type failure_reason;

} tdsrr_interrat_cc_order_rej_type;


/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_CC_ORDER_ABORT_CNF
----------------------------------------------------------------------------*/
typedef struct
{
  /* Status of the Abort request */
   tdsrrc_rr_status_e_type abort_status;

} tdsrr_interrat_cc_order_abort_cnf_type;
/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_CC_ORDER_CNF
----------------------------------------------------------------------------*/
typedef struct
{
  /* WTG CCO Status */
  tdsrr_interrat_cc_order_status_e_type status;

  /* Failure cause if the status is not Success */
  tdsrr_interrat_cc_order_fail_reason_type failure_reason;

} tdsrr_interrat_cc_order_cnf_type;


/***************************************************************************/
/****************  RR -> TD CC ORDER  **************************************/
/***************************************************************************/

/*
typedef enum
{
  TDSRRC_NONE,
  TDSRRC_INAVALID_STATE,
  TDSRRC_CELL_BARRED,
  TDSRRC_UARFCN_BARRED,
  TDSRRC_PLMN_MISMATCH,
  TDSRRC_LOW_S_VALUE,
  TDSRRC_ACQUISITION_FAIL,
  TDSRRC_SIBS_FAILED_LOWER_LAYERS,
  TDSRRC_INVALID_SIBS,
  TDSRRC_SIBS_TIMEOUT,
  TDSRRC_SIBS_FAILED_OTHER,
  TDSRRC_FORBIDDEN_LA,
  TDSRRC_CELL_QUALITY_FAILURE,
  TDSRRC_CELL_NOT_SUITABLE,
  TDSRRC_CELL_NOT_EVALUATED,
  TDSRRC_CELL_CHANGE_FAILURE,
  TDSRRC_T3174_EXPIRED,
  TDSRRC_MAX_RACH_ATTEMPTS,
  TDSRRC_CONNECTION_REJECT
} tdsrrc_interrat_reject_reason_T;
*/
typedef enum
{
	TDSRRC_UMTS_NONE,
	TDSRRC_UMTS_INAVALID_STATE,
	TDSRRC_UMTS_CELL_BARRED,
	TDSRRC_UMTS_UARFCN_BARRED,
	TDSRRC_UMTS_PLMN_MISMATCH,
	TDSRRC_UMTS_LOW_S_VALUE,
	TDSRRC_UMTS_ACQUISITION_FAIL,
	TDSRRC_UMTS_SIBS_FAILED_LOWER_LAYERS,
	TDSRRC_UMTS_INVALID_SIBS,
	TDSRRC_UMTS_SIBS_TIMEOUT,
	TDSRRC_UMTS_SIBS_FAILED_OTHER,
	TDSRRC_UMTS_FORBIDDEN_LA,
	TDSRRC_UMTS_CELL_QUALITY_FAILURE,
	TDSRRC_UMTS_CELL_NOT_SUITABLE,
	TDSRRC_UMTS_CELL_NOT_EVALUATED,
	TDSRRC_UMTS_CELL_CHANGE_FAILURE,
	TDSRRC_UMTS_T3174_EXPIRED,
	TDSRRC_UMTS_MAX_RACH_ATTEMPTS,
	TDSRRC_UMTS_CONNECTION_REJECT
} tdsrrc_interrat_reject_reason_T;


/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_CC_ORDER_REQ
----------------------------------------------------------------------------*/
typedef struct
{

   /* The Frequency of the Target UTRAN Cell */
   uint16                               UARFCN;

   /* The cell parameter id code of the target UTRAN Cell */
   uint16                               cpid;

   /* Indicates how a PLMN is to be selected. */
   sys_network_selection_mode_e_type network_select_mode;

   /* PLMNId of the requested PLMN */
   PLMN_id_T                            requested_PLMN;

   /* Timer T3174 */
   uint32                               t3174;

   sys_rat_pri_list_info_s_type         rat_pri_list_info;

   lte_irat_dedicated_priority_info_s dedicated_priority_info;
} tdsrrc_interrat_cc_order_req_type;


/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_CC_ORDER_CNF
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;

  /* GTW PCCO Status */
  tdsrr_interrat_cc_order_status_e_type status;

  /* Failure cause if the status is not Success */
  tdsrrc_interrat_reject_reason_T failure_reason;

} tdsrrc_interrat_cc_order_cnf_type;


/***************************************************************************/
/****************  RR -> TD REDIRECION  ************************************/
/***************************************************************************/
typedef enum
{
  TDSRR_REDIRECT_FAIL_OTHER_REASON,
  TDSRR_REDIRECT_FAIL_WAIT_TIMER_EXPIRY,
  TDSRR_REDIRECT_FAIL_NO_SUITABLE_CELL_FOUND,
  TDSRR_REDIRECT_FAIL_MAX
} tdsrr_interrat_redirect_fail_reason_type;

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_REDIRECT_REQ
----------------------------------------------------------------------------*/

/* Note: MAX_UTRAN_TDS_CELLS  and MAX_UTRAN_TDS_FREQUNECIES are arbitrary values 
 * chosen because the MS must be able to monitor this many TDD neighbor cells. 
 * This is also because the CSN decoder imposes the same constraints. 
 * There is no limit (except message length) imposed on how many TDD neighour
 * cells might appear in the message.
*/

#define MAX_UTRAN_TDS_CELLS    32
/* Need to be at least the size of max TDD frequencies in LTE SIB6 */
#define MAX_UTRAN_TDS_FREQUNECIES    16

typedef struct
{
  uint32    cpid; /*cell parameter id*/
  boolean   diversity;
  uint8     dsm_idx; /* for retrieving sib data from sib container */
}tdsrrc_cell_type;

typedef struct
{
  uint32           uarfcn;
  /* If PFR active, use TDS_TS0_POSITION_INVALID 
   * for the frequency that is not able to measure
   */
  uint16           ts0_pos;
  uint8            sib_cont_idx; /* start index for this uarfcn in sib container */
  uint8            num_of_cells;
  tdsrrc_cell_type cell_list [MAX_UTRAN_TDS_CELLS];
} tdsrrc_cell_list_type;

typedef struct
{
  uint8                 num_of_frequencies;
  /*temporary keep fdd_cell_list for compliation, need change to tds... later*/
  tdsrrc_cell_list_type fdd_cell_list[MAX_UTRAN_TDS_FREQUNECIES];
} tdsrrc_target_info_type;

//typedef struct
//{
//  /* PLMN of the UTRAN TDD cell */
//  PLMN_id_T    PLMN;  
//  /* Indicates how a PLMN is to be selected */
//  sys_network_selection_mode_e_type    network_select_mode;
//
//  /* list of UTRAN TDD cells or frequencies */
//  tdsrrc_target_info_type    tds_target_info;
//
//  sys_rat_pri_list_info_s_type   rat_pri_list_info;
//} tdsrrc_interrat_redirect_req_type;


/*temporary mainline FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE for API clean up*/


typedef struct
{
  /* PLMN of the WCDMA cell */
  PLMN_id_T    PLMN;  
  /* Indicates how a PLMN is to be selected */
  sys_network_selection_mode_e_type    network_select_mode;

  /* list of WCDMA cells or frequencies */
 tdsrrc_target_info_type    tds_target_info; 


/*temporary mainline FEATURE_RAT_PRIORITY_LIST for API clean up*/
  sys_rat_pri_list_info_s_type   rat_pri_list_info;
/* FEATURE_RAT_PRIORITY_LIST */

  /* Indicate PFR status */
  boolean                   pseudo_fr_active;

   lte_irat_dedicated_priority_info_s dedicated_priority_info;

} tdsrrc_interrat_redirect_req_type;  /*split change*/


/* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */



/***************************************************************************/
/****************  TD -> RR REDIRECTION  ***********************************/
/***************************************************************************/

/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_REDIRECT_REJ
----------------------------------------------------------------------------*/
typedef struct
{
  byte       num_avail_plmns;

  /* PLMN Ids of GSM cells found */
  PLMN_id_T  plmn[SYS_PLMN_LIST_MAX_LENGTH];

  /* Cause of InterRAT Redirection Failure */
  tdsrr_interrat_redirect_fail_reason_type failure_reason;

  /* Signal quality */
  sys_signal_quality_e_type  signal_quality[SYS_PLMN_LIST_MAX_LENGTH];

  /* Signaling strength of the strongest cell in PLMN */
  sys_signal_strength_type  signal_strength[SYS_PLMN_LIST_MAX_LENGTH];

  byte          num_available_fplmns;

} tdsrr_interrat_redirect_rej_type;


/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_REDIRECT_ABORT_CNF
----------------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_rr_status_e_type abort_status;
} tdsrr_interrat_redirect_abort_cnf_type;



/***************************************************************************/
/****************  RR -> TD STOP MODE  *************************************/
/***************************************************************************/
typedef enum
{
  TDSRRC_GO_TO_INACTIVE=0,
  TDSRRC_POWER_OFF,
  TDSRRC_MODE_LPM
} tdsrrc_rr_stop_tds_cause_e_type;

/*----------------------------------------------------------------------------
                         TDSRRC_RR_STOP_UTRAN_TDD_REQ
----------------------------------------------------------------------------*/
typedef struct
{
  /* Indicates the reason for stopping TDS RRC */
  tdsrrc_rr_stop_tds_cause_e_type    stop_req_cause;
} tdsrrc_rr_stop_tds_req_type;

/***************************************************************************/
/****************  TD -> RR STOP MODE  *************************************/
/***************************************************************************/

/*----------------------------------------------------------------------------
                         TDSRR_STOP_GSM_CNF
----------------------------------------------------------------------------*/
typedef struct
{
  /* Indicates the reason for stopping UTRAN TDD */
  tdsrrc_rr_stop_tds_cause_e_type    stop_cnf_cause;
} tdsrr_stop_gsm_cnf_type;

/*----------------------------------------------------------------------------
                         TDSRR_CIPHER_SYNC_IND
    This primitive is sent from RRC to RR to indicated
    that the new keys are activated at TDS side
----------------------------------------------------------------------------*/
typedef struct
{
  /* Message header */
  IMH_T message_header;
  sys_modem_as_id_e_type     as_id;
} tdsrr_cipher_sync_ind_type;

/*****************************************************************************/
/* TDSRR_DEDICATED_PRIORITIES_RSP                                               */
/* FROM: GERAN RR                                                            */
/* TO:   TDSCDMA RRC                                                           */
/* DESCRIPTION:                                                              */
/*   This response contains a copy of RR's dedicated priority information    */
/*   and associated PLMN.                                                    */
/*****************************************************************************/
typedef struct
{
  /* Boolean indicating if dedicated priorities are valid or not */
  boolean dedicated_pri_valid;

  /* PLMN ID in which the dedicated priorities are received */
  sys_plmn_id_s_type    plmn;

  /* Dedicated priority info */
  lte_irat_dedicated_priority_info_s dedicated_pri_info;

} tdsrr_dedicated_priorities_rsp_type;

/*****************************************************************************/
/* TDSRRC_DEDICATED_PRIORITIES_RSP                                              */
/* FROM: TDSCDMA RRC                                                           */
/* TO:   GERAN RR                                                            */
/* DESCRIPTION:                                                              */
/*   This response contains a copy of RRC's dedicated priority information   */
/*   and associated PLMN.                                                    */
/*****************************************************************************/
typedef struct
{
  IMH_T message_header;
  sys_modem_as_id_e_type     as_id;

  /* Boolean indicating if dedicated priorities are valid or not */
  boolean dedicated_pri_valid;

  /* PLMN ID in which the dedicated priorities are received */
  sys_plmn_id_s_type    plmn;

  /* Dedicated priority info */
  lte_irat_dedicated_priority_info_s dedicated_pri_info;

} tdsrrc_dedicated_priorities_rsp_type;

/*****************************************************************************/
/* TDSRR_DEDICATED_PRIORITIES_REQ                                               */
/* FROM: TDSCDMA RRC                                                           */
/* TO:   GERAN RR                                                            */
/* DESCRIPTION:                                                              */
/*   Request for a copy of RR's dedicated priority information and           */
/*   associated PLMN.                                                        */
/*****************************************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} tdsrr_dedicated_priorities_req_type;

/*****************************************************************************/
/* TDSRR_CLEAR_DEDICATED_PRIORITIES_IND                                         */
/* FROM: TDSCDMA RRC                                                           */
/* TO:   GERAN RR                                                            */
/* DESCRIPTION:                                                              */
/*   Indicates that RR should clear it's dedicated priority information.     */
/*   Used when RRC becomes active, if RR has ownership of dedicated          */
/*   priorities as indicated by lte_rrc_dedicated_pri_get_owner().           */
/*****************************************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} tdsrr_clear_dedicated_priorities_ind_type;

/***************************************************************************/
/****************  TDSRRC Message Union ************************************/
/***************************************************************************/
typedef union
{
  /**************************************************************/
  /******** RR Procedure ID's received by TDS RRC ***************/
  /**************************************************************/
  tdsrr_interrat_handover_cnf_type         interrat_handover_cnf;
  tdsrr_interrat_handover_abort_cnf_type   interrat_handover_abort_cnf;
  tdsrr_interrat_reselect_rej_type         interrat_reselection_rej; 
  tdsrr_interrat_redirect_rej_type         interrat_redirect_rej;
  tdsrr_interrat_redirect_abort_cnf_type   interrat_redirect_abort_cnf;
  tdsrr_interrat_plmn_srch_cnf_type        interrat_plmn_srch_cnf;
  tdsrr_interrat_cc_order_cnf_type         interrat_cc_order_cnf; 
  tdsrr_interrat_cc_order_abort_cnf_type   interrat_cc_order_abort_cnf;
  tdsrr_interrat_cc_order_rej_type         interrat_cc_order_rej;
  tdsrr_stop_gsm_cnf_type                  rr_rrc_stop_gsm_cnf;

  tdsrr_cipher_sync_ind_type                  cipher_sync_ind;

  /**************************************************************/
  /******** TDSRRC Procedure ID's received by TDS RRC ***********/
  /**************************************************************/
  tdsrrc_interrat_reselect_req_type        interrat_reselection_req;
  tdsrrc_interrat_handover_req_type        interrat_handover_req;
  tdsrrc_interrat_handover_abort_req_type  interrat_handover_abort_req;
  tdsrrc_nas_data_ind_type                 interrat_nas_data_ind;
  tdsrrc_interrat_cc_order_req_type        interrat_cc_order_req;
  tdsrrc_interrat_plmn_srch_req_type       interrat_plmn_srch_req;
  tdsrrc_interrat_plmn_srch_abort_req_type interrat_plmn_srch_abort_req;
  tdsrrc_interrat_redirect_req_type        interrat_redirect_req;
  tdsrrc_rr_stop_tds_req_type           tdsrrc_rr_stop_tds_req;
  tdsrr_dedicated_priorities_rsp_type dedicated_priorities_rsp;
} tdsrrc_rr_cmd_data_u_type;

typedef struct
{
  q_link_type link;
  tdsrrc_cmd_e_type  cmd_id;
  sys_modem_as_id_e_type as_id;
} tdsrrc_rr_cmd_hdr_type;

typedef struct
{
  /* RRC RR command header */
  tdsrrc_rr_cmd_hdr_type cmd_hdr;

  /* RRC RR command data */
  tdsrrc_rr_cmd_data_u_type cmd;
} tdsrrc_rr_cmd_type;

/* -----------------------------------------------------------------------
** Function Prototypes
** ----------------------------------------------------------------------- */

/*===========================================================================

FUNCTION TDSRRC_GET_RR_CMD_BUF

DESCRIPTION

  This function returns a RR command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling tdsrrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type tdsrrc_rr_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_rr_cmd_type *tdsrrc_get_rr_cmd_buf( void );

/*===========================================================================

FUNCTION TDSRRC_PUT_RR_CMD

DESCRIPTION

  This function puts a command buffer into RRC's RR
  command queue. Note that the command buffer must have been
  requested through a call to tdsrrc_get_rr_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void tdsrrc_put_rr_cmd
(
  tdsrrc_rr_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);

/*===========================================================================

FUNCTION TDSRRC_WRITE_NV_ACQ_INFO

DESCRIPTION

  This function is called by RR when it gets RR_DEACT_REQ from MM,
  so that RRC can write the acquisition database information to NV.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_write_nv_acq_info( void );

/*===========================================================================

FUNCTION TDSRRC_WRITE_NV_ACQ_INFO_SUB_ID

DESCRIPTION

  This function serves for the same purpose as tdsrrc_write_nv_acq_info, with TDS
  as_id provided for Dual_SIM.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_write_nv_acq_info_sub_id(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION: TDSRRC_UNBAR_FLAI_CELLS

DESCRIPTION:
           This function is called by RR upon the initiation of any cell selection, to unbar all the barred frequencies 
           because of partial roaming restrictions

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
=============================================================================*/
void tdsrrc_unbar_flai_cells(void);

/*===========================================================================

FUNCTION: TDSRRC_UNBAR_FLAI_CELLS_SUB_ID

DESCRIPTION:
  This function serves for the same purpose as tdsrrc_unbar_flai_cells, with TDS
  as_id provided for Dual_SIM.

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
=============================================================================*/
void tdsrrc_unbar_flai_cells_sub_id(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION TDSRRC_GET_CLASSMARK_CHANGE_HANDOVER_INFO

DESCRIPTION

  This function builds a INTERRAT HANDOVER INFO message type which will be
  sent by GSM to UTRAN thru GERAN.  An ASN1 encoded message and message
  length is populated by RRC, the message and msg_length is used by RR to
  send this as part of Classmark Change to UTRAN via GERAN
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

===========================================================================*/
void tdsrrc_get_classmark_change_handover_info
(
  byte ** msg_ptr,    /* The command to be sent to UTRAN */
  uint8 * msg_len    /* Length of the message           */
  , boolean irat_ho_info_compressed

);

/*===========================================================================

FUNCTION TDSRRC_GET_CLASSMARK_CHANGE_HANDOVER_INFO_SUB_ID

DESCRIPTION

  This function serves for the same purpose as tdsrrc_get_classmark_change_handover_info, with TDS
  as_id provided for Dual_SIM.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

===========================================================================*/
void tdsrrc_get_classmark_change_handover_info_sub_id
(
  byte ** msg_ptr,  /* The command to be sent to UTRAN */
  uint8 * msg_len,  /* Length of the message */
  boolean irat_ho_info_compressed,
  sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION TDSRRC_DELETE_CLASSMARK_CHANGE_HANDOVER_INFO

DESCRIPTION

  This function deletes the CLASSMARK_CHANGE_HANDOVER_INFO asn1 type
  which was previously allocated to send to UTRAN via GERAN.
  RR calls this function after it has copied the contents of the
  Handover To Utran Info contents locally.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tdsrrc_delete_classmark_change_handover_info(void);

/*===========================================================================

FUNCTION TDSRRC_DELETE_CLASSMARK_CHANGE_HANDOVER_INFO_SUB_ID

DESCRIPTION

  This function serves for the same purpose as tdsrrc_delete_classmark_change_handover_info, with TDS
  as_id provided for Dual_SIM.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tdsrrc_delete_classmark_change_handover_info_sub_id(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION TDSRRC_UPDATE_BAND_PREFERENCE

DESCRIPTION

  This function is called by RR to update the band preference
  in RRC.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
===========================================================================*/
void tdsrrc_update_band_preference
(
  sys_band_mask_type band_pref_mask  /* band mask */
);

/*===========================================================================

FUNCTION TDSRRC_UPDATE_BAND_PREFERENCE_SUB_ID

DESCRIPTION

  This function serves for the same purpose as tdsrrc_update_band_preference, with TDS
  as_id provided for Dual_SIM.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
===========================================================================*/
void tdsrrc_update_band_preference_sub_id
(
  sys_band_mask_type band_pref_mask,  /* band mask */
  sys_modem_as_id_e_type  as_id
);

/*===========================================================================

FUNCTION TDSRRC_IS_ARFCN_VALID

DESCRIPTION

  This function is called by RR to validate the UTRAN TDD ARFCN it
  reads from the SIs. RRC validates the range and band of the ARFCN.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If ARFCN is valid,
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrc_is_arfcn_valid
(
  uint32 UARFCN
);

/*===========================================================================

FUNCTION TDSRRC_IS_ARFCN_VALID_SUB_ID

DESCRIPTION

  This function serves for the same purpose as tdsrrc_is_arfcn_valid, with TDS
  as_id provided for Dual_SIM.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If ARFCN and PLMN_id combination is valid,
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrc_is_arfcn_valid_sub_id
(
  uint32 UARFCN,
  sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION TDSRRC_SET_UE_IN_DEEP_SLEEP

DESCRIPTION

  This function is called by RR to inform RRC that UE is going to DEEP SLEEP
  state.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_set_ue_in_deep_sleep( boolean value);

/*===========================================================================

FUNCTION TDSRRC_SET_UE_IN_DEEP_SLEEP_SUB_ID

DESCRIPTION

  This function serves for the same purpose as tdsrrc_set_ue_in_deep_sleep, with TDS
  as_id provided for Dual_SIM.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_set_ue_in_deep_sleep_sub_id( boolean value,sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION: TDSRRC_CHECK_CELL_BARRED

DESCRIPTION:
  This function is called by RR to check if a particular UTRAN TDD cell is 
  barred or not

DEPENDENCIES:
  NONE

RETURN VALUE:
  UTRAN TDD cell is not barred, or only the PSC is barred, or the frequency
  is barred

SIDE EFFECTS:
  NONE
====================================================================*/
tdsrrc_cell_bar_info_e_type tdsrrc_check_cell_barred
(
  uint16 freq, uint8 cpid
);

/*===========================================================================

FUNCTION: TDSRRC_CHECK_CELL_BARRED_SUB_ID

DESCRIPTION:
  This function serves for the same purpose as tdsrrc_check_cell_barred, with TDS
  as_id provided for Dual_SIM.

DEPENDENCIES:
  NONE

RETURN VALUE:
  TDS cell is not barred, or only the CPID is barred, or the frequency
  is barred

SIDE EFFECTS:
  NONE
====================================================================*/
tdsrrc_cell_bar_info_e_type tdsrrc_check_cell_barred_sub_id
(
  uint16 freq, uint8 cpid, sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION          TDSRRC_GET_INTER_RAT_BPLMN_PRTL_RESULT

DESCRIPTION       RR Calls this API to get Partial PLMN results from TDSRRC during ongoing GtoTD
                  Manual PLMN Search

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
boolean tdsrrc_get_inter_rat_bplmn_prtl_result(
  sys_detailed_plmn_list_s_type *available_plmn_list
);

/*===========================================================================

FUNCTION          TDSRRC_GET_INTER_RAT_BPLMN_PRTL_RESULT_SUB_ID

DESCRIPTION       
  This function serves for the same purpose as tdsrrc_get_inter_rat_bplmn_prtl_result, with TDS
  as_id provided for Dual_SIM.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
boolean tdsrrc_get_inter_rat_bplmn_prtl_result_sub_id(
  sys_detailed_plmn_list_s_type *available_plmn_list,
  sys_modem_as_id_e_type as_id
);
#endif /* TDSRRC_RR_IF_H */

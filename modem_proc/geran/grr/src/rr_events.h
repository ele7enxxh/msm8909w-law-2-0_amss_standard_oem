#ifndef RR_EVENTS_H
#define RR_EVENTS_H
/*============================================================================
  @file rr_events.h

  Events

  Copyright (c) 2008-2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_events.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/
#include "geran_variation.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/* rr_event_T: The following list of enums are numbered. The numbering is of
no particular relevance, and number assignments may be changed without risk.
The purpose is solely to aid debugging, so events printed by number in debug
messaging can be looked up */
typedef enum
{
  /**********************************************/
  /* Generic state machine events               */
  /**********************************************/

  EV_RESET_SOFTWARE                          = 0x00,
  EV_NO_EVENT                                = 0x01,
  EV_INPUT_MESSAGE                           = 0x02,

  /**********************************************/
  /* Events generated during sys info processing*/
  /**********************************************/

  RR_EV_GSM_SI_RECEIVED                      = 0x04,
  RR_EV_EARLY_CAMP_SI_RECEIVED               = 0x05,
  RR_EV_SYS_INFO_TIMEOUT                     = 0x06,
  RR_EV_CANDIDATE_CELL_SI_RECEIVED           = 0x07,
  RR_EV_GPRS_SI13_RECEIVED                   = 0x08,
  RR_EV_GPRS_SYS_INFO_TIMEOUT                = 0x09,
  RR_EV_SURROUNDING_SI_RECEIVED              = 0x0a,
  RR_EV_ACCESS_CLASS_CHANGED                 = 0x0b,
  RR_EV_SYS_INFO_EXPIRY_DURING_CS_CONN       = 0x0c,

  /**********************************************/
  /* Cell selection events                      */
  /**********************************************/

  RR_EV_PERFORM_ACQ_DB_POWER_SCAN            = 0x10,
  RR_EV_PERFORM_POWER_SCAN                   = 0x11,
  RR_EV_SPECIFIC_CHANNEL_SEARCH              = 0x12,
  RR_EV_DECODING_BCCH                        = 0x13,
  RR_EV_CHANNEL_FOUND                        = 0x14,
  RR_EV_NO_CELL_SELECTION_REQUIRED           = 0x15,
  RR_EV_CELL_SELECTION_ABORT_TIMEOUT         = 0x16,
  RR_EV_ACQUISITION_FAILURE                  = 0x17,
  RR_EV_CAMPED_ON_CELL                       = 0x18,
  RR_EV_CAMPED_ON_CELL_W2G_CCO               = 0x19,
  EV_CHECK_GPRS                              = 0x1a,
  EV_START_PBCCH_ACQUISITION                 = 0x1c,
  RR_EV_TRIGGER_CELL_SELECTION               = 0x1d,
  EV_CELL_SELECTION_TRIGGERED                = 0x1e,
  EV_L1_IDLE_MODE_CNF_RECEIVED               = 0x1f,
  RR_EV_PROCESS_PLMN_SELECT_REQ              = 0x20,
  EV_CELL_SELECTION_ABORTED                  = 0x21,
  RR_EV_ABORT_CELL_SELECTION                 = 0x22,
  RR_EV_REPROCESS_CANDIDATE_CELL_DB          = 0x23,
  RR_EV_DECODING_ACQ_DB_BCCH                 = 0x24,
  EV_USE_EXISTING_POWER_SCAN_RESULTS         = 0x25,

  /**********************************************/
  /* RR/GRR control events                      */
  /**********************************************/

  EV_RR_EST_L2_CONT_RES_FAIL                 = 0x30,
  EV_RR_EST_L1_IN_DED                        = 0x31,
  EV_GRR_RPS_STARTED                         = 0x32,
  EV_GO_TO_IDLE                              = 0x33,
  EV_GO_TO_IDLE_PLMN_SELECTION               = 0x34,
  EV_NO_BCCH_FOUND                           = 0x35,
  RR_EV_DECODE_BCCH_COMPLETE_TIMEOUT         = 0x36,
  EV_RR_EST_REQ                              = 0x37,
  EV_TRIGGER_CELL_SELECTION                  = 0x38,
  EV_FORCE_CELL_RESELECTION                  = 0x39,
  EV_GRR_EST_REQ                             = 0x3a,
  EV_RR_EST_CNF                              = 0x3b,
  EV_RR_EST_IND                              = 0x3c,
  EV_CHOOSE_CELL                             = 0x3d,
  EV_ABNORMAL_RELEASE_PART_SI                = 0x3e,
  EV_ABNORMAL_RELEASE_FULL_SI                = 0x3f,
  RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED         = 0x40,
  EV_GPRS_SUSPEND                            = 0x41,
  EV_GPRS_SUSPENDED                          = 0x42,
  EV_GPRS_SUSPEND_FOR_SI_ACQUISITION         = 0x43,
  EV_GPRS_SUSPENDED_FOR_SI_ACQUISITION       = 0x44,
  EV_PROCESS_SAVED_MESSAGE                   = 0x45,
  RR_EV_PROCESS_SAVED_MESSAGE                = 0x46,
  RR_EV_MM_REQ_RECEIVED_ABORT                = 0x47,
  EV_NON_CAMPED_SYS_INFO_ACQUISITION         = 0x48,
  EV_GPRS_SUSPEND_FOR_SI_ACQUISITION_NO_SI13 = 0x49,

  /**********************************************/
  /* Cell reselection events                    */
  /**********************************************/
  EV_CELL_RESELECTION                        = 0x50,
  EV_NO_CELL_RESELECTION                     = 0x51,
  EV_PATH_LOSS_TIMEOUT                       = 0x52,
  EV_ABANDON_RESELECTION                     = 0x53,
  EV_MAC_IDLE_MODE                           = 0x54,
  EV_RECENT_RESELECTION_TIMEOUT              = 0x55,
  EV_CELL_RESELECTION_ACT                    = 0x56,
  EV_CELL_RESELECTION_DEACT                  = 0x57,
  EV_ACTION_PCCO                             = 0x58,
  EV_ACTION_IMMEDIATE_PCCO                   = 0x59,
  EV_T3174_TIMEOUT                           = 0x5a,
  EV_T3176_TIMEOUT                           = 0x5b,
  EV_DSF_RESELECTION                         = 0x5c,
  EV_MAC_GUARD_TIMER_EXPIRED                 = 0x5d,
  EV_CAMP_ON_PCCF_OLD_SCELL                  = 0x5e,
  RR_EV_START_RESELECTION_POWER_SCAN         = 0x5f,
  EV_START_CCN_CELL_RESELECTION              = 0x60,
  EV_RESUME_CELL_RESELECTION                 = 0x61,
  EV_T3208_EXPIRY                            = 0x62,
  EV_T3210_EXPIRY                            = 0x63,
  EV_ALL_NACC_PNCD_DISPATCHED                = 0x64,
  EV_ACTION_PCCO_WITH_PNCD                   = 0x65,
  EV_PS_ACCESS_BLOCKED                       = 0x66,

  /**********************************************/
  /* Measurement reporting events               */
  /**********************************************/

  /* External input/output events */
  RR_EV_DEACTIVATE_MEASUREMENTS              = 0x70,
  RR_EV_MEASUREMENT_PARAMS_UPDATED           = 0x71,
  RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13      = 0x72,

  /* Internal events */
  EV_T3158_TIMEOUT                           = 0x73,
  EV_PMO_MESSAGE                             = 0x74,
  EV_PCCO_MESSAGE                            = 0x75,
  EV_MAC_TRANSFER                            = 0x76,
  EV_MAC_IDLE                                = 0x77,
  EV_MAC_IDLE_BLOCKED                        = 0x78,
  EV_NCO_MODE_0                              = 0x79,
  EV_NCO_MODE_1                              = 0x7a,
  EV_NCO_MODE_2                              = 0x7b,

  /**
   * Additional G2W cell reselection events
   */
  EV_CR_G2W_TIMER_EXPIRED                    = 0x7c,

  /**********************************************/
  /* Connection Establishment events            */
  /**********************************************/

  EV_INIT_ACCESS_PROCEDURE                   = 0x90,
  EV_ACCESS_CLASS_NOT_ALLOWED                = 0x91,
  EV_IMMEDIATE_ASSIGNMENT                    = 0x92,
  EV_MAX_RETRIES_ATTEMPTED                   = 0x93,
  EV_ASSIGNMENT_REJECTION                    = 0x94,
  EV_CHANNEL_DEACTIVATED                     = 0x95,
  EV_T3126_EXPIRY                            = 0x96,
  EV_RACH_FOR_CS_FAILURE                     = 0x98,
  EV_RACH_FOR_PS_FAILURE                     = 0x99,
  EV_CONNECTION_ABORTION                     = 0x9a,
  EV_CONNECTION_RELEASED                     = 0x9b,
  EV_GPRS_IMMEDIATE_ASSIGNMENT               = 0x9c,
  EV_GPRS_INIT_ACCESS_PROCEDURE              = 0x9d,
  EV_GPRS_ACCESS_NOT_POSSIBLE                = 0x9e,
  EV_T3146_EXPIRY                            = 0x9f,
  EV_TMULTIFRM_EXPIRY                        = 0xa0,
  EV_GPRS_RESET_RR_CE_STATE                  = 0xa3,
  EV_GPRS_CONNECTION_ABORTION                = 0xa4,
  EV_GRR51_GO_TO_PACKET_IDLE                 = 0xa5,
  EV_REPEAT_ASSIGNMENT_PROCEDURE             = 0xa6,
  EV_T3174_EXPIRY                            = 0xa7,
  EV_T3176_EXPIRY                            = 0xa8,
  EV_TBF_CONN_EST_FAILURE                    = 0xa9,
  EV_TBF_CONN_EST_ABORTED_CAMPED             = 0xaa,
  EV_TBF_CONN_EST_ABORTED_NON_CAMPED         = 0xab,
  EV_START_IDLE                              = 0xac,

  /**********************************************/
  /* Connection release events                  */
  /**********************************************/

  EV_CONNECTION_EXISTS                       = 0xb0,
  EV_T3110_TIMEOUT                           = 0xb1,
  RR_EV_CONNECTION_RELEASE_IN_PROGRESS       = 0xb2,
  EV_CHOOSE_CELL_FOR_REEST                   = 0xb4,
  EV_CHOOSE_CELL_FOR_CAMP                    = 0xb5,
  RR_EV_CELL_REEST_TRIGGERED                 = 0xb6,

  /**********************************************/
  /* Dispatch events                            */
  /**********************************************/

  EV_NEW_MESSAGE                             = 0xb8,
  EV_INTER_CELL_HANDOVER                     = 0xb9,
  EV_INTRA_CELL_HANDOVER                     = 0xba,
  EV_G2W_HANDOVER                            = 0xbb,
  EV_CIPHERING                               = 0xbc,
  EV_HANDOVER_TERMINATED                     = 0xbd,
  EV_CIPHERING_TERMINATED                    = 0xc0,
  EV_ASSIGNMENT_COMMAND_CHECKED              = 0xc1,
  EV_HANDOVER_COMMAND_CHECKED                = 0xc2,
  EV_T3124_TIMEOUT                           = 0xc3,
  EV_HANDOVER_COMPLETED                      = 0xc4,

  /**********************************************/
  /* Other Dedicated events                     */
  /**********************************************/

  EV_CIPHER_COMMAND_CHECKED                  = 0xc8,
  EV_MODE_MODIFY_CHECKED                     = 0xc9,
  EV_RADIO_LINK_FAILURE                      = 0xca,
  EV_PROCEDURE_COMPLETE                      = 0xcb,

  /**********************************************/
  /* RR Mode control events                     */
  /**********************************************/

  EV_ACTIVATE_GSM_MODE                       = 0xd0,
  EV_DEACTIVATE_GSM_MODE                     = 0xd1,
  EV_GSM_MODE_ACTIVATED                      = 0xd2,
  EV_GSM_MODE_DEACTIVATED                    = 0xd3,
  EV_PANIC_RESET                             = 0xd4,
  EV_ACTIVATE_GSM_MODE_FOR_X2G_RESEL         = 0xd5,
  EV_ACTIVATE_GSM_MODE_FOR_X2G_CCO           = 0xd6,

  /**********************************************/
  /* InterRAT WCDMA To GSM Handover events      */
  /**********************************************/

  RR_EV_W2G_HO_ABORTED                       = 0xd8,
  RR_EV_W2G_HO_COMPLETED                     = 0xd9,
  RR_EV_W2G_GO_TO_INACTIVE                   = 0xda,
  RR_EV_W2G_HO_PROTOCOL_ERROR                = 0xdb,

  /*********************************************/
  /* InterRAT WCDMA to GSM Reselection events  */
  /*********************************************/

  RR_EV_W2G_NO_SUITABLE_CELL_FOUND           = 0xe0,
  RR_EV_W2G_SUITABLE_CELL_FOUND              = 0xe1,
  RR_EV_W2G_PCCO_FAILED                      = 0xe4,

  /****************************************************/
  /* InterRAT WCDMA to GSM Service Redirection events */
  /****************************************************/
  RR_EV_TRIGGER_CELL_SELECTION_W2G            = 0xe8,
  RR_EV_SERVICE_REDIRECTION_WAIT_TIMER_EXPIRY = 0xe9,
  RR_EV_ABORT_PROCEDURE                       = 0xea,
#ifdef FEATURE_GSM_TDS
  RR_EV_T2G_REDIR_ABORT_PROCEDURE             = 0xeb,
#endif /*FEATURE_GSM_TDS*/

  /*********************************************/
  /* InterRAT WCDMA to GSM PLMN SEARCH events  */
  /*********************************************/
  EV_ACTIVATE_GSM_MODE_FOR_BPLMN             = 0xed,

  /*********************************************/
  /* InterRAT GSM to WCDMA Reselection events  */
  /*********************************************/

  RR_EV_FORCE_WCDMA_CELL_RESELECTION         = 0xf0,
  RR_EV_G2WR_RESELECTION_PSCAN_STARTED       = 0xf1,
  RR_EV_RESELECT_WCDMA_CELL                  = 0xf2,
  RR_EV_G2WR_GO_TO_INACTIVE                  = 0xf3,
  RR_EV_G2WR_GOING_INACTIVE                  = 0xf4,

  /**********************************************/
  /* InterRAT GSM To WCDMA Handover events      */
  /**********************************************/

  RR_EV_G2W_HO_STARTED                       = 0xf8,
  RR_EV_G2W_HO_ABORTED                       = 0xf9,
  RR_EV_G2W_HO_FAIL                          = 0xfa,
  RR_EV_G2W_HO_SUCCESS                       = 0xfb,
#ifdef FEATURE_INTERRAT_PCCO_GTOW
  RR_EV_G2W_PCCO_STARTED                     = 0xfc,
  RR_EV_G2W_PCCO_FAILED                      = 0xfd,
#endif /* FEATURE_INTERRAT_PCCO_GTOW */

  EV_CELL_CHOSEN                             = 0xfe,
  EV_CELL_CHOSEN_EARLY_CAMPED                = 0xff,

  /**********************************************/
  /* DTM events                                 */
  /**********************************************/

#ifdef FEATURE_GSM_DTM
  EV_DTM_CS_RELEASED                         = 0x0101,
  EV_DTM_CS_RELEASE_REQUIRED                 = 0x0102,
  EV_DTM_PERFORM_GSM_RELEASE                 = 0x0103,
  EV_DTM_PERFORM_CSPS_RELEASE                = 0x0104,
  EV_DTM_PERFORM_GSM_RELEASE_GTTP            = 0x0105,
  EV_DTM_CS_EST_CNF                          = 0x0106,
  EV_DTM_CS_EST_CNF_UL_TBF_REQ_PENDING       = 0x0107,
  EV_DTM_PS_EST_CNF                          = 0x0108,
  EV_DTM_CSPS_EST_CNF                        = 0x0109,
  EV_DTM_RR_EST_CNF                          = 0x010a,
  EV_DTM_RR_EST_FAIL                         = 0x010b,
  EV_DTM_ASSIGNMENT_COMMAND_RECEIVED         = 0x010c,
  EV_DTM_PACKET_ASSIGNMENT_RECEIVED          = 0x010d,
  EV_DTM_REJECT_RECEIVED                     = 0x010e,
  EV_DTM_ASSIGNMENT_COMMAND_CHECKED          = 0x010f,
  EV_DTM_PACKET_ASSIGNMENT_CHECKED           = 0x0110,
  EV_DTM_ASSIGNMENT_ERROR                    = 0x0111,
  EV_DTM_ASSIGNMENT_FAILURE                  = 0x0112,
  EV_DTM_ACCESS_REJECT                       = 0x0113,
  EV_DTM_ASSIGNMENT_RECEIVED                 = 0x0114,
  EV_DTM_ASSIGNMENT_COMPLETE                 = 0x0115,
  EV_DTM_T3148_EXPIRY                        = 0x0116,
  EV_DTM_SI6_RECEIVED                        = 0x0117,
  EV_DTM_INFORMATION_RECEIVED                = 0x0118,
  EV_DTM_CELL_SUPPORTS_DTM                   = 0x0119,
  EV_DTM_CELL_DOES_NOT_SUPPORT_DTM           = 0x011a,
  EV_DTM_ACCESS_REJECT_T3142_RUNNING         = 0x011b,
  EV_DTM_L2_DATA_ACK_IND                     = 0x011c,
#ifdef FEATURE_WCDMA
  EV_DTM_W2G_HO                              = 0x011d,
  EV_DTM_W2G_HO_PS_ATTACHED                  = 0x011e,
#endif // FEATURE_WCDMA
  EV_DTM_UL_TBF_REQ                          = 0x011f,
  EV_DTM_NO_RESPONSE_TO_DTM_REQUEST          = 0x0120,
  EV_DTM_ASSIGNMENT_READY                    = 0x0121,
  EV_DTM_PS_ACCESS_REQ                       = 0x0122,
  EV_DTM_PS_ACCESS_REQ_PURGE                 = 0x0123,
  EV_DTM_NO_PS_ACCESS_REQ                    = 0x0124,
  EV_DTM_MSC_CHANGE_IND                      = 0x0125,
  EV_DTM_ASSIGNMENT_READY_IN_DTM             = 0x0126,
  EV_DTM_CHN_ASSIGN_REQ_GUARD_TIMER          = 0x0127,
  EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL            = 0x0128,
  EV_DTM_ASSIGNMENT_ERROR_IN_DTM             = 0x0129,
  EV_DTM_ASSIGNMENT_ERROR_PS_ENABLED         = 0x012a,
  EV_DTM_ASSIGNMENT_ERROR_PS_BLOCKED         = 0x012b,

#ifdef FEATURE_GSM_EDTM
  EV_EDTM_DTM_ASSIGNMENT_RECEIVED            = 0x0130,
  EV_EDTM_IMM_ASSIGNMENT_RECEIVED            = 0x0131,
  EV_EDTM_IMM_ASSIGNMENT_REJECT_RECEIVED     = 0x0132,
  EV_EDTM_ESTABLISHMENT_STARTED              = 0x0133,
  EV_EDTM_ESTABLISHMENT_REJECTED             = 0x0134,
  EV_EDTM_ESTABLISHMENT_COMPLETED            = 0x0135,
  EV_EDTM_ESTABLISHMENT_ABORTED              = 0x0136,
  EV_EDTM_T3196_EXPIRY                       = 0x0137,
  EV_EDTM_RELEASE_ABORTED                    = 0x0138,
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */


  /**
   * PCCO events.
   */
  EV_PCCO_FAILED                             = 0x0161,
  EV_PS_ACCESS_IND                           = 0x0162,
  EV_GMM_LEFT_READY_STATE                    = 0x0163,

  /* Sys Info Handler events */

  EV_SI_ACTIVATE_CAMPED_HANDLING             = 0x0170,
  EV_SI_SYS_INFO_MESSAGE_RECEIVED            = 0x0171,

#ifdef FEATURE_GPRS_PS_HANDOVER
  /**
   * PS Handover events.
   */
  EV_ACTION_PSHO                           = 0x0180,
  EV_PSHO_STARTED                          = 0x0181,
  EV_PSHO_CONTINUE                         = 0x0182,
  EV_PSHO_REJECTED                         = 0x0183,
  EV_PSHO_COMPLETE                         = 0x0184,



#endif /* FEATURE_GPRS_PS_HANDOVER */

  EV_CHOOSE_CELL_BY_CELL_SELECTION_INDICATOR = 0x0200, /* from rr_conn_release to rr_control */
  EV_START_G2W_REDIRECTION                   = 0x0201, /* from rr_control to rr_g2w_redirection */
  EV_TRIGGER_CELL_SELECTION_BY_REL_IND       = 0x0202, /* from rr_control to rr_cell_selection */
#ifdef FEATURE_LTE
  EV_G2L_BLIND_REDIR_REQUIRED                = 0x0203,
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
  EV_G2T_PSEUDO_FR_REQUIRED                  = 0x0204,
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

  EV_PERFORM_MULTIPASS_SEARCH                = 0x0210,
  EV_MULTIPASS_SEARCH_FAILED                 = 0x0211,

  /**********************************************
   * Below are PLMN re-engineering events,
   * could be replaced by rr-rr messages
  **********************************************/
   
  /**********************************************/
  /* PLMN SEARCH events                         */
  /**********************************************/
  /* rr -> rr_plmn_list_control() */
  EV_PLMN_SEARCH_START                    = 0x0301,
  EV_PLMN_SEARCH_ABORT                    = 0x0302,
  
#ifdef FEATURE_WCDMA
  EV_PLMN_SEARCH_START_WTOG               = 0x0303,
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
  EV_PLMN_SEARCH_START_LTOG               = 0x0304,
#endif /* FEATURE_LTE */
  
  /* rr_plmn_list_control() -> rr */
  EV_PLMN_SEARCH_COMPLETE                 = 0x0305,

  EV_GRR_PLMN_SEARCH_READY_TO_START       = 0x0306,

  EV_PLMN_SEARCH_SUSPEND                  = 0x0307,

#ifdef FEATURE_GSM_TDS
  EV_PLMN_SEARCH_START_TTOG               = 0x0308,
#endif /*FEATURE_GSM_TDS*/

  EV_PLMN_LIST_SEARCH_GUARD_TIMER_EXPIRED = 0x0309,
#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
  EV_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER_EXPIRED = 0x030a,
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

  /**********************************************/
  /* DECODE BCCH LIST events                    */
  /**********************************************/
  /* Internal event used to avoid duplicate code */
  EV_DECODE_BCCH_LIST_UPDATE              = 0x0324,

  EV_PCCO_FAILURE_CCO_TIMER_EXPIRY        = 0x0380,
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  EV_X2G_CCO_FAILURE_CCO_TIMER_EXPIRY              = 0x0400,
  EV_X2G_CCO_FAILURE_NO_RESPONSE                   = 0x0401,
  EV_X2G_CCO_FAILURE_IA_REJECT                     = 0x0402,
  EV_X2G_CCO_FAILURE_CONTENTION_RESOLUTION_FAILURE = 0x0403,
  EV_X2G_CCO_FAILURE_DSF                           = 0x0404,
  EV_X2G_CCO_ABORTED_FOR_HIGH_PRI_REQ              = 0x0410,
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  EV_PANIC_RESET_REQ                               = 0x0500,
  EV_PANIC_RESET_CNF                               = 0x0501,
  EV_PANIC_RESET_CNF_RR_ACTIVATION_IND_SENT        = 0x0502,

  EV_MAC_PS_ACCESS_GUARD_TIMER_EXPIRED    = 0x0601,

#ifdef FEATURE_SGLTE
  EV_SGLTE_X2G_CCO_FAILED_GO_TO_IDLE      = 0x0700,
  EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT    = 0x0701,
  EV_SGLTE_G2L_BLIND_REDIR_REQUIRED       = 0x0702,
  EV_SGLTE_G2X_REDIR_COMPLETE             = 0x0703,
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
    EV_IRAT_SURROUND_MEAS_IND               = 0x0800,
    EV_G2X_CONN_RESEL_ABORT_REQ             = 0x0801,
    EV_G2X_CONN_RESEL_ABORT_CNF             = 0x0802,
    EV_G2X_CONN_RESEL_START_RESEL_EVAL      = 0x0803,
    EV_G2X_CONN_RESEL_START_RESELECTION     = 0x0804,
    EV_G2X_CONN_RESEL_PS_BLOCKED            = 0x0805,
#endif /* FEATURE_SGLTE_CONN_G2X_RESEL */
    EV_DSF_FOR_DENIALS                      = 0x0806,

#ifdef FEATURE_IDLE_DRX_SCALING
    EV_IDLE_DRX_DSC_BELOW_TH_LOW            = 0x0807,
    EV_IDLE_DRX_DSC_ABOVE_TH_HIGH           = 0x0808,
    EV_IDLE_DRX_SCALING_SUSPEND_TIMEOUT     = 0x0809,
    EV_IDLE_DRX_BS_PA_MFRMS_CHANGED         = 0x080A,
#endif /* FEATURE_IDLE_DRX_SCALING */

  RR_EV_NUM_OF_EVENTS

} rr_event_T;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/


#endif /* #ifndef RR_EVENTS_H */

/* EOF */


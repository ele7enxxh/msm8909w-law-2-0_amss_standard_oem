#ifndef RR_TDS_RRC_IF_H
#define RR_TDS_RRC_IF_H
/*===========================================================================
         TDSRRC  -  RR   INTERFACE 

DESCRIPTION

  This module is the header module that contains the type definitions for
  the primitives sent to GERAN RR from TD-SCDMA RRC. The module is only used for dual-mode.
  
Copyright (c) 2002-2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/rr_tdsrrc_if.h#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/13   cdf     Added changes to support suspension of intra/inter-TDS BPLMN 
                   search on entering DCH/FACH and resumption of the same on
                   entering PCH/IDLE
04/30/13   hx      Made chagnes to send TDSRR_CIPHER_SYNC_IND to G whenever new keys are applied 
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
#include "sys_type.h"

#include "tdsrrc_rr_if.h"

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef enum
{
  /**************************************************************/
  /******** RR Procedure ID's received by RR ********************/
  /**************************************************************/

  /****** TD -> RR HANDOVER ************/
  TDSRR_INTERRAT_HANDOVER_REQ,
  TDSRR_INTERRAT_HANDOVER_ABORT_REQ,
  TDSRR_NAS_DATA_IND,

  /****** TD -> RR RESELECTION ************/
  TDSRR_INTERRAT_RESELECTION_REQ,
  TDSRR_INTERRAT_RESELECTION_ABORT_REQ,

  /****** TD -> RR PLMN SEARCH ************/
  TDSRR_INTERRAT_PLMN_SRCH_REQ,
  TDSRR_INTERRAT_PLMN_SRCH_ABORT_REQ,
  TDSRR_INTERRAT_PLMN_SRCH_SUSPEND_IND,

  /****** TD -> RR CC ORDER ************/  
  TDSRR_INTERRAT_CC_ORDER_REQ,
  TDSRR_INTERRAT_CC_ORDER_ABORT_REQ,

  /****** TD -> RR REDIRECTION ************/
  TDSRR_INTERRAT_REDIRECT_REQ,
  TDSRR_INTERRAT_REDIRECT_ABORT_REQ,

  /****** TD -> RR Stop Mode ************/
  TDSRR_STOP_GSM_REQ,

  /****** TD -> RR TL1 Stop Ind *********/
  TDSRR_INTERRAT_TL1_STOP_IND,

  /****** TD -> RR Indicate new keys are activated at TDS ******/
  TDSRR_CIPHER_SYNC_IND,

  /**************************************************************/
  /******** TDS RRC Procedure ID's received by RR ***************/
  /**************************************************************/

  /****** RR -> TD HANDOVER ************/  
  TDSRRC_INTERRAT_HANDOVER_CNF,
  TDSRRC_INTERRAT_HANDOVER_ABORT_CNF,

  /****** RR -> TD RESELECTION ************/
  TDSRRC_INTERRAT_RESELECTION_REJ,
  TDSRRC_INTERRAT_RESELECTION_ABORT_CNF,

  /****** RR -> TD PLMN SEARCH ************/
  TDSRRC_INTERRAT_PLMN_SRCH_CNF,
  TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF,

  /****** RR -> TD CC ORDER ************/ 
  TDSRRC_INTERRAT_CC_ORDER_CNF,

  /****** RR -> TD REDIRECTION ************/
  TDSRRC_INTERRAT_REDIRECT_REJ,
  TDSRRC_INTERRAT_REDIRECT_ABORT_CNF,

  /****** RR -> TD Stop Mode ************/
  TDSRRC_STOP_TDS_CNF,

  /****** TD -> RR Dedicated Priorities ************/
  TDSRR_DEDICATED_PRIORITIES_REQ,
  TDSRR_CLEAR_DEDICATED_PRIORITIES_IND,

  /****** RR -> TD Dedicated Priorities ************/
  TDSRRC_DEDICATED_PRIORITIES_RSP
} tdsrr_rrc_cmd_e_type;

/***************************************************************************/
/****************  RR -> TD HANDOVER  **************************************/
/***************************************************************************/
typedef enum
{
  TDSRRC_INVALID_STATE,
  TDSRRC_TDS_PROTOCOL_ERROR,
  TDSRRC_INVALID_CONFIGURATION,
  TDSRRC_PHYSICAL_CHANNEL_FAILURE,
  TDSRRC_INVALID_CIPHER_ALGORITHM,
  TDSRRC_FREQUENCY_NOT_IMPLEMENTED,
  TDSRRC_MAX_FAILURE_CAUSE
} tdsrrc_failure_cause_e_type;

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_HANDOVER_CNF
----------------------------------------------------------------------------*/
typedef struct
{
  /* Message header */
  IMH_T message_header;

  sys_modem_as_id_e_type	  as_id;

  /* Handover status */
  tdsrrc_rr_status_e_type handover_status;

  /* Handover failure cause: this field is only valid if handover status is
     failure */
  tdsrrc_failure_cause_e_type failure_cause;

} tdsrrc_interrat_handover_cnf_type;

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_HANDOVER_ABORT_CNF
----------------------------------------------------------------------------*/
typedef struct
{
  /* Message header */
  IMH_T message_header;
  sys_modem_as_id_e_type      as_id;
} tdsrrc_interrat_handover_abort_cnf_type;


/***************************************************************************/
/****************  TD -> RR HANDOVER  **************************************/
/***************************************************************************/
/* Maximum number of inter-RAT measurements */
#define TDSRR_MAX_INTERRAT_MEASUREMENTS 32

typedef enum
{
  TDSRR_DCS_1800,
  TDSRR_PCS_1900,
  TDSRR_MAX_BANDS
} tdsrr_frequence_band_e_type;

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

  /* frequency offset */
  int16  frequency_offset;

} tdsrr_interrat_measurement_type;

typedef struct
{
  /* number of cells */
  uint8 number_of_cells;

  /* Inter-RAT measurement data */
  tdsrr_interrat_measurement_type interrat_measurements[TDSRR_MAX_INTERRAT_MEASUREMENTS];
} tdsrr_interrat_measurement_data_type;

/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_HANDOVER_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  uint8 rscp_threshold;
  uint8 num_frequencies;
  uint16 frequency_list[MAX_UTRAN_TDS_FREQUNECIES];
}tdsrr_ho_pseudo_fast_return_type;

typedef struct
{
  /* Message header */
  IMH_T message_header;

  sys_modem_as_id_e_type       as_id;

  /* Frequence band indicator */
  tdsrr_frequence_band_e_type frequency_band;

  /* Cipher Enable Indicator */
  boolean cipher_enabled;

  /* Inter-RAT measurement data */
  tdsrr_interrat_measurement_data_type interrat_measurement_data;

  /* GSM Commands: it should only include one command: GSM handover command */
  tdsrrc_rr_gsm_command_list_type gsm_command_list;

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  sys_rat_pri_list_info_s_type   rat_pri_list_info;

  boolean pseudo_fr_active;

  /* pseudo fr info is only valid when pseudo_fr_active is TRUE */
  tdsrr_ho_pseudo_fast_return_type pseudo_fr_info;

  lte_irat_dedicated_priority_info_s dedicated_priority_info;
} tdsrr_interrat_handover_req_type;

/*----------------------------------------------------------------------------
                         TDSRR_NAS_DATA_IND
This primitive is sent from RRC to RR to forward bufferred NAS signaling 
message. If there is not message left to send, RRC will send the primitive to 
RR with the message length field set to 0. 
----------------------------------------------------------------------------*/
typedef struct
{
  /* Message Header */
  IMH_T message_header;

  sys_modem_as_id_e_type       as_id;

  /* NAS signalling data */
  tdsrrc_rr_gsm_command_type nas_data;
} tdsrr_nas_data_ind_type;

/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_HANDOVER_ABORT_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  /* Message header */
  IMH_T message_header;
  sys_modem_as_id_e_type     as_id;
} tdsrr_interrat_handover_abort_req_type;



/***************************************************************************/
/****************  TD -> RR CC ORDER  **************************************/
/***************************************************************************/

/* Max. number of SI blocks in GERAN System Information for Network Assisted Cell Change(NACC) */
#define TDSRR_MAX_SI_BLOCKS 8

/* Max. SI block length in GERAN System Information for Network Assisted Cell Change(NACC) */
#define TDSRR_MAX_SI_BLOCK_LENGTH 23

typedef enum
{
  TDSRR_NC_MODE_0,
  TDSRR_NC_MODE_1,
  TDSRR_NC_MODE_2
} tdsrr_nc_mode_e_type;

typedef enum 
{
  TDSRR_GERAN_INFO_TYPE_NONE,
  TDSRR_GERAN_INFO_TYPE_SI,
  TDSRR_GERAN_INFO_TYPE_PSI
} tdsrr_geran_sysinfo_choice_e_type;

typedef struct
{
  // Length could be anywhere between 1 and 23 bytes
  uint8 msg_length;                       

  // The actual SI block message
  byte  msg_string[TDSRR_MAX_SI_BLOCK_LENGTH];    

} tdsrr_geran_SI_block_type;


/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_CC_ORDER_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
   IMH_T                          message_header;

   sys_modem_as_id_e_type     as_id;

   /* indicates NC Mode is present */
   boolean                        NC_Mode_present;

   /*  NC Mode */

   tdsrr_nc_mode_e_type           NC_Mode;

   /* indicates the Target GSM Cell BCCH ARFCN */
   ARFCN_T                          BCCH_ARFCN;

   /* indicates the Target GSM Cell BSIC */
   uint8                          BSIC_NCC;
   uint8                          BSIC_BCC;

   /* RXLev Average of the GSM Target Cell */
   byte                           RXLEV_average;

   /* Timer T309 Value */
   uint32                         T309_value;

   /* Indicates how a PLMN is to be selected. */
   sys_network_selection_mode_e_type network_select_mode;

   /* Type of Geran SysInfo */
   tdsrr_geran_sysinfo_choice_e_type geran_sysinfo_choice;

   /* Number of SI blocks in Geran SysInfo*/
   uint8  num_blocks; 

   /* Geran SysInfo blocks */
   tdsrr_geran_SI_block_type geran_SI[TDSRR_MAX_SI_BLOCKS];

   sys_rat_pri_list_info_s_type   rat_pri_list_info;

   lte_irat_dedicated_priority_info_s dedicated_priority_info;
} tdsrr_interrat_cc_order_req_type;


/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_CC_ORDER_ABORT_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} tdsrr_interrat_cc_order_abort_req_type;

/***************************************************************************/
/****************  TD -> RR RESELECTION ************************************/
/***************************************************************************/

/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_RESELECT_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
   IMH_T                          message_header;

   sys_modem_as_id_e_type     as_id;

   /* Indicates how a PLMN is to be selected. */
   sys_network_selection_mode_e_type network_select_mode;

   /* indicates the Target GSM Cell BCCH ARFCN */
   ARFCN_T                      BCCH_ARFCN;

   /* indicates the Target GSM Cell BSIC */
   uint8                          BSIC_NCC;
   uint8                          BSIC_BCC;

   /* RXLev Average of the GSM Target Cell */
   byte                           RXLEV_average;

   /* PLMNId of the requested PLMN */
   PLMN_id_T                      requested_PLMN;

   sys_rat_pri_list_info_s_type   rat_pri_list_info;
   
   lte_irat_dedicated_priority_info_s dedicated_priority_info;
} tdsrr_interrat_reselect_req_type;


/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_RESELECTION_ABORT_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T message_header;
  sys_modem_as_id_e_type     as_id;
} tdsrr_interrat_reselection_abort_req_type;

/***************************************************************************/
/****************  RR -> TD RESELECTION ************************************/
/***************************************************************************/

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_RESELECT_REJ 
----------------------------------------------------------------------------*/
typedef struct
{

   IMH_T                           message_header;

   sys_modem_as_id_e_type     as_id;

   /* Cause of InterRAT Reselection Failure */
   tdsrrc_interrat_reject_reason_T reject_reason;

   /* Time ( in Sec ) when the same cell should be considered */
   uint32                          time_to_reselect;

   /* Thresholds to be applied to Ec/No measurements */
   int16                           squalmin;

   int16                           srxlevmin;

   /* PLMN Id of the UTRAN TDD cell */
   PLMN_id_T                       PLMN;

   /* LAC */
   uint8                           forbidden_lac[2];

} tdsrrc_interrat_reselect_rej_type;


/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_RESELECTION_ABORT_CNF 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} tdsrrc_interrat_reselection_abort_cnf_type;


/***************************************************************************/
/****************  RR -> TD PLMN SEARCH  ***********************************/
/***************************************************************************/

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_RESELECTION_ABORT_CNF 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
  
  /* List of higher priority PLMNs that were found on 3G */
  sys_detailed_plmn_list_s_type available_plmn_list;
  /* Indicates if the BPLMN search is done or not */
  boolean srch_done;
  /* Indicates if the highest priority PLMN was found or not */
  boolean hplmn_found;
} tdsrrc_interrat_plmn_srch_cnf_type;

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T message_header;
  sys_modem_as_id_e_type     as_id;
} tdsrrc_interrat_plmn_srch_abort_cnf_type;

/***************************************************************************/
/****************  TD -> RR PLMN SEARCH  ***********************************/
/***************************************************************************/

/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_PLMN_SRCH_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;

  uint32     num_plmns;
  /* PLMN Ids to be searched on 2G with 0th element being
   * the highest priority PLMN */
  PLMN_id_T  plmn[SYS_PRIORITY_PLMN_LIST_MAX_LENGTH];

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

   /* Time allowed by GL1 for UTRAN TDD to be active */
  uint32 timer_val;

  /* Sent to RR so that RR can determine if the plmn search request
   * is for "full service search" or "plmn list search"
   *
   * RRC originally receives the enum value from NAS
   * in the PLMN_LIST_REQ
   */
  sys_service_search_e_type service_search;

  boolean is_foreground_search;

#ifdef FEATURE_TDSCDMA_SCAN_SCOPE
  /* Sent to RR so that RR can determine if the plmn search request
   * is for "full band search" or "ACQ_DB Search"
   *
   * RRC originally receives the enum value from NAS
   * in the PLMN_LIST_REQ
   */
  sys_scan_scope_e_type scan_scope;
#endif
} tdsrr_interrat_plmn_srch_req_type;


/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_PLMN_SRCH_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T message_header;
  sys_modem_as_id_e_type     as_id;
} tdsrr_interrat_plmn_srch_abort_req_type;

/*----------------------------------------------------------------------------
                     TDSRR_INTERRAT_PLMN_SRCH_SUSPEND_IND 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T message_header;
  sys_modem_as_id_e_type     as_id;
} tdsrr_interrat_plmn_srch_suspend_ind_type;

/***************************************************************************/
/****************  TD -> RR REDIRECTION  ***********************************/
/***************************************************************************/

#define TDS_MAX_RRC_RR_GSM_CELL_LIST_SIZE 32

typedef struct
{
  uint8                          BSIC_NCC;
  uint8                          BSIC_BCC;
} TDSRR_BSIC_type;

typedef struct
{
  uint32 size;
  ARFCN_T                     arfcns[TDS_MAX_RRC_RR_GSM_CELL_LIST_SIZE];

  TDSRR_BSIC_type             BSIC[TDS_MAX_RRC_RR_GSM_CELL_LIST_SIZE];
} tdsrr_gsm_cell_list_type;

typedef enum
{
  TDSRR_REDIRECTION_SEARCH,
  TDSRR_OUT_OF_SERVICE_SEARCH,
  TDSRR_SERVICE_SEARCH,
  TDSRR_REDIRECTION_AFTER_CONN_RELEASE
} tdsrr_cell_srch_reason_e_type;

/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_REDIRECT_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T                             message_header;

  sys_modem_as_id_e_type     as_id;

  /* PLMN of the UTRAN TDD cell */
  PLMN_id_T                         PLMN;  

  /* Indicates how a PLMN is to be selected */
  sys_network_selection_mode_e_type network_select_mode;

  /* Time for which GSM can look for service */
  uint32                            wait_timer;

  /* Establishment cause - Emergency call or not */
  boolean                           is_emergency_call;

  /* GSM neighbor list that will be searched first before cell selection
   * is triggered */
  tdsrr_gsm_cell_list_type         gsm_ncell_list;

  tdsrr_cell_srch_reason_e_type    cell_srch_reason;

  sys_rat_pri_list_info_s_type   rat_pri_list_info;

  lte_irat_dedicated_priority_info_s dedicated_priority_info;
} tdsrr_interrat_redirect_req_type;

/***************************************************************************/
/****************  RR -> TD REDIRECTION  ***********************************/
/***************************************************************************/
/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_REDIRECT_REJ 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type     as_id;
} tdsrrc_interrat_redirect_rej_type;

/*----------------------------------------------------------------------------
                         TDSRRC_INTERRAT_REDIRECT_ABORT_CNF 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type     as_id;
} tdsrrc_interrat_redirect_abort_cnf_type;

/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_REDIRECT_ABORT_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T message_header;
  sys_modem_as_id_e_type     as_id;
} tdsrr_interrat_redirect_abort_req_type;


/***************************************************************************/
/****************  RR -> TD STOP MODE  *************************************/
/***************************************************************************/

/*----------------------------------------------------------------------------
                         TDSRRC_STOP_TDS_CNF 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T    message_header;
  sys_modem_as_id_e_type     as_id;
  /* Indicates the reason for stopping UTRAN TDD */
  tdsrrc_rr_stop_tds_cause_e_type    stop_cnf_cause;
} tdsrrc_stop_tds_cnf_type;


/***************************************************************************/
/****************  TD -> RR STOP MODE  *************************************/
/***************************************************************************/

/*----------------------------------------------------------------------------
                         TDSRR_STOP_GSM_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  IMH_T    message_header;
  sys_modem_as_id_e_type     as_id;
  /* Indicates the reason for stopping UTRAN TDD */
  tdsrrc_rr_stop_tds_cause_e_type stop_req_cause;
} tdsrr_stop_gsm_req_type;


/***************************************************************************/
/****************  TD -> RR INTERRAT TL1 STOP IND **************************/
/***************************************************************************/

/*----------------------------------------------------------------------------
                         TDSRR_INTERRAT_TL1_STOP_IND 
----------------------------------------------------------------------------*/
typedef enum
{
  TDSRR_TL1_STOP_IND_NO_CAUSE,
  TDSRR_TL1_STOP_IND_T2G_CCO  
} tdsrr_tl1_stopped_cause_type;

typedef struct
{
  IMH_T    message_header;
  sys_modem_as_id_e_type     as_id;
  /* Indicates the reason for stopping TL1 */
  tdsrr_tl1_stopped_cause_type cause;
} tdsrr_interrat_tl1_stop_ind_type;

/* -----------------------------------------------------------------------
** Function Prototypes
** ----------------------------------------------------------------------- */
/**
  @brief Interface function used by RRC to send messages to RR.

  This function may be used by RRC to put a message on RR's message queue.

  @param message_ptr: Pointer to the RRC message to be sent to RR.
  @param rrc_cmd_id:  Message Id
  @param length:      Message Length.
*/
void tdsrr_send_rrc_message_to_rr(
  void *                message_ptr,
  tdsrr_rrc_cmd_e_type  rrc_cmd_id,
  word                  length
);

/**
  @brief Get the band for a GSM ARFCN given the band indicator value provided.

  This function is used to determine the band that the specified ARFCN is a
  member of, using the provided band indicator to resolve any conflict if the
  ARFCN is in the DCS 1800/PCS 1900 overlap region.

  @param arfcn:    The ARFCN that the caller requires the band of.
  @param band_ind: Band indicator value, as received OTA in UMTS.  This
                   parameter is unused if the ARFCN is not in DCS 1800/PCS
                   1900 band.
  @return:         The band that ARFCN is in, or INVALID_BAND if it is in a
                   band that is not currently supported by RR.
*/
sys_band_T tdsrr_get_arfcn_band(
  uint16                       arfcn,
  tdsrr_frequence_band_e_type band_ind
);

/**
  @brief Get the band for a GSM ARFCN given the band indicator value provided.

    This function serves for the same purpose as tdsrr_get_arfcn_band, with TDS
    as_id provided for Dual_SIM.
*/
sys_band_T tdsrr_get_arfcn_band_sub_id(
  uint16						 arfcn,
  tdsrr_frequence_band_e_type band_ind,
  sys_modem_as_id_e_type     as_id
);

/**
  @brief Interface used by RRC to update RR's band_pref.

  This function should be called whenever RRC receives a new band_pref from MM.

  @param band_pref: The band_pref for RR to set.
*/
void tdsrr_set_band_pref(
  sys_band_mask_type band_pref
);

/**
  @brief Interface used by RRC to update RR's band_pref.

  This function serves for the same purpose as tdsrr_set_band_pref, with TDS
  as_id provided for Dual_SIM.
*/
void tdsrr_set_band_pref_sub_id(
  sys_band_mask_type       band_pref,
  sys_modem_as_id_e_type   as_id
);

/**
  @brief Interface used by RRC to update RR's bst band_pref.

  This function should be called whenever RRC receives a new bst band_pref from MM.

  @param band_pref: The bst band_pref for RR to set.
*/
void tdsrr_set_bst_band_pref(
  sys_band_mask_type band_pref
);

/**
  @brief Interface used by RRC to update RR's bst band_pref.

  This function serves for the same purpose as tdsrr_set_bst_band_pref, with TDS
  as_id provided for Dual_SIM.
*/
void tdsrr_set_bst_band_pref_sub_id(
  sys_band_mask_type band_pref,
  sys_modem_as_id_e_type   as_id 
);

/**
  @brief Interface used by RRC to get GSM camped PLMN id.

  This function may be used to access the PLMN of the cell that RR is currently
  camped on.

  @return: PLMN ID of the cell that RR is camped on, or (0xFF 0xFF 0xFF) is
           returned if RR is not camped.
*/
extern PLMN_id_T  tdsrr_get_rr_curr_camped_plmn(void);

/**
  @brief Interface used by RRC to get GSM camped PLMN id.

  This function serves for the same purpose as tdsrr_get_rr_curr_camped_plmn, with TDS
  as_id provided for Dual_SIM.
*/
extern PLMN_id_T  tdsrr_get_rr_curr_camped_plmn_sub_id(
  sys_modem_as_id_e_type   as_id
);

/**
  @brief Save RR's acquisition database to NV.

  This function must be called by RRC at power-off to write RR's acquisition
  database to NV.
*/
extern void tdsrr_acq_db_save(void);

/**
  @brief Save RR's acquisition database to NV.

  This function serves for the same purpose as tdsrr_acq_db_save, with TDS
  as_id provided for Dual_SIM.
*/
extern void tdsrr_acq_db_save_sub_id(
  sys_modem_as_id_e_type   as_id
);

/**
  @brief TDSRRC Calls this API to get Partial PLMN results from RR during ongoing TDtoG
            Manual PLMN Search
*/
boolean rr_get_inter_rat_bplmn_prtl_result(
  sys_detailed_plmn_list_s_type *available_plmn_list
);

/**
  @brief TDSRRC Calls this API to get Partial PLMN results from RR during ongoing TDtoG
            Manual PLMN Search
  This function serves for the same purpose as rr_get_inter_rat_bplmn_prtl_result, with TDS
  as_id provided for Dual_SIM.
*/
boolean rr_get_inter_rat_bplmn_prtl_result_sub_id(
  sys_detailed_plmn_list_s_type *available_plmn_list,
  sys_modem_as_id_e_type as_id
);
#endif /* RR_TDS_RRC_IF_H */

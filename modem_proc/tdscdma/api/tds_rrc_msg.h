#ifndef TDS_RRC_MSG_H
#define TDS_RRC_MSG_H


/*===========================================================================
                      RRC  UMIDs and Interface Structures

DESCRIPTION

  This module contains the UMIDs and Interfaces Structures for InteRAT->TDSCDMA
  procedures



Copyright (c) 2000-2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tds_rrc_msg.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/10   rm      Added lte_irat_capabilities_s to tds_rrc_lte_utra_capabilities_req_type
04/20/10   as      Removed lte_irat_capabilities_s from tds_rrc_lte_utra_capabilities_req_type
04/19/10   rm      Added lte_irat_capabilities_s to tds_rrc_lte_utra_capabilities_req_type
03/13/10   rm      Modifed TDSCDMA_MAX_CAP_CONTAINER_LENGTH to 256 bytes
03/02/10   rm      Defined UMIDs,structures for TDSCDMA capabilities
02/11/09   rm      Added UMIDs for LTOW BPLMN search. Also corrected the dedicated priority
                   info structures.
02/08/10   np      Include new common header lte_irat_types.h and common sys file
01/31/10   rm      Modified PLMN_id_T to sys_plmn_id_s_type
01/28/10   rm      Added UMIDs, interface structures for LTE to TDSCDMA reselection
                   and redirection
===========================================================================*/

#include "tds_rrc_umid.h"

#include <sys_plmn_selection.h>

/* LTE header file for common definitions (dedicated priorities, bplmn structs etc. ) */
#include <lte_irat_types.h>

#include "tds_rrc_irat_types.h"

/*========================================================
            LTOTD InterRAT reselection structures
=========================================================*/

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

   /* The Frequency of the Target UTRAN Cell */
   uint16                              uarfcn;

   /* The Scrambling code of the target UTRAN Cell */
   uint8                               cpid;

   /* Indicates how a PLMN is to be selected. */
   sys_network_selection_mode_e_type network_select_mode;

   /* PLMNId of the requested PLMN */
   sys_plmn_id_s_type                  requested_plmn;

  /*RAT capability, should pass the same structure that NAS uses*/
  sys_rat_pri_list_info_s_type    rat_pri_list_info;

  /*dedicated priority info*/
  lte_irat_dedicated_priority_info_s dedicated_priority_info;
} tds_rrc_lte_resel_req_type;

/*LTE to TDSCDMA IRAT failure cause*/
typedef enum
{
  TDSCDMA_RRC_NONE,
  TDSCDMA_RRC_ACQUISITION_FAIL,         /*Acquisition Failure*/
  TDSCDMA_RRC_SIBS_FAILURE,             /*SIBs failure, either S-BCCH setup failure / SIBs timeout etc*/
  TDSCDMA_RRC_CELL_BARRED,              /*Cell Barred*/
  TDSCDMA_RRC_UARFCN_BARRED,            /*Frequency Barred*/
  TDSCDMA_RRC_FORBIDDEN_LA,             /*Forbidden LA*/
  TDSCDMA_RRC_PLMN_MISMATCH,            /*PLMN mismatch*/
  TDSCDMA_RRC_CELL_QUALITY_FAILURE,     /*S-Criteria Failure*/
  TDSCDMA_RRC_CELL_NOT_SUITABLE,        /*Access Not Allowed*/
  TDSCDMA_RRC_CELL_CHANGE_FAILURE,      /*Transition to the cell failure or Paging channel setup failure*/
  TDSCDMA_RRC_MAX_FAILURE
} tds_rrc_irat_failure_cause_e_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  /*Failure cause*/
  tds_rrc_irat_failure_cause_e_type failure_cause;

  /* Time ( in Sec ) when the same cell should be considered */
  uint32                         time_to_reselect;
}tds_rrc_lte_resel_failed_rsp_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} tds_rrc_lte_abort_resel_req_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} tds_rrc_lte_abort_resel_rsp_type;



/*========================================================
              LTOTD InterRAT redirection structures
=========================================================*/

#define TDSCDMA_RRC_LTE_MAX_UTRAN_TDD_CELLS_PER_FREQ    32
#define TDSCDMA_RRC_LTE_MAX_UTRAN_TDD_FREQ    (TDSCDMA_RRC_LTE_MAX_SIB6_UTRAN_TDD_NEIGH_FREQ + 1)
#define TDSCDMA_RRC_LTE_MAX_SIB6_UTRAN_TDD_NEIGH_FREQ   16

#define TDSCDMA_RRC_MAX_LTE_DEPRI_FREQ LTE_IRAT_MAX_EUTRAN_FREQ

#define TDSCDMA_RRC_MAX_UTRA_CONT 16

#define TDSCDMA_RRC_MAX_SHARED_PLMN 5

/* L2TD Redirection List Rel Type */
typedef enum
{
  L2TD_REDIR_REL_9,
  L2TD_REDIR_REL_10
} tds_rrc_lte_redir_list_rel_e_type;

typedef struct
{
  uint8     cpid;
  uint8     dsm_idx;
} tds_rrc_lte_tds_cell_id_dsm_info_type;

typedef struct
{
  uint16    uarfcn;
  uint8     num_of_cells;
#if defined(TEST_FRAMEWORK) && defined(TF_UNIT_TEST)
  #error code not present
#endif
  tds_rrc_lte_tds_cell_id_dsm_info_type     cell_id_dsm_info_list[TDSCDMA_RRC_MAX_UTRA_CONT];
} tds_rrc_lte_tds_cell_list_type;

typedef struct
{
  uint8    num_of_frequencies;
  tds_rrc_lte_tds_cell_list_type    tdd_cell_list[TDSCDMA_RRC_LTE_MAX_UTRAN_TDD_FREQ];
} tds_rrc_lte_tds_target_info_type;

typedef struct
{
  uint8     uarfcn_count;                       /*!< Num frequencies present */ 
  uint16    uarfcn[TDSCDMA_RRC_LTE_MAX_SIB6_UTRAN_TDD_NEIGH_FREQ]; /*!< Frequency - 0..16383 */
} tds_rrc_lte_tds_neigh_list_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  /*Message router Attachments*/
  msgr_attach_s msgr_attach[TDSCDMA_RRC_MAX_UTRA_CONT];

  tds_rrc_lte_redir_list_rel_e_type  tdd_redir_list_rel;

  /*Number of CPIDs on redirected frequency for which the N/W has sent SI container*/
  uint8 num_cpid;

  /*CPID list on redirected frequency*/
  /* Use uint16 to satisfy LTE SW needs */
  uint16 cpid_list[TDSCDMA_RRC_MAX_UTRA_CONT];

  /* PLMN of the TDSCDMA cell */
  sys_plmn_id_s_type    plmn;

  /* Indicates how a PLMN is to be selected */
  sys_network_selection_mode_e_type    network_select_mode;

  /* list of TDSCDMA cells or frequencies */
  tds_rrc_lte_tds_target_info_type    tdd_target_info;

  /* list of TDSCDMA frequencies from LTE SIB6 */
  tds_rrc_lte_tds_neigh_list_type     sib6_neighbors;

  /*RAT capability, should pass the same structure that NAS uses*/
  sys_rat_pri_list_info_s_type    rat_pri_list_info;

  /*dedicated priority info*/
  lte_irat_dedicated_priority_info_s priority_info;

  /*Fast redirection */
  boolean fast_redirection;

} tds_rrc_lte_redir_req_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} tds_rrc_lte_redir_failed_rsp_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
}tds_rrc_lte_abort_redir_req_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} tds_rrc_lte_abort_redir_rsp_type;


/*========================================================
              TDSCDMA Capability Stuctures
=========================================================*/
#define TDSCDMA_MAX_CAP_CONTAINER_LENGTH 256

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr;

  sys_band_mask_type             tdscdma_band_preference;
#ifdef LTE_BAND_NUM
  sys_lte_band_mask_e_type       lte_band_preference;//xlong 0323
#else
  sys_band_mask_type             lte_band_preference;
#endif
  sys_band_mask_type             gsm_band_preference;

  lte_irat_capabilities_s        rat_capabilities;
  boolean                        compressed_mode_required;
  boolean                        l2t_psho_enabled;

}tds_rrc_lte_utra_capabilities_req_type;


typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr;

  uint32     capabilities_container_length;
  uint8      capabilities_container[TDSCDMA_MAX_CAP_CONTAINER_LENGTH];

}tds_rrc_lte_utra_capabilities_rsp_type;

/*========================================================
              TDSCDMA to LTE dedicated priority Stuctures
=========================================================*/

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} tds_rrc_lte_get_dedicated_pri_req_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  /*Boolean indicating if dedicated priorities are valid or not*/
  boolean dedicated_pri_valid;

  /*PLMN ID in which the dedicated priorities are received*/
  sys_plmn_id_s_type    plmn;

  /*dedicated priority info*/
  lte_irat_dedicated_priority_info_s dedicated_pri_info;
} tds_rrc_lte_get_dedicated_pri_rsp_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} tds_rrc_lte_clear_dedicated_pri_ind_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} tds_rrc_lte_plmn_srch_suspend_ind_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr;

  /* TDS UARFCN on which IRAT is successful */
  uint16 uarfcn;
} tds_rrc_lte_irat_info_ind_type;

/*========================================================
              LTE to TDSCDMA PS Handover Stuctures
=========================================================*/

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  msgr_attach_s dsm_attach; /*!< dsm attachment
                                 containing the handOverToUTRANCommand */

  lte_irat_umts_sec_info_s security_keys;

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  /* PLMNId of the PLMN UE is camped on the LTE side */
  sys_plmn_id_s_type  plmn_id;

  sys_rat_pri_list_info_s_type   rat_pri_list_info;

  /* Inter-RAT measurement data */
  /* ZZTBD: TDD need new data type for this */
  /* HX turned it on for compiling purpose*/
  tdsrrc_interrat_measurement_data_type interrat_measurement_data;

} tds_rrc_lte_psho_req_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_LTE_PSHO_RSP
This primitive is sent from WRRC to LRRC to respond the TDSCDMA_RRC_LTE_PSHO_REQ 
----------------------------------------------------------------------------*/
/* Handover status */
typedef enum
{
  L2TD_PSHO_SUCCESS,
  L2TD_PSHO_FAILURE,
  L2TD_PSHO_MAX_STATUS
} tds_rrc_lte_psho_status_e_type;

/* RRC Handover failure causes */
typedef enum
{
  L2TD_PSHO_INVALID_STATE,
  L2TD_PSHO_TDSCDMA_PROTOCOL_ERROR,
  L2TD_PSHO_INVALID_CONFIGURATION,
  L2TD_PSHO_PHYSICAL_CHANNEL_FAILURE,
  L2TD_PSHO_INVALID_CIPHER_ALGORITHM,
  L2TD_PSHO_FREQUENCY_NOT_IMPLEMENTED,
  L2TD_PSHO_MAX_FAILURE_CAUSE
} tds_rrc_lte_psho_failure_cause_e_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  /* Handover status */
  tds_rrc_lte_psho_status_e_type handover_status;

  /* Handover failure cause: this field is only valid if handover status is
     failure */
  tds_rrc_lte_psho_failure_cause_e_type failure_cause;

} tds_rrc_lte_psho_rsp_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_LTE_ABORT_PSHO_REQ 
This primitive is sent from LRRC to WRRC to abort TDSCDMA to LTE inter-RAT 
handover. 
----------------------------------------------------------------------------*/

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} tds_rrc_lte_abort_psho_req_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_LTE_ABORT_PSHO_RSP
This primitive is sent from WRRC to LRRC in response to
TDSCDMA_RRC_LTE_ABORT_PSHO_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} tds_rrc_lte_abort_psho_rsp_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_LTE_GET_CGI_REQ
This primitive is sent from LRRC to WRRC for acquiring CGI information for 
a particular cell 
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s msg_hdr;

  /* UARFCN of the cell for which CGI info is requested */
  uint16    uarfcn;

  /* CPID of the cell for which CGI info is requested */
  uint8     cpid;     

  /* Time in ms that TDS has, for acquiring CGI info; TDS has to send a response 
    back to LTE with a success/failure within this time */
  uint32    time_available_for_cgi;  

} tds_rrc_lte_get_cgi_req_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_LTE_GET_CGI_RSP
This primitive is sent from WRRC to LRRC in response to
TDSCDMA_RRC_LTE_GET_CGI_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  /* Cell id for the TDS cell for which CGI info was requested */
  uint32 cell_id;

  /* PLMN id for the TDS cell for which CGI info was requested */
  sys_plmn_id_s_type  plmn_id;

  /* If location area code is included */
  boolean lac_included;

  /* Location area code for the cell for which CGI info was requested */
  uint8 lac[2];

  /* If Rounting area code is included */
  boolean rac_included;

  /* Rounting area code for the cell for which CGI info was requested */
  uint8 rac;

  /* Num of shared PLMNs included in the CGI response */
  uint8 num_of_shared_plmns;

  /* Shared PLMN ids for the cell for which CGI info was requested */
  sys_plmn_id_s_type  shared_plmn_list[TDSCDMA_RRC_MAX_SHARED_PLMN];  

} tds_rrc_lte_cgi_info_type;

typedef struct
{
  /* message header type */
  msgr_hdr_s msg_hdr;

  /* UARFCN of the cell for which CGI info was requested */
  uint16   uarfcn;

  /* CPID of the cell for which CGI info was requested */
  uint8    cpid;

  /* TRUE, if CGI acquisition suceeded and CGI info is included, FALSE otherwise */
  boolean  cgi_info_included;     

  /* CGI information for the requested cell */
  tds_rrc_lte_cgi_info_type cgi_info;

} tds_rrc_lte_get_cgi_rsp_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_LTE_ABORT_CGI_REQ
This primitive is sent from LRRC to TRRC for aborting CGI acquisition on TDS
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s msg_hdr;  

} tds_rrc_lte_abort_cgi_req_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_LTE_ABORT_CGI_RSP
This primitive is sent from TRRC to LRRC in response to 
TDSCDMA_RRC_LTE_ABORT_CGI_REQ
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s msg_hdr; 

} tds_rrc_lte_abort_cgi_rsp_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_LTE_GET_PLMN_PRTL_RESULTS_REQ
This primitive is sent from LRRC to TRRC for getting plmn partial results
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type*/
  msgr_hdr_s msg_hdr;

} tds_rrc_lte_get_plmn_prtl_results_req_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_GPS_POS_CELL_INFO_REQ
This primitive is sent from GPS application to TRRC to request for position
information of cells.
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s  msg_hdr; 

  /* User data. To be returned to calling client*/
  void * p_user_data;
} tds_rrc_gps_pos_cell_info_req_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_GPS_POS_CELL_INFO_RSP
This primitive is sent from TRRC to GPS application to response position
information of cells.
----------------------------------------------------------------------------*/
#define TDSCDMA_RRC_MAX_NBR_CELL_NUM 15

typedef struct
{
  //rrc_plmn_identity_type plmn_id;

  /* Mobile Country Code - one digit per byte range 0..9)  */
  byte mcc[3];

  /* Number of MNC digits */
  byte num_mnc_digits;

  /* Mobile Network Code - one digit per byte (range 0..9) */
  byte mnc[3];

  /* 16-bit data in 2 bytes. First byte (index 0) is the MSB */
  uint8       lac[2];

  /* 0 to 16383 */
  uint16      uarfcn;

  /* 0 to 268435455 */
  uint32      cell_id;

  /* 0 to 127 */
  uint8       cell_parameter_id;

  /* 46 to 158 dB */
  uint8       pathloss;

  /* 0 to 8191 cx8, same value for all timeslot */
  uint16      timing_advance;

  /* (-25 to -128)*256 RSCP dBm in Q8 L3 filtered. */
  int32       rscp;
} tds_rrc_scell_gps_pos_info;

typedef struct
{
  /* 0 to 16383 */
  uint16      uarfcn;

  /* 0 to 127 */
  uint8       cell_parameter_id;

  /* (-25 to -128)*256 RSCP dBm in Q8 L3 filtered. */
  int32       rscp;
} tds_rrc_ncell_gps_pos_info;

typedef struct
{
  /* message header type */
  msgr_hdr_s  msg_hdr; 

  /* Indicate whether RRC success to get the serving cell info 
   * TRUE: serving cell info is available 
   * FALSE: all data are invalid
   */

  /* User data. To be returned to calling client*/
  void * p_usr_data;

  boolean     success_flag;

  tds_rrc_scell_gps_pos_info serv_cell;
  
  /* Indicate whether RRC success to get the neighbor cells info 
   * TRUE: neighbor cell info available
   * FALSE: neighbor cell info not available
   */
  boolean     nbr_success_flag;

  /* 0 to TDSCDMA_RRC_MAX_NBR_CELL_NUM */
  uint8       num_ncell;

  tds_rrc_ncell_gps_pos_info  nbr_ncell[TDSCDMA_RRC_MAX_NBR_CELL_NUM];
  
} tds_rrc_gps_pos_cell_info_rsp_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_SCELL_SIGNAL_STATUS_IND
This primitive is sent from TRRC to upper layer to indicate signal status of 
serving cell. 
This structure contains: 
     - RSSI (0.1 dBm resolution)
     - RSCP (0.1 dBm resolution)
     - EcIo (0.1 dB resolution)
     - SINR (CELL_DCH only, 0.1 dB resolution)
     - BLER (CELL_DCH only)
     - AS_ID (TBD - will be added after DSDS/DSDA are supported)  
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s  msg_hdr;

#ifdef FEATURE_DUAL_SIM
  /* SUB ID */
  sys_modem_as_id_e_type as_id;
#endif

  /* (-120 to -25)*10 dBm */
  int16       rssi;

  /* (-120 to -25)*10 dBm */
  int16       rscp;

  /* (-128 to 0)*10 dB */
  int16       ecio;

  /* The flag determines if SINR and BLER are valid.
     If it is FALSE, do not use the values in SINR and BLER field
  */
  boolean      in_dch;

  /* (-15 to 127)*10 dB
     -15 dB is sent to upper layer if actual SINR is less than -15 dB.
     It is only valid when UE is in CELL_DCH state.
  */
  int16        sinr;
 
  /* This is a percentage rate range from 0 - 63 (%)
     It is only valid when UE is in CELL_DCH state.
  */
  uint8       bler;

} tds_rrc_scell_signal_status_ind_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_LTE_DEPRI_FREQ_REQ
This primitive is sent from LRRC to TRRC for passing Deprioritization
list from LTE
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s  msg_hdr;
 
  /*! Indicates if all EUTRA neighbor freqs should be deprioritized */
  boolean  depri_all_freq;
 
  /*! Number of freqs being deprioritized; Will be set to a valid value
      only if the depri_all_freq boolean is set to FALSE
      If num_depri_freq = 0 and depri_all_freq = FALSE, this implies
      LTE deprioritization is cleared*/
  uint8   num_depri_freq;
  
  /*! Set of deprioritized EUTRA frequencies. This will be valid only
    if the depri_all_freq boolean is set to FALSE */
  lte_earfcn_t depri_freqs[TDSCDMA_RRC_MAX_LTE_DEPRI_FREQ];
  
} tds_rrc_lte_depri_freq_req_type;

#endif /*TDSCDMA_RRC_MSG_H*/


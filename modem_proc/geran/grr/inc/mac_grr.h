/*****************************************************************************
***
*** TITLE
***
***  mac_grr.h
***
*** DESCRIPTION
***
***  Signal definitions for use over the MAC -> GRR interface.
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/

/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/inc/mac_grr.h#1 $
***
*** $History: mac_grr.h $
 *
 * *****************  Version 3  *****************
 * User: Damon        Date: 1/11/02    Time: 17:56
 * Updated in $/GPRS_RR_Host_2_0/gprs/grr
 * SUCCESS->MAC_GRR_SUCCESS
 * FAILURE->MAC_GRR_FAILURE
 *
 * *****************  Version 2  *****************
 * User: Damon        Date: 15/10/02   Time: 16:42
 * Updated in $/GPRS_RR_Host_2_0/gprs/grr
 * New signals MAC_GRR_RACH_CNF / REJ
 *
 * *****************  Version 1  *****************
 * User: Nick         Date: 2/10/02    Time: 12:44
 * Created in $/GPRS_RR_HOST_V2/gprs/grr
 * Added to newly created host archive Version2
 *
 * *****************  Version 4  *****************
 * User: Damon        Date: 4/09/02    Time: 11:23
 * Updated in $/GPRS_RR_Host/gsm/grr
 * mac_grr_sig_id_t numbering now starts at 0x80
 *
 * *****************  Version 3  *****************
 * User: Damon        Date: 30/08/02   Time: 15:55
 * Updated in $/GPRS_RR_Host/gsm/grr
 * Gave all GRR_MAC_* enum a value for logging purposes
 *
 * *****************  Version 2  *****************
 * User: Nick         Date: 23/08/02   Time: 10:45
 * Updated in $/GPRS_RR_Host/gsm/grr
 * created new pcco_status_t for use in the cell_resel_status message from
 * MAC.
 *
 * *****************  Version 1  *****************
 * User: Damon        Date: 8/07/02    Time: 11:17
 * Created in $/GPRS_RR_Host/gsm/grr
 * Compatible with GSM_RR.02.01.04
 *
 * *****************  Version 1  *****************
 * User: Damon        Date: 3/07/02    Time: 18:51
 * Created in $/GSM_GRR_VU
 * Initial port from 'PerlUTF archive'
 *
 * *****************  Version 16  *****************
 * User: Sally        Date: 7/06/02    Time: 16:10
 * Updated in $/GPRS_MAC_Host/Gprs/gsm headers
 * Remove definitions for tbf access type (moved to GPRSDEF.H and renamed
 * to share definitions with RLC)
 *
 * *****************  Version 15  *****************
 * User: Sally        Date: 13/05/02   Time: 11:27
 * Updated in $/GPRS_MAC_Host/Gprs/gsm headers
 * Add rr_radio_priority_t and change mac_grr_ul_tbf_req_t
 *
 * *****************  Version 14  *****************
 * User: Damon        Date: 12/05/02   Time: 12:20p
 * Updated in $/PerlUTF/gsm/inc
 * Addec function prototype rr_process_mac_grr_store_update(), included
 * when DEBUG_NO_RR is defined. This is for MAC testing purposes.
 *
 * *****************  Version 13  *****************
 * User: Damon        Date: 10/04/02   Time: 10:58a
 * Updated in $/PerlUTF/gsm/inc
 * Added 'bcch_arfcn' into mac_grr_data_ind_t for MAC_GRR_DATA_IND
 *
 * *****************  Version 12  *****************
 * User: Sally        Date: 3/28/02    Time: 12:19p
 * Updated in $/GPRS_MAC_Host/Gprs/gsm headers
 * Updates required to bring inline with RELEASE 02 changes
 *
 * Change to MAC_GRR_DATA_IND contents
 *
 * *****************  Version 11  *****************
 * User: Tony         Date: 3/09/02    Time: 1:34p
 * Updated in $/GPRS_MAC_Host/Gprs/gsm headers
 * added emlpp_present, emlpp_priority, channel_needed to signal
 * MAC_GRR_PAGING_NOTIFICATION
 *
 * *****************  Version 9  *****************
 * User: Damon        Date: 27/02/02   Time: 11:04a
 * Updated in $/PerlUTF/gsm/inc
 * success_failure_t moved from gprsdef.h
 *
 * *****************  Version 7  *****************
 * User: Damon        Date: 29/01/02   Time: 3:53p
 * Updated in $/PerlUTF/gsm/inc
 * Corrected syntactical error in mac_grr_store_update_t.
 *
 * *****************  Version 6  *****************
 * User: Damon        Date: 19/12/01   Time: 11:50a
 * Updated in $/PerlUTF/gsm/inc
 * Amendment to the mac_grr_store_update_t
 *
 * *****************  Version 4  *****************
 * User: Damon        Date: 18/12/01   Time: 3:41p
 * Updated in $/PerlUTF/gsm/inc
 * Removed the RR_ from rr_mac_status_t defines
 *
 * *****************  Version 3  *****************
 * User: Damon        Date: 18/12/01   Time: 2:45p
 * Updated in $/PerlUTF/gsm/inc
 * Added IMH_T message header in mac_grr_store_update_t
 *
 * *****************  Version 2  *****************
 * User: Damon        Date: 18/12/01   Time: 1:43p
 * Updated in $/PerlUTF/gsm/inc
 * Renamed MAC_GRR_CS_PAGING_NOTIFICATION to
 * MAC_GRR_PAGING_NOTIFICATION
 *
 * *****************  Version 1  *****************
 * User: Damon        Date: 18/12/01   Time: 11:49a
 * Created in $/PerlUTF/gsm/inc
 * MAC -> GRR signals and associated types, defines, etc.
***
*****************************************************************************/

#ifndef MAC_GRR_H
#define MAC_GRR_H


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

#include "csn.h"
#include "nas_exp.h"
#include "rr_public_store.h"

#include "gprsdef.h"
#include "gprsdef_g.h"
#include "geran_dual_sim.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*************************************************************************
*  General purpose success/failure flag
*************************************************************************/

typedef enum
{
  MAC_GRR_SUCCESS,
  MAC_GRR_FAILURE

} success_failure_t;

typedef enum
{
  PCCO_SUCCESS,
  PCCO_FAILURE_NO_RESPONSE,
  PCCO_FAILURE_ACCESS_REJ

} pcco_status_t;


/*************************************************************************
*  MAC -> GRR signals
*************************************************************************/

/**************************************************/
/* The enum values below are assigned for logging */
/* purposes - please do not change them!          */
/* If a signal is removed, it's number should be  */
/* unassigned, not re-used                        */
/* GRR->MAC signals number from 0x00              */
/* MAC->GRR signals number from 0x80              */
/**************************************************/
typedef enum
{
   MAC_GRR_DATA_IND                = 0x80
  ,MAC_GRR_CELL_RESEL_STATUS       = 0x81
  ,MAC_GRR_MAC_STATUS              = 0x82
  ,MAC_GRR_PAGING_NOTIFICATION     = 0x83
  ,MAC_GRR_ABNORMAL_RELEASE        = 0x84
  ,MAC_GRR_MEAS_RPT_CNF            = 0x85
  ,MAC_GRR_PCCF_SENT               = 0x86
  ,MAC_GRR_TBF_REJ                 = 0x87
  ,MAC_GRR_TBF_ACCEPT              = 0x88
  ,MAC_GRR_UL_TBF_REQ              = 0x89
  ,MAC_GRR_STORE_UPDATE            = 0x8a
  ,MAC_GRR_RACH_CNF                = 0x8b
  ,MAC_GRR_RACH_REJ                = 0x8c
  ,MAC_GRR_NCELL_DATA_IND          = 0x8d
  ,MAC_GRR_NCELL_DATA_CNF          = 0x8e
  ,MAC_GRR_RESET_CNF               = 0x8f
#ifdef FEATURE_GSM_DTM
  ,MAC_GRR_DTM_CHANNEL_ASSIGN_REJ  = 0x90
#ifdef FEATURE_GSM_EDTM
  ,MAC_GRR_EDTM_PKT_CS_EST_REJ     = 0x91
  ,MAC_GRR_EDTM_CS_RELEASE_IND     = 0x92
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
  ,MAC_GRR_PSHO_CNF                = 0x93
  ,MAC_GRR_PS_ACCESS_CNF           = 0x94
} mac_grr_sig_id_t;

/*************************************************************************
*  MAC_GRR_DATA_IND signal
*************************************************************************/

typedef struct
{
  IMH_T              message_header;
  csn_decoder_ptrs_t data_ptr;
  ARFCN_T            bcch_arfcn;
  gas_id_t           gas_id;
} mac_grr_data_ind_t;

#ifdef FEATURE_GPRS_PS_HANDOVER
typedef struct
{
  IMH_T          message_header;
  psho_status_t  psho_status;
  gas_id_t       gas_id;
} mac_grr_psho_cnf_t;
#endif

/*************************************************************************
*  MAC_GRR_CELL_RESEL_STATUS signal
*************************************************************************/

typedef struct
{
  IMH_T              message_header;
  pcco_status_t      status;
  gas_id_t           gas_id;
} mac_grr_cell_resel_status_t;


/*************************************************************************
*  MAC_GRR_MAC_STATUS signal
*************************************************************************/

typedef byte rr_mac_status_t;

#define MAC_IDLE              0
#define MAC_IDLE_BLOCKED      1
#define MAC_TRANSFER          2
#define MAC_TRANSFER_BLOCKED  3

typedef struct
{
  IMH_T            message_header;
  rr_mac_status_t  mac_status;
  gas_id_t         gas_id;
} mac_grr_mac_status_t;


/*************************************************************************
*  MAC_GRR_PAGING_NOTIFICATION signal
*************************************************************************/

typedef enum
{
  RR_CS_PAGING,
  RR_PS_PAGING,
  RR_PS_PAGING_ABNORMAL
} grr_paging_type_T;

typedef struct
{
  IMH_T             message_header;
  grr_paging_type_T paging_type;
  boolean           emlpp_present;
  uint8             emlpp_priority;
  uint8             channel_needed;   /* only present if cs page */
  uint8             identity_type;    /* only present if cs page (IMSI or TMSI) */
  gas_id_t          gas_id;
} mac_grr_paging_notification_t;


/*************************************************************************
*  MAC_GRR_ABNORMAL_RELEASE signal
*************************************************************************/

typedef byte rr_abnormal_release_cause_t;

#define RR_FULL_SYS_INFO_REQ  0
#define RR_PART_SYS_INFO_REQ  1
#define RR_RETURN_TO_IDLE     2
#define RR_CELL_RESEL_REQ     3
#define RR_CONTENTION_FAILED  4

typedef struct
{
  IMH_T                        message_header;
  rr_abnormal_release_cause_t  cause;
  gas_id_t                     gas_id;
} mac_grr_abnormal_release_t;


/*************************************************************************
*  MAC_GRR_MEAS_RPT_CNF signal
*************************************************************************/

typedef struct
{
  IMH_T              message_header;
  success_failure_t  status;
  gas_id_t           gas_id;
} mac_grr_meas_rpt_cnf_t;


/*************************************************************************
*  MAC_GRR_PCCF_SENT signal
*************************************************************************/
typedef enum
{
 FAILED_TO_SEND_PCCF,
 SUCCESSFULLY_SENT_PCCF
}pccf_status_t;

typedef struct
{
  IMH_T              message_header;
  pccf_status_t      pccf_status;
  gas_id_t           gas_id;
} mac_grr_pccf_sent_t;


/*************************************************************************
*  MAC_GRR_TBF_REJ signal
*************************************************************************/

typedef struct
{
  IMH_T              message_header;
  gas_id_t           gas_id;
} mac_grr_tbf_rej_t;


/*************************************************************************
*  MAC_GRR_TBF_ACCEPT signal
*************************************************************************/

typedef struct
{
  IMH_T              message_header;
  gas_id_t           gas_id;
} mac_grr_tbf_accept_t;


/*************************************************************************
*  MAC_GRR_UL_TBF_REQ signal
*************************************************************************/

/* Priority Class */
#define RR_PRIORITY_CLASS_1  1
#define RR_PRIORITY_CLASS_2  2
#define RR_PRIORITY_CLASS_3  3
#define RR_PRIORITY_CLASS_4  4

typedef enum
{
  RR_PKT_EST_CAUSE_UNKNOWN,
  RR_PKT_EST_CAUSE_USER_DATA,
  RR_PKT_EST_CAUSE_PAGE_RESP,
  RR_PKT_EST_CAUSE_CELL_UPD,
  RR_PKT_EST_CAUSE_MM

} rr_pkt_est_cause_t;

typedef struct
{
  uint8                 access_type;
  uint8                 radio_priority;
  rr_pkt_est_cause_t    pkt_est_cause;
  #ifdef FEATURE_GSM_DTM
  uint8                 peak_thr_class;
  uint8                 rlc_mode;
  uint8                 llc_pdu_type;
  uint16                rlc_octet_cnt;
  boolean               pfi_present;
  uint8                 pfi;
  #endif /* FEATURE_GSM_DTM */

} rr_channel_req_info_t;

typedef struct
{
  IMH_T                  message_header;
  rr_channel_req_info_t  channel_req_info;
#ifdef FEATURE_GSM_DTM
  uint16                 gttp_gmm_sig_octets;  /*Size of GMM signaling PDU in octets */
#endif /* FEATURE_GSM_DTM */                   /*(0 = not GMM signalling) */
  gas_id_t               gas_id;
} mac_grr_ul_tbf_req_t;


/*************************************************************************
*  MAC_GRR_STORE_UPDATE signal
*************************************************************************/

/* Used by MAC to update parameters in the Public Store */
/* The 'id' indicates which of the parameter groups in the union is being */
/* updated */
typedef struct
{
  IMH_T                            message_header;
  rr_public_store_id_t             id;
  union
  {
    rrps_global_power_ctrl_params_t  global_power_control_params;

  }                                update;
  gas_id_t                         gas_id;
} mac_grr_store_update_t;


/*************************************************************************
*  MAC_GRR_RACH_CNF signal
*************************************************************************/

typedef struct
{
  IMH_T              message_header;
  gas_id_t           gas_id;
} mac_grr_rach_cnf_t;


/*************************************************************************
*  MAC_GRR_RACH_REJ signal
*************************************************************************/

typedef struct
{
  IMH_T              message_header;
  gas_id_t           gas_id;
} mac_grr_rach_rej_t;


/*************************************************************************
*  MAC_GRR_NCELL_DATA_IND signal
*************************************************************************/

typedef struct
{
  IMH_T              message_header;
  csn_decoder_ptrs_t data_ptr;
  uint8              container_id;
  uint8              protocol;
  gas_id_t           gas_id;
} mac_grr_ncell_data_ind_t;

/*************************************************************************
*  MAC_GRR_NCELL_DATA_CNF signal
*************************************************************************/

typedef struct
{
  IMH_T              message_header;
  uint8              container_id;
  ARFCN_T            arfcn;
  uint8              msgs_left;
  gas_id_t           gas_id;
} mac_grr_ncell_data_cnf_t;

/*************************************************************************
*  MAC_GRR_RESET_CNF
*************************************************************************/

typedef struct
{
  IMH_T              message_header;
  gas_id_t           gas_id;
} mac_grr_reset_cnf_t;

#ifdef FEATURE_GSM_DTM

/*************************************************************************
*  MAC_GRR_DTM_PACKET_CS_REQUEST_REJ signal
*************************************************************************/
typedef struct
{
  IMH_T              message_header;
  gas_id_t           gas_id;
} mac_grr_dtm_packet_cs_request_rej_t;

/*************************************************************************
*  MAC_GRR_DTM_CHANNEL_ASSIGN_REJ signal
*************************************************************************/
typedef struct
{
  IMH_T              message_header;
  uint8              RR_cause;
  gas_id_t           gas_id;
} mac_grr_dtm_channel_assign_rej_t;

#ifdef FEATURE_GSM_EDTM
typedef enum
{
  EDTM_CS_REJ_T3196_EXPIRY            = 0,
  EDTM_CS_REJ_COUNTDOWN_IN_PROGRESS   = 1,
  EDTM_CS_REJ_PS_HANDOVER_IN_PROGRESS = 2,
  EDTM_CS_REJ_TBF_RELEASE_IN_PROGRESS = 3,
  EDTM_CS_REJ_CANNOT_SERVICE_REQUEST  = 4,
  EDTM_CS_REJ_CAUSE_UNSPECIFIED       = 5
} mac_grr_edtm_cs_rej_cause_t;

/*************************************************************************
*  MAC_GRR_EDTM_PKT_CS_EST_REJ signal
*************************************************************************/
typedef struct
{
  IMH_T                       message_header;
  mac_grr_edtm_cs_rej_cause_t reject_cause;
  gas_id_t                    gas_id;
} mac_grr_edtm_pkt_cs_est_rej_t;

/*************************************************************************
*  MAC_GRR_EDTM_CS_RELEASE_IND signal
*************************************************************************/

typedef enum
{
  EDTM_CS_REL_ENHANCED = 0, /* Enhanced CS release */
  EDTM_CS_REL_ABNORMAL = 1  /* MAC is abnormally releasing all TBFs, perform immediate CS release */
}mac_grr_edtm_cs_rel_cause_t;

typedef struct
{
  IMH_T                       message_header;
  mac_grr_edtm_cs_rel_cause_t release_cause;
  gas_id_t                    gas_id;
} mac_grr_edtm_cs_release_ind_t;

#endif /* FEATURE_GSM_EDTM */

#endif /*FEATURE_GSM_DTM*/

/*************************************************************************
*  MAC_GRR_PS_ACCESS_CNF signal
*************************************************************************/

typedef struct
{
  IMH_T              message_header;
  gas_id_t           gas_id;
} mac_grr_ps_access_cnf_t;

/*************************************************************************
*  Union of all GRR to MAC messages
*************************************************************************/

typedef union
{
  mac_grr_data_ind_t                mac_grr_data_ind;
  mac_grr_cell_resel_status_t       mac_grr_cell_resel_status;
  mac_grr_mac_status_t              mac_grr_mac_status;
  mac_grr_paging_notification_t     mac_grr_paging_notification;
  mac_grr_abnormal_release_t        mac_grr_abnormal_release;
  mac_grr_meas_rpt_cnf_t            mac_grr_meas_rpt_cnf;
  mac_grr_pccf_sent_t               mac_grr_pccf_sent;
  mac_grr_tbf_rej_t                 mac_grr_tbf_rej;
  mac_grr_tbf_accept_t              mac_grr_tbf_accept;
  mac_grr_ul_tbf_req_t              mac_grr_ul_tbf_req;
  mac_grr_store_update_t            mac_grr_store_update;
  mac_grr_rach_cnf_t                mac_grr_rach_cnf;
  mac_grr_rach_rej_t                mac_grr_rach_rej;
  mac_grr_ncell_data_ind_t          mac_grr_ncell_data_ind;
  mac_grr_ncell_data_cnf_t          mac_grr_ncell_data_cnf;
  mac_grr_reset_cnf_t               mac_grr_reset_cnf;
#ifdef FEATURE_GSM_DTM
  mac_grr_dtm_packet_cs_request_rej_t mac_grr_dtm_packet_cs_request_rej;
  mac_grr_dtm_channel_assign_rej_t    mac_grr_dtm_channel_assign_rej;
#ifdef FEATURE_GSM_EDTM
  mac_grr_edtm_pkt_cs_est_rej_t       mac_grr_edtm_pkt_cs_est_rej;
  mac_grr_edtm_cs_release_ind_t       mac_grr_edtm_cs_release_ind;
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
#ifdef FEATURE_GPRS_PS_HANDOVER
  mac_grr_psho_cnf_t                  mac_grr_psho_cnf;
#endif
  mac_grr_ps_access_cnf_t             mac_grr_ps_access_cnf;
} mac_grr_sig_t;



/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

#ifdef DEBUG_NO_RR
#error code not present
#endif


#endif /* MAC_GRR_H */

/*** EOF: don't remove! ***/

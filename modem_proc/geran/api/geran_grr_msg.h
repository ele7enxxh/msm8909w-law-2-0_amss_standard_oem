/**
  @file geran_grr_msg.h

  @brief Defines the external Message Router interface for the GERAN GRR module.
*/

/*============================================================================

  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/geran_grr_msg.h#1 $ */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/11   mm      Added definitions for CGI requests from LTE
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
02/18/10   sk      Added redirection payload information
                   Added dedicated priority field for reselection and
                   redirection request structs
01/22/10   gjohn   Initial revision
===========================================================================*/

#ifndef GERAN_GRR_MSG_H
#define GERAN_GRR_MSG_H

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "msgr.h"
#include "msgr_umid.h"
#include "sys.h"
#include "sys_type.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

#include "geran_grr_types.h"
#include "msgr_geran.h"
#ifdef FEATURE_LTE
#include "lte_rrc_irat_msg.h"
#endif /* FEATURE_LTE */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#ifdef FEATURE_LTE
#define GERAN_CS_MAX_CAP_CONTAINER_LENGTH 39
#define GERAN_PS_MAX_CAP_CONTAINER_LENGTH 52

#define GERAN_GRR_REDIR_MAX_FREQ          33

#define GERAN_GRR_LTE_GERAN_FREQ_IND_MAX_FREQ  33

#define GERAN_GRR_MAX_REQ_MSG_SIZE sizeof(geran_grr_msg_req_u)
#define GERAN_GRR_MAX_RSP_MSG_SIZE sizeof(geran_grr_msg_rsp_u)

#define GERAN_GRR_LTE_CCO_REQ_MAX_SI_MESSAGES 10 /* DEPRECATED */
#define GERAN_GRR_LTE_MAX_SI_MESSAGES         10
#define GERAN_GRR_LTE_CGI_MAX_NUM_ARFCN       32

#define GERAN_GRR_MAX_CELL_INFO               32U

#define GERAN_GRR_LTE_MAX_INTERRAT_MEASUREMENTS 32U

#define GERAN_GRR_MAX_LTE_DEPRI_FREQ LTE_IRAT_MAX_EUTRAN_FREQ
#endif /* FEATURE_LTE */

#define GERAN_GRR_MAX_NCELL_PLUS_SCELL_INFO    7
#define CA_LEN                            64


/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*

Requests sent to GERAN GRR.

*/

enum
{
  /*! @brief The first message in the enum */
  GERAN_GRR_REQ_FIRST = MSGR_UMID_BASE(MSGR_GERAN_GRR, MSGR_TYPE_REQ),

#ifdef FEATURE_LTE
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_CS_CAPABILITIES,      0x01, geran_grr_lte_cs_capabilities_req_t),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_PS_CAPABILITIES,      0x02, geran_grr_lte_ps_capabilities_req_t),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_RESEL,                0x03, geran_grr_lte_resel_req_t),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_ABORT_RESEL,          0x04, none),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_REDIR,                0x05, geran_grr_lte_redir_req_t),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_ABORT_REDIR,          0x06, none),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_CCO,                  0x07, geran_grr_lte_cco_req_t),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_ABORT_CCO,            0x08, none),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_PLMN_SRCH,            0x09, lte_irat_plmn_srch_req_s),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_ABORT_PLMN_SRCH,      0x0a, lte_irat_abort_plmn_srch_req_s),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_DEDICATED_PRIORITIES, 0x0b, none),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_GET_CGI,              0x0c, geran_grr_lte_get_cgi_req_t),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_ABORT_CGI,            0x0d, geran_grr_lte_abort_cgi_req_t),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_HO,                   0x0e, geran_grr_lte_ho_req_t),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_ABORT_HO,             0x0f, none),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_SGLTE_SERVING_MEAS,   0x10, none),
#endif /* FEATURE_LTE */
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, TDS_SGLTE_SERVING_MEAS,   0x11, none),
#ifdef FEATURE_LTE
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_SGLTE_NEIGHBOR_MEAS,  0x12, none),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_GET_PLMN_PRTL_RESULTS,0x13, none),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, LTE_DEPRI_FREQ,           0x14, geran_grr_lte_depri_freq_req_type),
#endif /* FEATURE_LTE */
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, MTC_CFG,                  0x15, geran_grr_mtc_cfg_req_t),  
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, QMI_CELL_INFO,            0x16, geran_grr_qmi_cell_info_req_t),
  MSGR_DEFINE_UMID(GERAN, GRR, REQ, FAKE_GCELL_ACTION,        0x17, geran_grr_fake_gcell_action_req_t),

  /*! @brief The last message in the enum */
  GERAN_GRR_REQ_LAST = GERAN_GRR_FAKE_GCELL_ACTION_REQ
};

#ifdef FEATURE_LTE
typedef struct
{
  /*! @brief Message header. */
  msgr_hdr_struct_type hdr;

  /*! @brief RAT capabilities. */
  lte_irat_capabilities_s rat_capabilities;

} geran_grr_lte_cs_capabilities_req_t;

typedef struct
{
  /*! @brief Message header. */
  msgr_hdr_struct_type hdr;

  /*! @brief RAT capabilities. */
  lte_irat_capabilities_s rat_capabilities;

} geran_grr_lte_ps_capabilities_req_t;

typedef int16 geran_grr_rssi_t;

typedef struct
{
  /*! @brief Message header. */
  msgr_hdr_struct_type              hdr;

  /*! @brief Indicates ARFCN of the GSM cell to be selected. */
  uint16 arfcn;

  /*! @brief Indicates whether the ARFCN belongs to 1800 or 1900 band
  if it lies in the 'overlap' range 512 - 810. */
  geran_grr_band_ind_e band_ind;

  /*! @brief Indicates cell id of the cell to be selected. */
  geran_grr_gsm_cell_id_s           cell_id;

  /*! The averaged RSSI value for the cell in dB */
  geran_grr_rssi_t rssi;

  /*! @brief Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  /*! @brief PLMNId of the requested PLMN */
  sys_plmn_id_s_type                requested_PLMN;

  /*! @brief RAT priority list */
  sys_rat_pri_list_info_s_type      rat_pri_list;

  /*! @brief Dedicated priority info*/
  lte_irat_dedicated_priority_info_s dedicated_priority_info;

} geran_grr_lte_resel_req_t;

typedef struct
{
  /*! @brief Indicates arfcn of the GSM cell*/
  uint16                arfcn;

  /*! @brief Indicates whether ARFCNs in the overlap region of 1800 and 1900
  bands belong to 1800 or 1900. */
  geran_grr_band_ind_e  band_ind;

} geran_grr_redir_freq_type;

typedef struct
{
  /*! @brief Indicates number of redirection frequencies provided */
  uint8                       num_of_freq;

  /*! @brief Indicates frequency list */
  geran_grr_redir_freq_type   freq_list[GERAN_GRR_REDIR_MAX_FREQ];

} geran_grr_redir_info_type;

typedef struct
{

  /*! @brief Indicates arfcn of the GSM cell*/
  uint16                    arfcn;

  /*! @brief Indicates whether ARFCNs in the overlap region of 1800 and 1900
  bands belong to 1800 or 1900. */
  geran_grr_band_ind_e      band_ind;

  /*! @brief Indicates the BSIC of the GSM cell */
  geran_grr_gsm_cell_id_s   cell_id;

  /*! Number of SI containers */
  uint8                     si_cont_count;

  /*! Total size of all SI containers */
  uint8                     total_si_cont_size;

} geran_grr_cell_info_list_type;

typedef struct
{
  /*! @brief Message header. */
  msgr_hdr_struct_type               hdr;

  /*! @brief Message router Attachments */
  msgr_attach_s                      msgr_attach[GERAN_GRR_MAX_CELL_INFO];

  /*! @brief Number of cells for which the N/W has sent SI containers */
  uint8                              num_cell_info;

  /*! @brief Cell Info List containing SIs */
  geran_grr_cell_info_list_type      cell_info[GERAN_GRR_MAX_CELL_INFO];

  /*! @brief Redirection band/freq list */
  geran_grr_redir_info_type          redir_info;

  /*! @brief Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type  network_select_mode;

  /*! @brief PLMN Id of the requested PLMN */
  sys_plmn_id_s_type                 requested_PLMN;

  /*! @brief RAT priority list */
  sys_rat_pri_list_info_s_type       rat_pri_list;

  /*! @brief Dedicated priority info */
  lte_irat_dedicated_priority_info_s dedicated_priority_info;

} geran_grr_lte_redir_req_t;

typedef struct
{
  /*! @brief Message header. */
  msgr_hdr_struct_type               hdr;

  /*! @brief Indicates ARFCN of the GSM cell to be selected. */
  uint16                             arfcn;

  /*! @brief Indicates whether the ARFCN belongs to 1800 or 1900 band
  if it lies in the 'overlap' range 512 - 810. */
  geran_grr_band_ind_e               band_ind;

  /*! @brief Indicates cell id of the cell to be selected. */
  geran_grr_gsm_cell_id_s            cell_id;

  /*! @brief Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type  network_select_mode;

  /*! @brief PLMNId of the requested PLMN */
  sys_plmn_id_s_type                 requested_PLMN;

  /*! @brief RAT priority list */
  sys_rat_pri_list_info_s_type       rat_pri_list;

  /*! @brief T309 - time allowed in milliseconds for Cell Change Order
  procedure to complete. */
  uint32                             t304;

  boolean                            network_control_order_present;

  /* Network Control Order.  Values are 0, 1, 2 as defined in 3GPP 44.018 */
  uint8                              network_control_order;

  /* Number of OTA-encoded GERAN SI messages available for the target cell. */
  uint8                              no_of_si_messages;

  /* OTA-encoded GERAN SI messages available for the target cell. */
  geran_grr_si_message_t             si_messages[GERAN_GRR_LTE_MAX_SI_MESSAGES];

  /* CSFB indicator flag */
  boolean                            is_cco_for_csfb;

} geran_grr_lte_cco_req_t;

/*!@brief                        
  This structure defines the CGI information sent by GERAN to LTE
*/
typedef struct
{
  /* Cell id for the G cell for which CGI info was requested */
  uint32 cell_id;

  /* PLMN id for the G cell for which CGI info was requested */
  sys_plmn_id_s_type  plmn_id;

  /* Location area code for the cell for which CGI info was requested */
  uint8 lac[2];

  /* OBSOLETE - rac_included will be removed in a future version of this file */
  boolean rac_included;

  /* If RAC is not applicable (i.e. cell does not support GPRS), GRR should set
  the GERAN_GRR_LTE_CGI_RSP_RAC mask and set this field to FALSE. If RAC is
  applicable not acquired, GRR should not set the GERAN_GRR_LTE_CGI_RSP_RAC mask  */
  boolean rac_present;

  /* Rounting area code for the cell for which CGI info was requested */
  uint8 rac;

} geran_grr_lte_cgi_info_t;

/* Preprocessor definitions describing the bit positions in the geran_grr_lte_cgi_rsp_mask_e.
  This shows the different elements in CGI response and their corresponding bitmask. */
#define GERAN_GRR_LTE_CGI_RSP_NONE    0x00
#define GERAN_GRR_LTE_CGI_RSP_CELL_ID 0x01
#define GERAN_GRR_LTE_CGI_RSP_PLMN_ID 0x02
#define GERAN_GRR_LTE_CGI_RSP_LAC     0x04
#define GERAN_GRR_LTE_CGI_RSP_RAC     0x08
#define GERAN_GRR_LTE_CGI_RSP_RSSI    0x10
#define GERAN_GRR_LTE_CGI_RSP_ALL     0x1F

/*!@brief This structure contains the elements returned in CGI RSP from G */
typedef struct
{
  /* Bitmask indicates the elements in CGI related info that are included in  the response */
  uint8 cgi_rsp_info_bitmask;

  /*! RSSI (RXLEV), range is 0 - 63 */
  uint8                          rssi;

  /* CGI information for the requested cell */
  geran_grr_lte_cgi_info_t       cgi_info;

} geran_grr_lte_cgi_rsp_info_t;

/*!@brief                        
  This structure contains the message type for GERAN_GRR_LTE_GET_CGI_REQ, sent by
  LTE RRC to GRR for acquiring CGI information for a particular GERAN cell 
*/
typedef struct
{
  /*! message header type */
  msgr_hdr_s              msg_hdr;

  /*! Num of ARFCNs passed */
  uint8                   num_arfcns;

  /*! ARFCNs in GERAN where the passed BSIC can be found. L RRC will try to determine
    the ARFCN where the BSIC can be found based on prior GERAN measurements and will
    set it as the first ARFCN in the array */
  uint16                  arfcns[GERAN_GRR_LTE_CGI_MAX_NUM_ARFCN];

  /*! @brief Indicates whether the passed ARFCN belongs to 1800 or 1900 band
  if it lies in the 'overlap' range 512 - 810. */
  geran_grr_band_ind_e    band_ind;

  /*! Cell id (BSIC) of the cell for which CGI info is requested */
  geran_grr_gsm_cell_id_s cell_id;     

  /*! Time in ms that GRR has, for acquiring CGI info; GRR has to send a response 
    back to LTE with a success/failure within this time */
  uint32                  time_available_for_cgi;

  /*! CGI related info that was received in the last CGI rsp from G */
  geran_grr_lte_cgi_rsp_info_t last_cgi_rsp_info;

} geran_grr_lte_get_cgi_req_t;

/*!@brief                        
	This structure contains the message type for GERAN_GRR_LTE_DEPRI_FREQ_REQ, sent by
	LTE RRC to GRR
*/
typedef struct
{
	/* message header type */
	msgr_hdr_s      msg_hdr;
 
	/*! Indicates if all EUTRA neighbor freqs should be deprioritized */
	boolean                  depri_all_freq;
 
	/*! Number of freqs being deprioritized; Will be set to a valid value
	    only if the depri_all_freq boolean is set to FALSE
	    If num_depri_freq = 0 and depri_all_freq = FALSE, this implies
	    LTE deprioritization is cleared*/
	uint8                    num_depri_freq;
 
	/*! Set of deprioritized EUTRA frequencies. This will be valid only
	  if the depri_all_freq boolean is set to FALSE */
	uint32                   depri_freqs[GERAN_GRR_MAX_LTE_DEPRI_FREQ];
 
} geran_grr_lte_depri_freq_req_type;


/*!@brief                        
  This structure contains the message type for GERAN_GRR_LTE_ABORT_CGI_REQ, sent by
  LTE RRC to GRR for aborting CGI acquistion for a particular GERAN cell 
*/
typedef struct
{
  /* message header type */
  msgr_hdr_s msg_hdr;

} geran_grr_lte_abort_cgi_req_t;

/*!@brief This structure contains the inter-rat measurement data list for timing transfer 
*/
typedef struct
{
  byte number_of_freq;
  geran_grr_irat_measurement_t geran_measurements[GERAN_GRR_LTE_MAX_INTERRAT_MEASUREMENTS];
} geran_grr_irat_measurement_list_t;


/*!@brief                        
  This structure contains the message type for GERAN_GRR_LTE_HO_REQ, sent by
  LTE RRC to GRR for Handover request to a GERAN cell
*/

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  msgr_attach_s dsm_attach; /*!< dsm attachment
                                 containing the handover Command */

  lte_irat_umts_sec_info_s security_keys;

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  /* PLMNId of the PLMN UE is camped on the LTE side */
  sys_plmn_id_s_type  plmn_id;

  sys_rat_pri_list_info_s_type   rat_pri_list_info;

  /*! @brief Indicates whether the ARFCN belongs to 1800 or 1900 band
  if it lies in the 'overlap' range 512 - 810. */
  geran_grr_band_ind_e     band_ind;

  /* Number of OTA-encoded GERAN SI messages available for the target cell. */
  uint8                              no_of_si_messages;

  /* OTA-encoded GERAN SI messages available for the target cell. */
  geran_grr_si_message_t             si_messages[GERAN_GRR_LTE_MAX_SI_MESSAGES];

  /* Inter-RAT measurement data for timing transfer */
  geran_grr_irat_measurement_list_t  meas_data_list;

} geran_grr_lte_ho_req_t;
#endif /* FEATURE_LTE */

typedef struct
{
  msgr_hdr_struct_type hdr;
} geran_grr_qmi_cell_info_req_t;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint16                 arfcn;              /* arfcn of the cell */
  uint8                  bsic;               /* bsic of the cell */
  sys_plmn_id_s_type     plmn_id;            /* PLMN id of the cell */
  uint16                 cell_id;            /* Cell identifier of the cell */
  uint32                 location_area_code; /* Location Area Code for the cell */
  uint32                 barred_time;        /* time (in milli seconds) cell to be kept barred */
} geran_grr_fake_gcell_action_req_t; 

/*! @brief Message for scaling Idle DRX / skipping Idle Meas Request
*/
typedef struct
{
  /*! @brief Message router header. */
  msgr_hdr_struct_type msg_hdr;

  /*! @brief To be set to TRUE when drx_scaling_factor is configured */  
  boolean              drx_scaling_factor_valid;

  /*! @brief Scaling factor for increasing DRX cycle */ 
  uint8                drx_scaling_factor;

  /*! @brief To be set to TRUE when skip_idle_mode_meas is configured */
  boolean              skip_idle_mode_meas_valid;

  /*! @brief To be set to TRUE when idle mode meas have to be skipped */
  boolean              skip_idle_mode_meas;
} geran_grr_mtc_cfg_req_t;


typedef union
{
  msgr_hdr_struct_type                hdr;
#ifdef FEATURE_LTE
  geran_grr_lte_cs_capabilities_req_t lte_cs_capabilities_req;
  geran_grr_lte_ps_capabilities_req_t lte_ps_capabilities_req;
  geran_grr_lte_resel_req_t           lte_resel_req;
  geran_grr_lte_redir_req_t           lte_redir_req;
  geran_grr_lte_cco_req_t             lte_cco_req;
  lte_irat_plmn_srch_req_s            plmn_srch_req;
  lte_irat_abort_plmn_srch_req_s      abort_plmn_srch_req;
  geran_grr_lte_get_cgi_req_t         lte_get_cgi_req;
  geran_grr_lte_abort_cgi_req_t       lte_abort_cgi_req;
  geran_grr_lte_ho_req_t              lte_ho_req;

  geran_grr_lte_depri_freq_req_type   lte_depri_freq_req;
#endif /* FEATURE_LTE */
  geran_grr_mtc_cfg_req_t             mtc_cfg_req;
  geran_grr_qmi_cell_info_req_t       qmi_cell_info_req;
  geran_grr_fake_gcell_action_req_t   fake_gcell_action_req;
} geran_grr_msg_req_u;

/**

Responses sent from GERAN GRR.

*/

enum
{
  /*! @brief The first message in the enum */
  GERAN_GRR_RSP_FIRST = MSGR_UMID_BASE(MSGR_GERAN_GRR, MSGR_TYPE_RSP),

#ifdef FEATURE_LTE
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_CS_CAPABILITIES,      0x01, geran_grr_lte_cs_capabilities_rsp_t),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_PS_CAPABILITIES,      0x02, geran_grr_lte_ps_capabilities_rsp_t),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_RESEL_FAILED,         0x03, geran_grr_lte_resel_failed_rsp_t),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_ABORT_RESEL,          0x04, none),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_REDIR_FAILED,         0x05, geran_grr_lte_redir_failed_rsp_t),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_ABORT_REDIR,          0x06, none),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_CCO,                  0x07, geran_grr_lte_cco_rsp_t),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_ABORT_CCO,            0x08, none),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_PLMN_SRCH,            0x09, lte_irat_plmn_srch_rsp_s),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_ABORT_PLMN_SRCH,      0x0a, lte_irat_abort_plmn_srch_rsp_s),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_DEDICATED_PRIORITIES, 0x0b, none),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_GET_CGI,              0x0c, geran_grr_lte_get_cgi_rsp_t),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_ABORT_CGI,            0x0d, geran_grr_lte_abort_cgi_rsp_t),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_HO_FAILED,            0x0e, geran_grr_lte_ho_failed_rsp_t),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_ABORT_HO,             0x0f, none),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_SGLTE_SERVING_MEAS,   0x10, geran_grr_irat_sglte_serving_meas_rsp_t),
#endif /* FEATURE_LTE */
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, TDS_SGLTE_SERVING_MEAS,   0x11, geran_grr_irat_sglte_serving_meas_rsp_t),
#ifdef FEATURE_LTE
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, LTE_SGLTE_NEIGHBOR_MEAS,  0x12, geran_grr_irat_sglte_neighbor_meas_rsp_t),
#endif /* FEATURE_LTE */
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, MTC_CFG,                  0x13, geran_grr_mtc_cfg_rsp_t),
  MSGR_DEFINE_UMID(GERAN, GRR, RSP, QMI_CELL_INFO,            0x14, geran_grr_qmi_cell_info_rsp_t),
  /*! @brief The last message in the enum */
  GERAN_GRR_RSP_LAST = GERAN_GRR_QMI_CELL_INFO_RSP
};

#ifdef FEATURE_LTE
typedef enum
{
  GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_UNDEFINED          = 0x00,
  GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_CELL_BARRED        = 0x01,
  GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_LOW_C1             = 0x02,
  GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_PLMN_MISMATCH      = 0x03,
  GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_LAI      = 0x04,
  GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_BSIC_MISMATCH      = 0x05,
  GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_BCCH_NOT_FOUND     = 0x06,
  GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_BCCH_DECODE_FAILED = 0x07,
  GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_MAX
} geran_grr_lte_resel_failure_cause_type_e;

typedef enum
{
  GERAN_GRR_LTE_CCO_STATUS_UNDEFINED,
  GERAN_GRR_LTE_CCO_STATUS_SUCCESS,
  GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_ACQUIRE,
  GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_RACH,
  GERAN_GRR_LTE_CCO_STATUS_ABORTED_DUE_TO_HIGH_PRI_REQ,
  GERAN_GRR_LTE_CCO_STATUS_MAX
} geran_grr_lte_cco_status_e;

/* RRC Handover failure causes */
typedef enum
{
  GERAN_GRR_LTE_HO_FAILURE_CAUSE_UNDEFINED,
  GERAN_GRR_LTE_HO_FAILURE_CAUSE_INVALID_STATE,
  GERAN_GRR_LTE_HO_FAILURE_CAUSE_PROTOCOL_ERROR,
  GERAN_GRR_LTE_HO_FAILURE_CAUSE_INVALID_CONFIGURATION,
  GERAN_GRR_LTE_HO_FAILURE_CAUSE_PHYSICAL_CHANNEL_FAILURE,
  GERAN_GRR_LTE_HO_FAILURE_CAUSE_INVALID_CIPHER_ALGORITHM,
  GERAN_GRR_LTE_HO_FAILURE_CAUSE_FREQUENCY_NOT_IMPLEMENTED,
  GERAN_GRR_LTE_HO_FAILURE_CAUSE_MAX
} geran_grr_lte_ho_failure_cause_type_e;

typedef struct
{
  /*! @brief Message header */
  msgr_hdr_struct_type  hdr;

  /*! @brief Length of the encoded CS capabilities data. */
  uint32                capabilities_container_length;

  /*! @brief Encoded CS capabilities. */
  uint8                 capabilities_container[GERAN_CS_MAX_CAP_CONTAINER_LENGTH];

} geran_grr_lte_cs_capabilities_rsp_t;

typedef struct
{
  /*! @brief Message header */
  msgr_hdr_struct_type hdr;

  /*! @brief Length of the encoded PS capabilities data. */
  uint32                capabilities_container_length;

  /*! @brief Encoded PS capabilities. */
  uint8                 capabilities_container[GERAN_PS_MAX_CAP_CONTAINER_LENGTH];

  /*! @brief DTM capability. */  
  boolean               geran_dtm_is_supported;

} geran_grr_lte_ps_capabilities_rsp_t;

typedef struct
{
  /* Message header. */
  msgr_hdr_struct_type                     hdr;

  /* Indicates reselection failure cause. */
  geran_grr_lte_resel_failure_cause_type_e resel_fail_cause;

} geran_grr_lte_resel_failed_rsp_t;

typedef struct
{
  /* Message header. */
  msgr_hdr_struct_type hdr;

} geran_grr_lte_redir_failed_rsp_t;

typedef struct
{
  /* Message header. */
  msgr_hdr_struct_type       hdr;

  /* Indicates Cell Change Order failure cause. */
  geran_grr_lte_cco_status_e status;

} geran_grr_lte_cco_rsp_t;

typedef struct
{
  /* Message header. */
  msgr_hdr_struct_type       hdr;

  /* Indicates Handover failure cause. */
  geran_grr_lte_ho_failure_cause_type_e ho_fail_cause;

} geran_grr_lte_ho_failed_rsp_t;

typedef struct
{
  /* Message header */
  msgr_hdr_s msg_hdr; 

  /* Boolean indicating if dedicated priorities are valid or not */
  boolean dedicated_pri_valid;

  /* PLMN ID in which the dedicated priorities are received */
  sys_plmn_id_s_type    plmn;

  /* Dedicated priority info */
  lte_irat_dedicated_priority_info_s dedicated_pri_info;

} geran_grr_lte_dedicated_priorities_rsp_t;

/*!@brief                        
  This structure contains the message type for GERAN_GRR_LTE_GET_CGI_RSP, sent by
  GRR to LTE RRC in response to GERAN_GRR_LTE_GET_CGI_REQ
*/
typedef struct
{
  /* message header type */
  msgr_hdr_s               msg_hdr;

  /* ARFCN of the cell for which CGI info was read */
  uint16                   arfcn;

  /*! @brief Indicates whether the ARFCN belongs to 1800 or 1900 band
  if it lies in the 'overlap' range 512 - 810. */
  geran_grr_band_ind_e     band_ind;

  /*! Cell id of the cell for which CGI info is requested */
  geran_grr_gsm_cell_id_s  cell_id;

  /*! CGI rsp info from G */
  geran_grr_lte_cgi_rsp_info_t cgi_rsp_info;

} geran_grr_lte_get_cgi_rsp_t;

/*!@brief                        
  This structure contains the message type for GERAN_GRR_LTE_ABORT_CGI_RSP, sent by
  GRR to LTE RRC in response to GERAN_GRR_LTE_ABORT_CGI_REQ
*/
typedef struct
{
  /* message header type */
  msgr_hdr_s msg_hdr;

  /* ARFCN of the cell for which CGI info was read */
  uint16                   arfcn;

  /*! @brief Indicates whether the ARFCN belongs to 1800 or 1900 band
  if it lies in the 'overlap' range 512 - 810. */
  geran_grr_band_ind_e     band_ind;

  /*! Cell id of the cell for which CGI info is requested */
  geran_grr_gsm_cell_id_s  cell_id;

  /*! CGI rsp info from G */
  geran_grr_lte_cgi_rsp_info_t cgi_rsp_info;

} geran_grr_lte_abort_cgi_rsp_t;
#endif /* FEATURE_LTE */

typedef struct
{
  /*! message header type */
  msgr_hdr_s              msg_hdr;
  
  /*! TRUE if GSM is in service and serving cell measurements are available,
  FALSE if GSM is not in service. */
  boolean                 service_available;
  
  /*! ARFCN of the cell for which measurement information is provided. */
  uint16                  arfcn;
  
  /*! @brief Indicates whether the ARFCN belongs to 1800 or 1900 band
  if it lies in the 'overlap' range 512 - 810. */
  geran_grr_band_ind_e    band_ind;

  /*! @brief TRUE if the cell_id is known, FALSE if not. */
  boolean                 cell_id_valid;

  /*! Cell id of the cell for which measurement information is provided. */
  geran_grr_gsm_cell_id_s cell_id;

  /* RSSI, range -111 to -48 */
  int16                   rssi_dBm;

} geran_grr_irat_sglte_serving_meas_rsp_t;

typedef struct
{  
  /*! TRUE if this cell is the current serving cell, FALSE otherwise. */
  boolean                 is_serving_cell;

  /*! ARFCN of the cell for which measurement information is provided. */
  uint16                  arfcn;
  
  /*! Indicates whether the ARFCN belongs to 1800 or 1900 band if it lies 
      in the 'overlap' range 512 - 810. */
  geran_grr_band_ind_e    band_ind;

  /*! TRUE if the cell_id is known, FALSE if not. */
  boolean                 cell_id_valid;

  /*! Cell id of the cell for which measurement information is provided. */
  geran_grr_gsm_cell_id_s cell_id;

  /* RSSI, range -111 to -48 */
  int16                   rssi_dBm;

} sglte_neighbor_cell_meas_t;


typedef struct
{
  /*! message header type */
  msgr_hdr_s                  msg_hdr;
  
  boolean                    service_available; /*! TRUE if GSM is in service and neigbour  cell measurements are available, FALSE if GSM is not in service. */

  uint8                      ncell_count; //No. of N cell in the list 

  sglte_neighbor_cell_meas_t n_cell_meas[GERAN_GRR_MAX_NCELL_PLUS_SCELL_INFO];    //Array to a  N cell list max can be of 6 cell   + 1 cell of serving .

} geran_grr_irat_sglte_neighbor_meas_rsp_t;

/*! @brief Message for scaling Idle DRX / skipping Idle Meas Response
*/
typedef struct
{
  /*! @brief Message router header. */
  msgr_hdr_struct_type msg_hdr;

  /*! @brief Gives execution status of GERAN_GRR_MTC_CFG_REQ. */
  errno_enum_type      status;
} geran_grr_mtc_cfg_rsp_t;


typedef union
{
  /*! @brief Message header */
  msgr_hdr_struct_type                     hdr;
#ifdef FEATURE_LTE
  geran_grr_lte_cs_capabilities_rsp_t      lte_cs_capabilities_rsp;
  geran_grr_lte_ps_capabilities_rsp_t      lte_ps_capabilities_rsp;
  geran_grr_lte_resel_failed_rsp_t         lte_resel_failed_rsp;
  geran_grr_lte_redir_failed_rsp_t         lte_redir_failed_rsp;
  geran_grr_lte_cco_rsp_t                  lte_cco_rsp;
  lte_irat_plmn_srch_rsp_s                 plmn_srch_rsp;
  lte_irat_abort_plmn_srch_rsp_s           abort_plmn_srch_rsp;
  geran_grr_lte_dedicated_priorities_rsp_t lte_dedicated_priorities_rsp;
  geran_grr_lte_get_cgi_rsp_t              lte_get_cgi_rsp;
  geran_grr_lte_abort_cgi_rsp_t            lte_abort_cgi_rsp;
  geran_grr_lte_ho_failed_rsp_t            lte_ho_failed_rsp;
#endif /* FEATURE_LTE */
  geran_grr_irat_sglte_serving_meas_rsp_t  sglte_serving_meas_rsp;
  geran_grr_irat_sglte_neighbor_meas_rsp_t sglte_neighbor_meas_rsp;
  geran_grr_mtc_cfg_rsp_t                  mtc_cfg_rsp;
} geran_grr_msg_rsp_u;
/*

Indications sent to GERAN GRR.

*/

enum
{
  /*! @brief The first message in the enum */
  GERAN_GRR_IND_FIRST = MSGR_UMID_BASE(MSGR_GERAN_GRR, MSGR_TYPE_IND),

#ifdef FEATURE_LTE
  MSGR_DEFINE_UMID(GERAN, GRR, IND, CLEAR_DEDICATED_PRIORITIES, 0x01, none),
  MSGR_DEFINE_UMID(GERAN, GRR, IND, LTE_ABORT_CCO_FAILURE_PROCEDURE, 0x02, none),
  MSGR_DEFINE_UMID(GERAN, GRR, IND, LTE_SUSPEND_PLMN_SRCH, 0x03, lte_irat_suspend_plmn_srch_ind_s), //tlx merged cr401605
#endif /* FEATURE_LTE */
  MSGR_DEFINE_UMID(GERAN, GRR, IND, UPDATE_FORBIDDEN_BANDS, 0x04, geran_grr_update_forbidden_bands_t),

  /*! @brief The last message in the enum */
  GERAN_GRR_IND_LAST = GERAN_GRR_UPDATE_FORBIDDEN_BANDS_IND
};


enum
{
#ifdef FEATURE_LTE
  MSGR_DEFINE_UMID(GERAN, GRR, IND, LTE_GERAN_FREQ_INFO, 0x05, geran_grr_lte_geran_freq_info_ind_t),
  MSGR_DEFINE_UMID(GERAN, GRR, IND, LTE_GERAN_ACTIVITY,  0x06, geran_grr_lte_geran_activity_ind_t),
#endif /* FEATURE_LTE */
  MSGR_DEFINE_UMID(GERAN, GRR, IND, QMI_CELL_INFO, 0x07, geran_grr_qmi_cell_info_ind_t)
};

#ifdef FEATURE_LTE
/*!@brief                        
  This structure contains the message type for GERAN_GRR_LTE_GERAN_FREQ_INFO_IND, sent by
  LTE RRC to GRR for GERAN neighbour cell info received by LTE in SIB's.
*/
typedef struct
{
  /*! @brief Indicates arfcn of the GSM cell*/
  uint16                arfcn;

  /*! @brief Indicates whether ARFCNs in the overlap region of 1800 and 1900
  bands belong to 1800 or 1900. */
  geran_grr_band_ind_e  band_ind;

} geran_grr_lte_freq_type;

typedef struct
{
  /*! @brief Indicates number of frequencies provided */
  uint8                       num_of_freq;

  /*! @brief Indicates frequency list */
  geran_grr_lte_freq_type   freq_list[GERAN_GRR_LTE_GERAN_FREQ_IND_MAX_FREQ];

} geran_grr_lte_geran_freq_info_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  /*! @brief Current PLMN ID LTE is camped on*/
  sys_plmn_id_s_type plmn_id;

  /*! High speed indicator */
  boolean is_hst_cell; 

  /*! freq_info_present */
  boolean freq_info_present;
  
  /*! @brief GERAN frequency info*/
  geran_grr_lte_geran_freq_info_type freq_info;
} geran_grr_lte_geran_freq_info_ind_t;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  /*! @brief TRUE if EMBMS is active */
  boolean embms_active;

  /*! @brief TRUE if CDRX is active */
  boolean cdrx_active;

} geran_grr_lte_geran_activity_ind_t;
#endif /* FEATURE_LTE */

typedef struct 
{
  uint16                      arfcn;
  uint8                       bsic;
  sys_plmn_id_s_type          plmn_id;
  uint16                      cell_id;
  uint32                      location_area_code;
} geran_grr_gcell_identifier_t;

typedef struct 
{
  msgr_hdr_struct_type            hdr;
  geran_grr_gcell_identifier_t    uid_camped_cell;
  boolean                         si_mask_valid;
  uint16                          broadcast_si_mask;
  uint16                          si1_cell_allocation[CA_LEN];
  uint8                           si1_ca_length;
  uint8                           ra_color;
  uint8                           reselection_offset;
  uint8                           t3212;
  uint8                           rx_lev_min;
  uint8                           si13_position;
  uint8                           MSCR;
  boolean                         mandatory_si_status;
} geran_grr_qmi_cell_info_rsp_t;

typedef geran_grr_qmi_cell_info_rsp_t geran_grr_qmi_cell_info_ind_t;
typedef union
{
  /*! @brief Message header */
  msgr_hdr_struct_type                     hdr;
#ifdef FEATURE_LTE
  lte_irat_suspend_plmn_srch_ind_s         suspend_plmn_srch_ind;
  geran_grr_lte_geran_freq_info_ind_t      lte_geran_freq_info;
  geran_grr_lte_geran_activity_ind_t       lte_geran_activity_ind;
#endif /* FEATURE_LTE */
} geran_grr_msg_ind_u;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

#endif /* GERAN_GRR_MSG_H */

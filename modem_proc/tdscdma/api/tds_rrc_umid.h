#ifndef TDS_RRC_UMID_H
#define TDS_RRC_UMID_H


/*===========================================================================
                      RRC EXT UMIDs and Interface Structures

DESCRIPTION

  This module contains the UMIDs and Interface Structures for
  TDSCDMA RRC procedures

  !!!SHOULD NOT INCLUDE ANY RRC HEADER HERE!!!


Copyright (c) 2000-2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tds_rrc_umid.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#include "tds_msgr.h"


/*
  Requests that TDSCDMA RRC receives*/
/*
  >LTOTD reselection request                     TDSCDMA_RRC_LTE_RESEL_REQ
  >LTOTD reselection abort req                   TDSCDMA_RRC_LTE_ABORT_RESEL_REQ
  >LTOTD redirection request                     TDSCDMA_RRC_LTE_REDIR_REQ
  >LTOTD redirection abort req                   TDSCDMA_RRC_LTE_ABORT_REDIR_REQ
  >LTOTD PLMN search request                     TDSCDMA_RRC_LTE_PLMN_SRCH_REQ
  >LTOTD PLMN search abort req                   TDSCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ
  >TDSCDMA capabilities req                      TDSCDMA_RRC_LTE_UTRA_CAPABILITIES_REQ
  >TDSCDMA Get dedicated prriorities req         TDSCDMA_RRC_LTE_GET_DEDICATED_PRI_REQ
  >LTOTD PSHO request                            TDSCDMA_RRC_LTE_PSHO_REQ
  >LTOTD PSHO abort req                          TDSCDMA_RRC_LTE_ABORT_PSHO_REQ
  >LTOTD Get CGI req                             TDSCDMA_RRC_LTE_GET_CGI_REQ
  >LTOTD Abort CGI req                           TDSCDMA_RRC_LTE_ABORT_CGI_REQ
  >GPStoTD: Cell position info req               TDSCDMA_RRC_GPS_POS_CELL_INFO_REQ
  >QMItoTD: Get TDS config req                   TDSCDMA_RRC_GET_CONFIG_REQ
  >QMItoTD: Set TDS config req                   TDSCDMA_RRC_SET_CONFIG_REQ
  >LTOTD Depri req                               TDSCDMA_RRC_LTE_DEPRI_FREQ_REQ
*/
enum
{
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_RESEL,0x01,tds_rrc_lte_resel_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_ABORT_RESEL,0x02,tds_rrc_lte_abort_resel_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_REDIR,0x03,tds_rrc_lte_redir_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_ABORT_REDIR,0x04,tds_rrc_lte_abort_redir_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_PLMN_SRCH,0x05,lte_irat_plmn_srch_req_s),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_ABORT_PLMN_SRCH,0x06,lte_irat_abort_plmn_srch_req_s),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_UTRA_CAPABILITIES,0x07,tds_rrc_lte_utra_capabilities_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_GET_DEDICATED_PRI,0x08,tds_rrc_lte_get_dedicated_pri_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_PSHO,0x09,tds_rrc_lte_psho_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_ABORT_PSHO,0x0A,tds_rrc_lte_abort_psho_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_GET_CGI,0x0B,tds_rrc_lte_get_cgi_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_ABORT_CGI,0x0C,tds_rrc_lte_abort_cgi_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,GPS_POS_CELL_INFO,0x0D,tds_rrc_gps_pos_cell_info_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,GET_CONFIG,0x0E,tds_rrc_get_config_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,SET_CONFIG,0x0F,tds_rrc_set_config_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_GET_PLMN_PRTL_RESULTS,0x10,tds_rrc_lte_get_plmn_prtl_results_req_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,REQ,LTE_DEPRI_FREQ,0x11,tds_rrc_lte_depri_freq_req_type),
};



/*
  Requests that TDSCDMA RRC sends
*/
  
/*
  >LTOTD reselection confirmation                TDSCDMA_RRC_LTE_RESEL_FAILED_RSP
  >LTOTD reslection abort confirmation           TDSCDMA_RRC_LTE_ABORT_RESEL_RSP
  >LTOTD redirection confirmation                TDSCDMA_RRC_LTE_REDIR_FAILED_RSP
  >LTOTD redirection abort confirmation          TDSCDMA_RRC_LTE_ABORT_REDIR_RSP
  >LTOTD PLMN search confirmation                TDSCDMA_RRC_LTE_PLMN_SRCH_RSP
  >LTOTD PLMN search abort confirmation          TDSCDMA_RRC_LTE_ABORT_PLMN_SRCH_RSP
  >TDSCDMA Capabilities response                 TDSCDMA_RRC_LTE_UTRA_CAPABILITIES_RSP
  >TDSCDMA Get dedicated prriorities response    TDSCDMA_RRC_LTE_GET_DEDICATED_PRI_RSP
  >LTOTD PSHO confirmation                       TDSCDMA_RRC_LTE_PSHO_RSP
  >LTOTD PSHO abort confirmation                 TDSCDMA_RRC_LTE_ABORT_PSHO_RSP
  >LTOTD Get CGI response                        TDSCDMA_RRC_LTE_GET_CGI_RSP
  >LTOTD Abort CGI response                      TDSCDMA_RRC_LTE_ABORT_CGI_RSP
  >TDtoGPS: Cell position info response          TDSCDMA_RRC_GPS_POS_CELL_INFO_RSP
  >TDtoQMI: Get TDS config response              TDSCDMA_RRC_GET_CONFIG_RSP
  >TDtoQMI: Set TDS config response              TDSCDMA_RRC_SET_CONFIG_RSP
*/


enum
{
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_RESEL_FAILED,0x01,tds_rrc_lte_resel_failed_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_ABORT_RESEL,0x02,tds_rrc_lte_abort_resel_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_REDIR_FAILED,0x03,tds_rrc_lte_redir_failed_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_ABORT_REDIR,0x04,tds_rrc_lte_abort_redir_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_PLMN_SRCH,0x05,lte_irat_plmn_srch_rsp_s),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_ABORT_PLMN_SRCH,0x06,lte_irat_abort_plmn_srch_rsp_s),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_UTRA_CAPABILITIES,0x07,tds_rrc_lte_utra_capabilities_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_GET_DEDICATED_PRI,0x08,tds_rrc_lte_get_dedicated_pri_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_PSHO,0x09,tds_rrc_lte_psho_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_ABORT_PSHO,0x0A,tds_rrc_lte_abort_psho_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_GET_CGI,0x0B,tds_rrc_lte_get_cgi_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,LTE_ABORT_CGI,0x0C,tds_rrc_lte_abort_cgi_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,GPS_POS_CELL_INFO,0x0D,tds_rrc_gps_pos_cell_info_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,GET_CONFIG,0x0E,tds_rrc_get_config_rsp_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,RSP,SET_CONFIG,0x0F,tds_rrc_set_config_rsp_type),
};


/*
  Indications that TDSCDMA RRC received
*/
/*
  >TDSCDMA clear dedicated priorities indication TDSCDMA_RRC_LTE_CLEAR_DEDICATED_PRI_IND
  >Serving Cell Signal Status info response      TDSCDMA_RRC_SCELL_SIGNAL_STATUS_IND
  >L2TD PLMN search suspend ind                  TDSCDMA_RRC_LTE_PLMN_SRCH_SUSPEND_IND
  >TDS UARFCN on which L2T IRAT is successful    TDSCDMA_RRC_LTE_IRAT_INFO_IND
*/
enum
{
  MSGR_DEFINE_UMID(TDSCDMA,RRC,IND,LTE_CLEAR_DEDICATED_PRI,0x01,tds_rrc_lte_clear_dedicated_pri_ind_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,IND,SCELL_SIGNAL_STATUS,0x02,tds_rrc_scell_signal_status_ind_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,IND,LTE_PLMN_SRCH_SUSPEND,0x03,tds_rrc_lte_plmn_srch_suspend_ind_type),
  MSGR_DEFINE_UMID(TDSCDMA,RRC,IND,LTE_IRAT_INFO,0x04,tds_rrc_lte_irat_info_ind_type),
};


#endif /*TDS_RRC_UMID_H*/


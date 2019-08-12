#ifndef WCDMA_RRC_MSG_H
#define WCDMA_RRC_MSG_H


/*===========================================================================
                      RRC  UMIDs and Interface Structures

DESCRIPTION

  This module contains the UMIDs and Interfaces Structures for InteRAT->WCDMA
  procedures



Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/wcdma_rrc_msg.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/16   sn      Changes for LTE U feature
10/07/15   vi      Changes for force DRX and skip idlemode measurements based on QMI request 
04/25/14   vi      Made changes to handle LTE deprioritization request in W-RRC
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/14/14   bc      Made changes to send partial plmn results to NAS during manual plmn search
09/13/13   sn      Changes to include ECNO in CGI response to LTE
07/08/13   sn      Changes for LTE CSG feature
06/19/13   sn      Changes to include CSG ID and RSCP in CGI response
05/31/13   vi      Added code for CS/PS capbability check before processing L->W PSHO
04/24/13   sn      WRRC side of changes to meet CSG ADAPT TCs
10/11/12   sn      CSG feature changes
10/30/12   gv      Added code to support suspend and resume of BPLMN searches from LTE to WCDMA
07/06/12   sn      Changes to filter out bands properly 
08/06/12   gv      Added support to maintain W timing while performing L2W Cell reselection
03/09/12   as      Added changes to send start values only if l2w ps HO is enabled
08/12/11   sks     Made changes to use DSM pointer to transfer HO container from LRRC to WRRC.
06/28/11   sks     Made changes to use the timing info from LRRC for ACQ during LTOW PSHO.
03/21/11   vg      added W<->L interface change for fast re-direction
03/21/11   rl      Added code to support L2W CGI 
02/02/11   mm      Added definitions for CGI requests from LTE
11/11/10   sks     Removed featurization in the interface file.
11/09/10   sks     Fixed compilation error.
11/09/10   sks     Added interface changes to support LTOW PS handover.
09/07/10   rm      Updated the LTOW redirection structure to support CSFB
08/10/10   rm      Fixed compilation errors
08/10/10   rm      Added interface changes for transfer of dedicated priorities
05/03/10   rm      Added lte_irat_capabilities_s to wcdma_rrc_lte_utra_capabilities_req_type
04/20/10   as      Removed lte_irat_capabilities_s from wcdma_rrc_lte_utra_capabilities_req_type
04/19/10   rm      Added lte_irat_capabilities_s to wcdma_rrc_lte_utra_capabilities_req_type
03/13/10   rm      Modifed WCDMA_MAX_CAP_CONTAINER_LENGTH to 256 bytes
03/02/10   rm      Defined UMIDs,structures for WCDMA capabilities
02/11/09   rm      Added UMIDs for LTOW BPLMN search. Also corrected the dedicated priority
                   info structures.
02/08/10   np      Include new common header lte_irat_types.h and common sys file
01/31/10   rm      Modified PLMN_id_T to sys_plmn_id_s_type
01/28/10   rm      Added UMIDs, interface structures for LTE to WCDMA reselection
                   and redirection
===========================================================================*/

#include "msgr.h"
#include <sys_plmn_selection.h>
#include "wcdma_msgr.h"

#include <msgr_attach.h>

/* LTE header file for common definitions (dedicated priorities, bplmn structs etc. ) */
#include <lte_irat_types.h>

#include "rrc_rr_types.h"


/*
  Requests that WCDMA RRC receives*/
/*
  >LTOW reselection request                        WCDMA_RRC_LTE_RESEL_REQ
  >LTOW reselection abort req                      WCDMA_RRC_LTE_ABORT_RESEL_REQ
  >LTOW redirection request                         WCDMA_RRC_LTE_REDIR_REQ
  >LTOW redirection abort req                      WCDMA_RRC_LTE_ABORT_REDIR_REQ
  >LTOW PLMN search request                      WCDMA_RRC_LTE_PLMN_SRCH_REQ
  >LTOW PLMN search abort req                    WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ
  >WCDMA capabilities req                            WCDMA_RRC_LTE_UTRA_CAPABILITIES_REQ
  >WCDMA Get dedicated prriorities req   WCDMA_RRC_LTE_GET_DEDICATED_PRI_REQ
  >LTOW PSHO request                                WCDMA_RRC_LTE_PSHO_REQ
  >LTOW PSHO abort req                              WCDMA_RRC_LTE_ABORT_PSHO_REQ
  >LTOW Get CGI req                                 WCDMA_RRC_LTE_GET_CGI_REQ
  >LTOW Abort CGI req                               WCDMA_RRC_LTE_ABORT_CGI_REQ
  >LTOW DEPRI FREQ Request                       WCDMA_RRC_LTE_DEPRI_FREQ_REQ
*/
enum
{
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_RESEL,0x01,wcdma_rrc_lte_resel_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_ABORT_RESEL,0x02,wcdma_rrc_lte_abort_resel_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_REDIR,0x03,wcdma_rrc_lte_redir_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_ABORT_REDIR,0x04,wcdma_rrc_lte_abort_redir_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_PLMN_SRCH,0x05,lte_irat_plmn_srch_req_s),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_ABORT_PLMN_SRCH,0x06,lte_irat_abort_plmn_srch_req_s),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_UTRA_CAPABILITIES,0x07,wcdma_rrc_lte_utra_capabilities_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_GET_DEDICATED_PRI,0x08,wcdma_rrc_lte_get_dedicated_pri_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_PSHO,0x09,wcdma_rrc_lte_psho_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_ABORT_PSHO,0x0A,wcdma_rrc_lte_abort_psho_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_GET_CGI,0x0B,wcdma_rrc_lte_get_cgi_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_ABORT_CGI,0x0C,wcdma_rrc_lte_abort_cgi_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_GET_PLMN_PRTL_RESULTS,0x0D,wcdma_rrc_lte_get_plmn_prtl_results_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,LTE_DEPRI_FREQ,0x0E,wcdma_rrc_lte_depri_freq_req_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,REQ,QMI_MTC_CFG,0x0F,wcdma_rrc_qmi_mtc_cfg_req_type),
};
/* Below #defines are added for logging purpose only */
#define wcdma_lte_event_umid_value_0xb010201 WCDMA_RRC_LTE_RESEL_REQ
#define wcdma_lte_event_umid_value_0xb010202 WCDMA_RRC_LTE_ABORT_RESEL_REQ 
#define wcdma_lte_event_umid_value_0xb010203 WCDMA_RRC_LTE_REDIR_REQ
#define wcdma_lte_event_umid_value_0xb010204 WCDMA_RRC_LTE_ABORT_REDIR_REQ
#define wcdma_lte_event_umid_value_0xb010205 WCDMA_RRC_LTE_PLMN_SRCH_REQ
#define wcdma_lte_event_umid_value_0xb010206 WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ
#define wcdma_lte_event_umid_value_0xb010207 WCDMA_RRC_LTE_UTRA_CAPABILITIES_REQ
#define wcdma_lte_event_umid_value_0xb010208 WCDMA_RRC_LTE_GET_DEDICATED_PRI_REQ
#define wcdma_lte_event_umid_value_0xb010209 WCDMA_RRC_LTE_PSHO_REQ
#define wcdma_lte_event_umid_value_0xb01020a WCDMA_RRC_LTE_ABORT_PSHO_REQ
#define wcdma_lte_event_umid_value_0xb01020b WCDMA_RRC_LTE_GET_CGI_REQ
#define wcdma_lte_event_umid_value_0xb01020c WCDMA_RRC_LTE_ABORT_CGI_REQ


/*
  Responses that WCDMA RRC sends
*/
  
/*
  >LTOW reselection confirmation                  WCDMA_RRC_LTE_RESEL_FAILED_RSP
  >LTOW reslection abort confirmation            WCDMA_RRC_LTE_ABORT_RESEL_RSP
  >LTOW redirection confirmation                   WCDMA_RRC_LTE_REDIR_FAILED_RSP
  >LTOW redirection abort confirmation           WCDMA_RRC_LTE_ABORT_REDIR_RSP
  >LTOW PLMN search confirmation                WCDMA_RRC_LTE_PLMN_SRCH_RSP
  >LTOW PLMN search abort confirmation        WCDMA_RRC_LTE_ABORT_PLMN_SRCH_RSP
  >WCDMA Capabilities response                    WCDMA_RRC_LTE_UTRA_CAPABILITIES_RSP
  >WCDMA Get dedicated prriorities response   WCDMA_RRC_LTE_GET_DEDICATED_PRI_RSP
  >LTOW PSHO confirmation                          WCDMA_RRC_LTE_PSHO_RSP
  >LTOW PSHO abort confirmation                  WCDMA_RRC_LTE_ABORT_PSHO_RSP
  >LTOW Get CGI response                        WCDMA_RRC_LTE_GET_CGI_RSP
  >LTOW Abort CGI response                      WCDMA_RRC_LTE_ABORT_CGI_RSP
*/


enum
{
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_RESEL_FAILED,0x01,wcdma_rrc_lte_resel_failed_rsp_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_ABORT_RESEL,0x02,wcdma_rrc_lte_abort_resel_rsp_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_REDIR_FAILED,0x03,wcdma_rrc_lte_redir_failed_rsp_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_ABORT_REDIR,0x04,wcdma_rrc_lte_abort_redir_rsp_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_PLMN_SRCH,0x05,lte_irat_plmn_srch_rsp_s),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_ABORT_PLMN_SRCH,0x06,lte_irat_abort_plmn_srch_rsp_s),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_UTRA_CAPABILITIES,0x07,wcdma_rrc_lte_utra_capabilities_rsp_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_GET_DEDICATED_PRI,0x08,wcdma_rrc_lte_get_dedicated_pri_rsp_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_PSHO,0x09,wcdma_rrc_lte_psho_rsp_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_ABORT_PSHO,0x0A,wcdma_rrc_lte_abort_psho_rsp_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_GET_CGI,0x0B,wcdma_rrc_lte_get_cgi_rsp_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_ABORT_CGI,0x0C,wcdma_rrc_lte_abort_cgi_rsp_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,LTE_RESEL,0x0D,lte_rrc_wcdma_resel_rsp_s),
  MSGR_DEFINE_UMID(WCDMA,RRC,RSP,QMI_MTC_CFG,0x0E,wcdma_rrc_qmi_mtc_cfg_cnf_type),
};
/* Below #defines are added for logging purpose only */
#define wcdma_lte_event_umid_value_0xb010301 WCDMA_RRC_LTE_RESEL_FAILED_RSP
#define wcdma_lte_event_umid_value_0xb010302 WCDMA_RRC_LTE_ABORT_RESEL_RSP
#define wcdma_lte_event_umid_value_0xb010303 WCDMA_RRC_LTE_REDIR_FAILED_RSP
#define wcdma_lte_event_umid_value_0xb010304 WCDMA_RRC_LTE_ABORT_REDIR_RSP
#define wcdma_lte_event_umid_value_0xb010305 WCDMA_RRC_LTE_PLMN_SRCH_RSP
#define wcdma_lte_event_umid_value_0xb010306 WCDMA_RRC_LTE_ABORT_PLMN_SRCH_RSP
#define wcdma_lte_event_umid_value_0xb010307 WCDMA_RRC_LTE_UTRA_CAPABILITIES_RSP
#define wcdma_lte_event_umid_value_0xb010308 WCDMA_RRC_LTE_GET_DEDICATED_PRI_RSP
#define wcdma_lte_event_umid_value_0xb010309 WCDMA_RRC_LTE_PSHO_RSP
#define wcdma_lte_event_umid_value_0xb01030a WCDMA_RRC_LTE_ABORT_PSHO_RSP
#define wcdma_lte_event_umid_value_0xb01030b WCDMA_RRC_LTE_GET_CGI_RSP
#define wcdma_lte_event_umid_value_0xb01030c WCDMA_RRC_LTE_ABORT_CGI_RSP
#define wcdma_lte_event_umid_value_0xb01030d WCDMA_RRC_LTE_RESEL_RSP

/*
  Indications that WCDMA RRC received
*/
/*
  >WCDMA clear dedicated priorities indication
  >LTOW. PLMN search suspend ind              WCDMA_RRC_LTE_PLMN_SRCH_SUSPEND_IND
*/
enum
{
  MSGR_DEFINE_UMID(WCDMA,RRC,IND,LTE_CLEAR_DEDICATED_PRI,0x01,wcdma_rrc_lte_clear_dedicated_pri_ind_type),
  MSGR_DEFINE_UMID(WCDMA,RRC,IND,LTE_PLMN_SRCH_SUSPEND,0x02,wcdma_rrc_lte_plmn_srch_suspend_ind_type),
};
/* Below #defines are added for logging purpose only */
#define wcdma_lte_event_umid_value_0xb010401 WCDMA_RRC_LTE_CLEAR_DEDICATED_PRI_IND
#define wcdma_lte_event_umid_value_0xb010402 WCDMA_RRC_LTE_PLMN_SRCH_SUSPEND_IND


/* for logging purpose only. In the below # defines right hand side is coming from modem_proc\lte\api\lte_rrc_irat_msg.h */
#define wcdma_lte_event_umid_value_0x40d0280 LTE_RRC_WCDMA_RESEL_REQ  
#define wcdma_lte_event_umid_value_0x40d0281 LTE_RRC_WCDMA_REDIR_REQ  
#define wcdma_lte_event_umid_value_0x40d0283 LTE_RRC_WCDMA_ABORT_REDIR_REQ  
#define wcdma_lte_event_umid_value_0x40d0288 LTE_RRC_EUTRA_CAPABILITIES_REQ  
#define wcdma_lte_event_umid_value_0x40d0289 LTE_RRC_WCDMA_PLMN_SRCH_REQ  
#define wcdma_lte_event_umid_value_0x40d028a LTE_RRC_WCDMA_ABORT_PLMN_SRCH_REQ  
#define wcdma_lte_event_umid_value_0x40d028d LTE_RRC_GET_DEDICATED_PRI_REQ  
#define wcdma_lte_event_umid_value_0x40d0292 LTE_RRC_WCDMA_PSHO_REQ  
#define wcdma_lte_event_umid_value_0x40d0293 LTE_RRC_WCDMA_ABORT_PSHO_REQ  

#define wcdma_lte_event_umid_value_0x40d0381 LTE_RRC_WCDMA_RESEL_FAILED_RSP  
#define wcdma_lte_event_umid_value_0x40d0382 LTE_RRC_WCDMA_REDIR_FAILED_RSP  
#define wcdma_lte_event_umid_value_0x40d0383 LTE_RRC_WCDMA_ABORT_RESEL_RSP  
#define wcdma_lte_event_umid_value_0x40d0384 LTE_RRC_WCDMA_ABORT_REDIR_RSP  
#define wcdma_lte_event_umid_value_0x40d0389 LTE_RRC_EUTRA_CAPABILITIES_RSP  
#define wcdma_lte_event_umid_value_0x40d038a LTE_RRC_WCDMA_PLMN_SRCH_RSP  
#define wcdma_lte_event_umid_value_0x40d038b LTE_RRC_WCDMA_ABORT_PLMN_SRCH_RSP  
#define wcdma_lte_event_umid_value_0x40d038e LTE_RRC_GET_DEDICATED_PRI_RSP  
#define wcdma_lte_event_umid_value_0x40d0394 LTE_RRC_WCDMA_PSHO_RSP  
#define wcdma_lte_event_umid_value_0x40d0395 LTE_RRC_WCDMA_ABORT_PSHO_RSP  
#define wcdma_lte_event_umid_value_0x40d039e LTE_RRC_WCDMA_RESEL_RSP  


#define wcdma_lte_event_umid_value_0x40d0487 LTE_RRC_WCDMA_SUSPEND_PLMN_SRCH_IND


/*========================================================
            LTOW InterRAT reselection structures
=========================================================*/

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

   /* The Frequency of the Target UTRAN Cell */
   uint16                               UARFCN;

   /* The Scrambling code of the target UTRAN Cell */
   uint16                               scrambling_code;

   /* Indicates how a PLMN is to be selected. */
   sys_network_selection_mode_e_type network_select_mode;

   /* PLMNId of the requested PLMN */
   sys_plmn_id_s_type                    requested_PLMN;

  /*RAT capability, should pass the same structure that NAS uses*/
  sys_rat_pri_list_info_s_type    rat_pri_list_info;

  /*dedicated priority info*/
  lte_irat_dedicated_priority_info_s dedicated_priority_info;

  /* Inter-RAT measurement data */
  diversity_T   diversity;
  uint16 PN_Offset; /* 0-38399 (valid), 65535 (not-known) */

  /* LTE passes its its current camped information. This will be used
     for CSG cell finger printing */
  source_cell_info_type serving_cell_info;

  /* Set to TRUE is resel is a result of ASF*/
  boolean csg_resel;

  
  /*Nas requested CSG ID*/
  uint32 nas_csg_id;
} wcdma_rrc_lte_resel_req_type;

/*LTE to WCDMA IRAT failure cause*/
typedef enum
{
  WCDMA_RRC_NONE,
  WCDMA_RRC_ACQUISITION_FAIL,            /*Acquisition Failure*/
  WCDMA_RRC_SIBS_FAILURE,                        /*SIBs failure, either S-BCCH setup failure / SIBs timeout etc*/
  WCDMA_RRC_CELL_BARRED,                    /*Cell Barred*/
  WCDMA_RRC_UARFCN_BARRED,               /*Frequency Barred*/
  WCDMA_RRC_FORBIDDEN_LA,                  /*Forbidden LA*/
  WCDMA_RRC_PLMN_MISMATCH,               /*PLMN mismatch*/
  WCDMA_RRC_CELL_QUALITY_FAILURE,     /*S-Criteria Failure*/
  WCDMA_RRC_CELL_NOT_SUITABLE,               /*Access Not Allowed*/
  WCDMA_RRC_CELL_CHANGE_FAILURE,           /*Transition to the cell failure or Paging channel setup failure*/
  WCDMA_RRC_CSG_FORBIDDEN,                 /*CSG is in non-white*/
  WCDMA_RRC_MAX_FAILURE
} wcdma_rrc_irat_failure_cause_e_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  /*Failure cause*/
  wcdma_rrc_irat_failure_cause_e_type failure_cause;

  /* Time ( in Sec ) when the same cell should be considered */
  uint32                         time_to_reselect;
}wcdma_rrc_lte_resel_failed_rsp_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} wcdma_rrc_lte_abort_resel_req_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} wcdma_rrc_lte_abort_resel_rsp_type;



/*========================================================
              LTOW InterRAT redirection structures
=========================================================*/

#define WCDMA_RRC_LTE_MAX_UTRAN_FDD_CELLS_PER_FREQ    32
#define WCDMA_RRC_LTE_MAX_UTRAN_FDD_FREQ    8

#define WCDMA_RRC_MAX_UTRA_CONT 16

#define WCDMA_RRC_MAX_SHARED_PLMN 5

#define WCDMA_RRC_MAX_LTE_DEPRI_FREQ LTE_IRAT_MAX_EUTRAN_FREQ /*! \brief MAX number of EUTRA freq deprioritized */


typedef struct
{
  uint32    uarfcn;
  uint8      num_of_cells;
  uint32    scrambling_code[WCDMA_RRC_LTE_MAX_UTRAN_FDD_CELLS_PER_FREQ];
} wcdma_rrc_lte_fdd_cell_list_type;

typedef struct
{
  uint8    num_of_frequencies;
  wcdma_rrc_lte_fdd_cell_list_type    fdd_cell_list[WCDMA_RRC_LTE_MAX_UTRAN_FDD_FREQ];
} wcdma_rrc_lte_fdd_target_info_type;


typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  /*Message router Attachments*/
  msgr_attach_s msgr_attach[WCDMA_RRC_MAX_UTRA_CONT];

  /*Number of PCI on redirected frequency for which the N/W has sent SI container*/
  uint8 num_pci;

  /*PCI list on redirected frequency, PCI is PSC on WCDMA*/
  uint16 pci_list[WCDMA_RRC_MAX_UTRA_CONT];

  /* PLMN of the WCDMA cell */
  sys_plmn_id_s_type    PLMN;

  /* Indicates how a PLMN is to be selected */
  sys_network_selection_mode_e_type    network_select_mode;

  /* list of WCDMA cells or frequencies */
  wcdma_rrc_lte_fdd_target_info_type    fdd_target_info;

  /*RAT capability, should pass the same structure that NAS uses*/
  sys_rat_pri_list_info_s_type    rat_pri_list_info;

  /*dedicated priority info*/
  lte_irat_dedicated_priority_info_s priority_info;

  /*Fast redirection */
  boolean fast_redirection;

  source_cell_info_type serving_cell_info;

  /*Nas requested CSG ID*/
  uint32 nas_csg_id;

} wcdma_rrc_lte_redir_req_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} wcdma_rrc_lte_redir_failed_rsp_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
}wcdma_rrc_lte_abort_redir_req_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} wcdma_rrc_lte_abort_redir_rsp_type;


/*========================================================
              WCDMA Capability Stuctures
=========================================================*/
#define WCDMA_MAX_CAP_CONTAINER_LENGTH 256

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr;

  sys_band_mask_type             wcdma_band_preference;
  sys_lte_band_mask_e_type       lte_band_preference;
  sys_band_mask_type             gsm_band_preference;

  lte_irat_capabilities_s        rat_capabilities;
  boolean                        compressed_mode_required;
  boolean                        l2w_psho_enabled;
  sys_plmn_id_s_type             camped_plmn;

}wcdma_rrc_lte_utra_capabilities_req_type;


typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr;

  uint32     capabilities_container_length;
  uint8      capabilities_container[WCDMA_MAX_CAP_CONTAINER_LENGTH];

}wcdma_rrc_lte_utra_capabilities_rsp_type;

/*========================================================
              WCDMA to LTE dedicated priority Stuctures
=========================================================*/

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} wcdma_rrc_lte_get_dedicated_pri_req_type;

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
} wcdma_rrc_lte_get_dedicated_pri_rsp_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} wcdma_rrc_lte_clear_dedicated_pri_ind_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} wcdma_rrc_lte_plmn_srch_suspend_ind_type;

/*========================================================
              LTE to WCDMA PS Handover Stuctures
=========================================================*/

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  msgr_attach_s dsm_attach; /*!< dsm attachment
                                 containing the handOverToUTRANCommand */

  /*PS HO is supported*/
  boolean ps_ho_is_supported;

  /*CS HO is supported*/
  boolean cs_ho_is_supported;


  lte_irat_umts_sec_info_s security_keys;

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  /* PLMNId of the PLMN UE is camped on the LTE side */
  sys_plmn_id_s_type  plmn_id;

  sys_rat_pri_list_info_s_type   rat_pri_list_info;

  /* Inter-RAT measurement data */
  rr_rrc_interrat_measurement_data_type interrat_measurement_data;

} wcdma_rrc_lte_psho_req_type;

/*----------------------------------------------------------------------------
                         WCDMA_RRC_LTE_PSHO_RSP
This primitive is sent from WRRC to LRRC to respond the WCDMA_RRC_LTE_PSHO_REQ 
----------------------------------------------------------------------------*/
/* Handover status */
typedef enum
{
  LTOW_PSHO_SUCCESS,
  LTOW_PSHO_FAILURE,
  LTOW_PSHO_MAX_STATUS
} wcdma_rrc_lte_psho_status_e_type;

/* RRC Handover failure causes */
typedef enum
{
  LTOW_PSHO_INVALID_STATE,
  LTOW_PSHO_WCDMA_PROTOCOL_ERROR,
  LTOW_PSHO_INVALID_CONFIGURATION,
  LTOW_PSHO_PHYSICAL_CHANNEL_FAILURE,
  LTOW_PSHO_INVALID_CIPHER_ALGORITHM,
  LTOW_PSHO_FREQUENCY_NOT_IMPLEMENTED,
  LTOW_PSHO_MAX_FAILURE_CAUSE
} wcdma_rrc_lte_psho_failure_cause_e_type;

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 

  /* Handover status */
  wcdma_rrc_lte_psho_status_e_type handover_status;

  /* Handover failure cause: this field is only valid if handover status is
     failure */
  wcdma_rrc_lte_psho_failure_cause_e_type failure_cause;

} wcdma_rrc_lte_psho_rsp_type;

/*----------------------------------------------------------------------------
                         WCDMA_RRC_LTE_ABORT_PSHO_REQ 
This primitive is sent from LRRC to WRRC to abort WCDMA to LTE inter-RAT 
handover. 
----------------------------------------------------------------------------*/

typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} wcdma_rrc_lte_abort_psho_req_type;

/*----------------------------------------------------------------------------
                         WCDMA_RRC_LTE_ABORT_PSHO_RSP
This primitive is sent from WRRC to LRRC in response to
WCDMA_RRC_LTE_ABORT_PSHO_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  /*message header type*/
  msgr_hdr_s msg_hdr; 
} wcdma_rrc_lte_abort_psho_rsp_type;

/*----------------------------------------------------------------------------
                         WCDMA_RRC_LTE_GET_CGI_REQ
This primitive is sent from LRRC to WRRC for acquiring CGI information for 
a particular cell 
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s msg_hdr;

  /* UARFCN of the cell for which CGI info is requested */
  uint32    uarfcn;

  /* PSC of the cell for which CGI info is requested */
  uint32    scr_code;     

  /* Time in ms that W has, for acquiring CGI info; W has to send a response 
    back to LTE with a success/failure within this time */
  uint32    time_available_for_cgi;  

} wcdma_rrc_lte_get_cgi_req_type;

/*----------------------------------------------------------------------------
                         WCDMA_RRC_LTE_GET_CGI_RSP
This primitive is sent from WRRC to LRRC in response to
WCDMA_RRC_LTE_GET_CGI_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  /* Cell id for the W cell for which CGI info was requested */
  uint32 cell_id;

  /* PLMN id for the W cell for which CGI info was requested */
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
  sys_plmn_id_s_type  shared_plmn_list[WCDMA_RRC_MAX_SHARED_PLMN];

  /* CSG ID field  */
  uint32 csg_id;

  /*RSCP of the cell*/
  int16 rscp;

  /*ECNO of the cell*/
  uint8 ecno;

} wcdma_rrc_lte_cgi_info_type;

typedef struct
{
  /* message header type */
  msgr_hdr_s msg_hdr;

  /* UARFCN of the cell for which CGI info was requested */
  uint32    uarfcn;

  /* PSC of the cell for which CGI info was requested */
  uint32    scr_code;

  /* TRUE, if CGI acquisition suceeded and CGI info is included, FALSE otherwise */
  boolean cgi_info_included;     

  /* CGI information for the requested cell */
  wcdma_rrc_lte_cgi_info_type cgi_info;

} wcdma_rrc_lte_get_cgi_rsp_type;

/*----------------------------------------------------------------------------
                         WCDMA_RRC_LTE_ABORT_CGI_REQ
This primitive is sent from LRRC to WRRC for aborting CGI acquisition on W 
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s msg_hdr;  

} wcdma_rrc_lte_abort_cgi_req_type;

typedef struct
{
  /* message header type*/
  msgr_hdr_s msg_hdr;

} wcdma_rrc_lte_get_plmn_prtl_results_req_type;


/*----------------------------------------------------------------------------
                         WCDMA_RRC_LTE_ABORT_CGI_RSP
This primitive is sent from WRRC to LRRC in response to 
WCDMA_RRC_LTE_ABORT_CGI_REQ
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s msg_hdr; 

} wcdma_rrc_lte_abort_cgi_rsp_type;


/*?---------------------------------------?-------------------------------------
                         WCDMA_RRC_LTE_DEPRI_FREQ_REQ
This primitive is sent from LRRC to WRRC for passing Deprioritization
list from LTE
-----------------------------------------?-----------------------------------*/
 
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
  lte_earfcn_t depri_freqs[WCDMA_RRC_MAX_LTE_DEPRI_FREQ];
  
} wcdma_rrc_lte_depri_freq_req_type;

/*----------------------------------------------------------------------------
                         WCDMA_RRC_QMI_MTC_CFG_REQ
----------------------------------------------------------------------------*/


typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /* To be set to TRUE when scaling factor is configured */
  boolean drx_scaling_factor_valid;

  /* WCDMA scaling factor to be configured */ 
  uint8 drx_scaling_factor;

  /* To be set to TRUE when scaling factor is configured */
  boolean skip_idle_srch_meas_valid;
  
  /* To be set to TRUE when idle srch/meas have to be skipped.,
  else default FALSE must be set */
  boolean skip_idle_srch_meas;

} wcdma_rrc_qmi_mtc_cfg_req_type;


/*----------------------------------------------------------------------------
                         WCDMA_RRC_QMI_MTC_CFG_CNF
----------------------------------------------------------------------------*/

typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  
  boolean status;     /*!< SUCCESS if cfg req passed, FAILURE otherwise */
  
} wcdma_rrc_qmi_mtc_cfg_cnf_type;


#endif /*WCDMA_RRC_MSG_H*/

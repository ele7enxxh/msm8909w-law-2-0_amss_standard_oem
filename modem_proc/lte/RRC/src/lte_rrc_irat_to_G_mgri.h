/*!
  @file
  lte_rrc_irat_to_G_mgri.h

  @brief
  This module contains the entry, exit, and transition functions
  necessary to implement the following state machines:

  @detail
  LTE_RRC_G_RAT_MGR ( 1 instance/s )


*/

/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_to_G_mgri.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/11   mm      Added definitions for IRAT CGI
03/03/10   sk      Added override bar time flag
02/18/10   sk      Added support to store failure cause field
01/05/2009 sk      Initial revision
===========================================================================*/

#ifndef LTE_RRC_IRAT_TO_G_RATI_H
#define LTE_RRC_IRAT_TO_G_RATI_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

//! @todo Include necessary files here
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_irat_msg.h"
#include "lte_rrc_irat_utils.h"
#include "lte_l1_types.h"
#include "geran_grr_msg.h"


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*! @brief Enum to represent various resume causes
*/
typedef enum
{
  /*! LTE stack resumed due to procedure failure */
  LTE_RRC_TO_G_PROC_FAILED = 0,

  /*! LTE stack resumed due to procedure being aborted */
  LTE_RRC_TO_G_PROC_ABORTED = 1,

  /*! LTE stack to be resumed after a proc (e.g. PLMN Srch) completes on G */
  LTE_RRC_TO_G_PROC_COMPLETED = 2,

}lte_rrc_to_G_resume_cause_type_e;

/*! @brief Enum to represent various resume causes
*/
typedef enum
{
  /*! LTE stack deactivated due to successful completion of procedure */
  LTE_RRC_TO_G_CCO_SUCCESS = 0,

  /*! LTE stack resumed due to procedure being aborted */
  LTE_RRC_TO_G_CCO_ABORTED = 1,

}lte_rrc_to_G_deactivate_cco_cause_type_e;

/*! @brief Enum to represent various resume causes
*/
typedef enum
{
  /*! PLMN Request */
  LTE_RRC_TO_G_PLMN_REQ = 0,

   /*! PLMN Response */
  LTE_RRC_TO_G_PLMN_RSP = 1,

}lte_rrc_to_G_plmn_info_type_e;

/*! @brief Structure for reselection to G
*/
typedef struct
{
  geran_grr_band_ind_e band;    /*!< Band to which the G cell belongs */
  uint16                freq;    /*!< Freq to which the G cell belongs */
  lte_l1_gsm_cell_id_s  cell_id; /*!< Physical cell id a.k.a BSIC_ID */
  lte_l1_gsm_rssi_t     rssi;    /*! <Rssi value */
}lte_rrc_to_G_resel_s;

/*! @brief Structure for redirection to G
*/
typedef struct
{
  geran_grr_band_ind_e  band_ind;  /*!< Band indicator for overlapping ARFCN */
  uint16                freq;      /*!< ARFCN */
}lte_rrc_to_G_redir_s;

/*! @brief Structure for redirection list to G
*/
typedef struct
{
  uint8 num_freq;
  lte_rrc_to_G_redir_s redir_info[LTE_RRC_MAX_GERAN_FREQUENCIES];
  boolean               cell_info_list_is_present;  /*!< If SIBs are present */
  /*!< List of cells and their SIB Containers for the RAT */
  lte_rrc_redir_cell_info_list_s  cell_info_list;
}lte_rrc_to_G_redir_list_s;

/*! @brief Structure for redirection list to G
*/
typedef struct
{
  lte_rrc_to_G_plmn_info_type_e 		info_type; /*!<PLMN info type*/
  union
  {
	lte_irat_plmn_srch_req_s                plmn_srch_req; /*!< PLMN search Req*/

	lte_irat_plmn_srch_rsp_s                plmn_srch_rsp; /*!< PLMN search Rsp*/ 
  }plmn_info;
}lte_rrc_to_G_plmn_srch_s; 

/*! @brief Structure for IRAT CGI to G
*/
typedef struct
{
  lte_rrc_irat_from_lte_to_g_get_cgi_reqi_s   get_cgi_req;  /*!< Get CGI Req */
  lte_rrc_irat_from_lte_to_g_get_cgi_cnfi_s   get_cgi_rsp;  /*!< Get CGI Rsp */

}lte_rrc_to_G_cgi_s;

/*! @brief Structure for CCO/NACC to G
*/
typedef struct
{
  uint32                   t304_tmr_val; /*!<CCO T304 Timer Value */
  geran_grr_band_ind_e     band_ind;     /*!< Band to which the G cell belongs */
  uint16                   freq;         /*!< Freq to which the G cell belongs */
  geran_grr_gsm_cell_id_s  cell_id;      /*!< Physical cell id a.k.a BSIC_ID */
  /*!< SI */
  /* OTA-encoded GERAN SI/PSI messages available for the target cell. */
  lte_rrc_geran_sib_container_list_s   si_messages;
  /*!< NCO */
  boolean                  nw_ctrl_order_is_present;
  uint8                    nw_ctrl_order;/*!<Network Control Order */
  boolean                  proc_for_csfb; /*!<Boolean to indicate if procedure is for CSFB */
}lte_rrc_to_G_cco_s;

/*! #brief Structure for PSHO to G 
*/
typedef struct
{
  uint8             nas_count;
  dsm_item_type     *dsm_ptr;
  boolean           proc_for_csfb; /*!<Boolean to indicate if procedure is for CSFB */
  geran_grr_band_ind_e band; /*! <Band indicator */
  /*!< OTA-encoded GERAN SI/PSI messages available for the target cell. */
  lte_rrc_geran_sib_container_list_s   si_messages;
}lte_rrc_to_G_psho_s;

/*! @brief Structure for storing the inter-RAT procedure details
*/
typedef struct
{
  /*!< Procedure indicator */
  lte_rrc_irat_proc_e   proc_type;
  union
  {
    lte_rrc_to_G_resel_s to_G_resel_info;
    lte_rrc_to_G_redir_list_s to_G_redir_list;
    lte_rrc_to_G_plmn_srch_s to_G_plmn_srch_info; 
    lte_rrc_to_G_cgi_s to_G_cgi_info; 
    lte_rrc_to_G_cco_s to_G_cco_info;
    lte_rrc_to_G_psho_s to_G_psho_info;
    geran_grr_redir_info_type geran_internal_redir_info;
  }proc;
}lte_rrc_to_G_proc_s;

/*! @brief Structure for dynamic private data
*/
typedef struct
{

  /*!< Inter-RAT procedure details */
  lte_rrc_to_G_proc_s to_G_proc_info;

  /* !< Abort cause if an Abort request is received */
  lte_rrc_irat_abort_cause_type_e  abort_cause;

  /*!< Abort Proc type */
  lte_rrc_proc_e abort_proc;

  /* !< Resume cause if the LTE stack is being resumed */
  lte_rrc_to_G_resume_cause_type_e resume_cause;

  /* !< Deactivate cause if the LTE stack is being stopped and if the procedure is CCO */
  lte_rrc_to_G_deactivate_cco_cause_type_e deactivate_cause;

  /*!< Procedure failure cause - received from GSM RR */
  lte_rrc_irat_from_lte_failure_cause_type_e fail_cause;
  
  /*!< Failure phase - applicable to CCO only */
  lte_rrc_irat_cco_nacc_failure_phase_type_e failure_phase;

  /*!< Boolean to indicate if bar time needs to be 
     overrided by value provided by GSM RR  
  */
   boolean      override_bar_time; 

   /*Boolean indicating release TRM req sent for internal redir*/
   boolean      sent_release_trm_req;

 /***********************************************************************
   ADD NEW VARIABLES TO BE RESET IN lte_rrc_irat_to_g_mgr_reset_data()
   IF NEED BE
 ************************************************************************/

 /*!< Boolean to keep track if SIB-7 was sent */
  boolean                             sib7_sent_earlier;

  /*!< LTE cell for which sib7 info was sent last */
  lte_rrc_cell_info_s                 last_sib7_cell_info;

  /*!< Boolean to indicate whether High Speed Flag is present in SIB2*/
  boolean                             high_speed_flag;

}lte_rrc_irat_to_G_dynamic_data_s;

/*! @brief Structure for state-machine per-instance local variables
*/
typedef struct
{ 
  /*! Dynamic data pointer */
  lte_rrc_irat_to_G_dynamic_data_s *dd_ptr;
  
} lte_rrc_irat_to_G_s;


#endif /* LTE_RRC_IRAT_TO_G_RATI_H */



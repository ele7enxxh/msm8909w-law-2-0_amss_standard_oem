/*!
  @file
  lte_rrc_irat_to_w_mgri.h

  @brief

  @detail

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_to_w_mgri.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/11   mm      Added definitions for IRAT CGI
01/06/11   np      Added L2W PSHO abort related handling
11/10/10   np      Initial L2W PS HO Support
02/18/10   np      Integrate with real W/api Header
02/15/10   np      Support for new IRAT Abort causes
01/12/2010 np      Store W Target Cell Info for reselections
12/21/2009 np      Initial revision
===========================================================================*/

#ifndef LTE_RRC_IRAT_TO_W_MGRI_H
#define LTE_RRC_IRAT_TO_W_MGRI_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_rrc_ext_msg.h"
#include "lte_rrc_irat_utils.h"
#include "wcdma_rrc_msg.h"
#include "lte_rrc_irat_to_w_mgr.h"

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

typedef enum
{
  /*! Default value */
  LTE_RRC_IRAT_TO_W_MODE_INITIAL            = 0,

  /*! IRAT Resel to w started */
  LTE_RRC_IRAT_TO_W_MODE_RESEL_STARTED      = 1,

  /*! IRAT Resel to w failed */
  LTE_RRC_IRAT_TO_W_MODE_RESEL_FAILED       = 2,

  /*! IRAT Resel to w aborted */
  LTE_RRC_IRAT_TO_W_MODE_RESEL_ABORTED      = 3,

  /*! IRAT Redir to w Started */
  LTE_RRC_IRAT_TO_W_MODE_REDIR_STARTED      = 4,

  /*! IRAT Redir to w Failed */
  LTE_RRC_IRAT_TO_W_MODE_REDIR_FAILED       = 5,

  /*! IRAT Redir to w Aborted */
  LTE_RRC_IRAT_TO_W_MODE_REDIR_ABORTED      = 6,

  /*! IRAT PLMN Srch W started */
  LTE_RRC_IRAT_TO_W_MODE_PLMN_SRCH_STARTED  = 7,

  /*! IRAT PLMN Srch W Aborted */
  LTE_RRC_IRAT_TO_W_MODE_PLMN_SRCH_ABORTED  = 8,

  /*! IRAT PLMN Srch W started */
  LTE_RRC_IRAT_TO_W_MODE_PLMN_SRCH_INACTIVE_STARTED  = 9,

  /*! IRAT PLMN Srch W Aborted */
  LTE_RRC_IRAT_TO_W_MODE_PLMN_SRCH_INACTIVE_ABORTED  = 10,

  /*! IRAT HO to w started */
  LTE_RRC_IRAT_TO_W_MODE_HO_STARTED                  = 11,

  /*! IRAT HO to w failed */
  LTE_RRC_IRAT_TO_W_MODE_HO_FAILED                   = 12,

  /*! IRAT HO to w aborted */
  LTE_RRC_IRAT_TO_W_MODE_HO_ABORTED                  = 13,

  /*! W CGI reading started */
  LTE_RRC_IRAT_TO_W_MODE_CGI_STARTED  = 14,

  /*! W CGI reading aborted */
  LTE_RRC_IRAT_TO_W_MODE_CGI_ABORTED  = 15,

  /*! W CGI reading aborted due to CONN_ABORT*/
  LTE_RRC_IRAT_TO_W_MODE_CGI_ABORTED_CONN_ABORT  = 16,

} lte_rrc_irat_to_w_mode_e;

/*! @brief Structure for storing dynamic IRAT data
*/
typedef struct
{
  /*!< Current Inter-RAT procedure details */
  lte_rrc_irat_proc_e                 proc;

  /* !< Abort cause if an Abort request is received */
  lte_rrc_irat_abort_cause_type_e     abort_cause;

  /*!< Abort Proc type */
  lte_rrc_proc_e abort_proc;

  /* !< WCDMA Cell Info for reselections */
  lte_cphy_wcdma_cell_info_s          w_cell_info;

  /* !< CSG reselection */
  boolean                             csg_resel;

  /* !< WCDMA IRAT Redirection related info */
  lte_rrc_redir_info_s                redir_info;
  
  /* !< Resel Failed Data from WCDMA */
  wcdma_rrc_lte_resel_failed_rsp_type resel_failed_info;

  /*!< W PLMN SRCH Req*/
  lte_irat_plmn_srch_req_s            plmn_srch_req;

  /*!< W PLMN Srch Rsp*/
  lte_irat_plmn_srch_rsp_s            plmn_srch_rsp; 

  /*!< W Get CGI Req*/
  lte_rrc_irat_from_lte_to_w_get_cgi_reqi_s get_cgi_req;

  /*!< W Get CGI Rsp*/
  lte_rrc_irat_from_lte_to_w_get_cgi_cnfi_s get_cgi_rsp;

  /*!< W HO command */
  uint8                               dl_nas_count;
  dsm_item_type                       *dsm_ptr;

  /* !< Keeps track of IRAT mode */
  lte_rrc_irat_to_w_mode_e            mode;

  boolean                             csfb_call_started;
  boolean                             csfb_call_ended;

  /*!< Boolean to keep track if SIB-6 was sent */
  boolean                             sib6_sent_earlier;

  /*!< LTE cell for which sib6 info was sent last */
  lte_rrc_cell_info_s                 last_sib6_cell_info;

} lte_rrc_irat_to_w_mgr_dynamic_data_s;

/*! @brief Structure for storing IRAT data
 */
typedef struct
{ 
  /* Dynamic data pointer */
  lte_rrc_irat_to_w_mgr_dynamic_data_s    *dd_ptr;

  // No static data
  
} lte_rrc_irat_to_w_mgr_s;


#endif /* LTE_RRC_IRAT_TO_W_MGRI_H */



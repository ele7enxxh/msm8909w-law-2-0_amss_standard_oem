/*!
  @file
  lte_rrc_irat_from_w_mgri.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_from_w_mgri.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/2010 np      Add mode for redir failed after exhausting full srch
02/09/2010 np      Add redir related variables
01/21/2010 np      Initial revision
===========================================================================*/

#ifndef LTE_RRC_IRAT_FROM_W_MGRI_H
#define LTE_RRC_IRAT_FROM_W_MGRI_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_variation.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_irat_utils.h"
#include "lte_rrc_irat_from_w_mgr.h"

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*! @brief Enum to represent various things to do on deactivation.
 *
*/
typedef enum
{
  /*! Default value */
  LTE_RRC_IRAT_FROM_W_MODE_INITIAL,

  /*! IRAT Resel to LTE started */
  LTE_RRC_IRAT_FROM_W_MODE_RESEL_STARTED,

  /*! IRAT Resel to LTE failed */
  LTE_RRC_IRAT_FROM_W_MODE_RESEL_FAILED,

  /*! IRAT Resel to LTE aborted */
  LTE_RRC_IRAT_FROM_W_MODE_RESEL_ABORTED_BY_W,

  /*! IRAT Redir to LTE Blind Started */
  LTE_RRC_IRAT_FROM_W_MODE_REDIR_BLIND_STARTED,

  /*! IRAT Redir to LTE (List) Started */
  LTE_RRC_IRAT_FROM_W_MODE_REDIR_LIST_STARTED,

  /*! IRAT Redir to LTE (Full) started */
  LTE_RRC_IRAT_FROM_W_MODE_REDIR_FULL_STARTED,

  /*! IRAT Redir to LTE (Any) aborted by W */
  LTE_RRC_IRAT_FROM_W_MODE_REDIR_ABORTED_BY_W,

  /*! IRAT Redir to LTE failed, for conn reject case, wait timer has expired */
  LTE_RRC_IRAT_FROM_W_MODE_REDIR_WAIT_TMR_EXPIRED,

  /*! IRAT Redir to LTE failed, blind guard timer has expired */
  LTE_RRC_IRAT_FROM_W_MODE_REDIR_BLIND_TMR_EXPIRED,

  /*! IRAT Redir to LTE failed for conn release case! */
  LTE_RRC_IRAT_FROM_W_MODE_REDIR_FAILED,

  /*! IRAT Resel to LTE Success! */
  LTE_RRC_IRAT_FROM_W_MODE_RESEL_SUCCESS,

  /*! IRAT Redir to LTE Success! */
  LTE_RRC_IRAT_FROM_W_MODE_REDIR_SUCCESS,

  /*! PLMN  Search Normal */
  LTE_RRC_IRAT_FROM_W_PLMN_SRCH_NORMAL,

  /*! PLMN  Search Aborted */
  LTE_RRC_IRAT_FROM_W_PLMN_SRCH_ABORTED,

  /*! IRAT PSHO to LTE started */
  LTE_RRC_IRAT_FROM_W_MODE_PSHO_STARTED,

  /*! IRAT PSHO to LTE Success! */
  LTE_RRC_IRAT_FROM_W_MODE_PSHO_SUCCESS,

  /*! IRAT PSHO to LTE failed */
  LTE_RRC_IRAT_FROM_W_MODE_PSHO_FAILED,

  /*! IRAT PSHO to LTE aborted */
  LTE_RRC_IRAT_FROM_W_MODE_PSHO_ABORTED_BY_W,

}lte_rrc_irat_from_w_mode_e;

typedef union
{
  /*! The reselection request from WCDMA RRC */
  lte_rrc_wcdma_resel_req_s               resel_req;

  /*! The redirection request from WCDMA RRC */
  lte_rrc_wcdma_redir_req_s               redir_req;  

}lte_rrc_from_w_req_proc_u;

typedef union
{
  /*! The reselection result/cnf from CSP */
  lte_rrc_irat_to_lte_resel_cnfi_s        resel_cnf;

  /*! The redirection result/cnf from CSP */
  lte_rrc_irat_to_lte_redir_cnfi_s        redir_cnf;
}lte_rrc_from_w_rsp_proc_u;

/*! @brief Structure for storing IRAT data
*/
typedef struct
{
  /*! Current Inter-RAT procedure details */
  lte_rrc_irat_proc_e                     proc;

  /*! Current Inter-RAT procedure request structure */
  lte_rrc_from_w_req_proc_u             proc_req; 

   /*! Current Inter-RAT procedure response structure */
  lte_rrc_from_w_rsp_proc_u             proc_rsp; 
  
  /*! PLMN search Req*/
  lte_irat_plmn_srch_req_s                plmn_srch_req;
  
  /*! PLMN search Rsp*/
  lte_irat_plmn_srch_cnfi_s                plmn_srch_cnfi; 

  /*! Wait Timer for Conn. Reject Redir and disabling UTRA resel. */
  lte_rrc_tmr_s                           redir_wait_tmr;

  /*! Wait Timer value stored for testcases to verify calculation (Redir) */
  uint64                                  redir_wait_tmr_val;  

  /*! Remaining wait timer value */
  int64                                   rem_wait_time;

  /*! Blind Redir Guard Timer  */
  lte_rrc_tmr_s                           blind_redir_timer; 
  
  /*! Current mode that the STM is operating in */
  lte_rrc_irat_from_w_mode_e              mode;

    /*! Next mode that the STM will be operating in */
  lte_rrc_irat_from_w_mode_e              next_mode;

  /*! W2L PSHO RRC Reconf msg */
  lte_rrc_osys_DL_DCCH_Message                    *psho_decoded_msg_ptr;
  /*! Flag to keep track of key generation needed for W2L PSHO */
  boolean                                         psho_key_generated;
  /*! Stores PSHO failure status (if any) */
  lte_rrc_irat_to_lte_psho_failure_cause_type_e   psho_fail_cause;
  /*! Stores the IRAT NAS Info */
  lte_rrc_irat_nas_info_s                         psho_irat_nas_info;
  /*! Stores the timing info */
  lte_irat_target_cell_info_s                     psho_x2l_ho_info;
  /*! Serving cell info for ASF fingerprinting */
  source_cell_info_type                           psho_serving_cell_info;
  /*! UTRA cell info for too early handover */
  utra_serving_cell_info_type                 utra_cell_info;

} lte_rrc_irat_from_w_mgr_dynamic_data_s;

typedef struct
{
  /*! < Blind Redir guard timer value */
  uint32                                  blind_redir_time_value;
  
} lte_rrc_irat_from_w_mgr_static_data_s;

/*! @brief Structure for storing IRAT data
 */
typedef struct
{ 
  /* Dynamic data pointer */
  lte_rrc_irat_from_w_mgr_dynamic_data_s    *dd_ptr;

  /* Static data pointer */
  lte_rrc_irat_from_w_mgr_static_data_s     *sd_ptr;
  
} lte_rrc_irat_from_w_mgr_s;

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#endif /* LTE_RRC_IRAT_FROM_W_MGRI_H */



/*!
  @file
  lte_rrc_irat_from_DO_mgri.h

  @brief
  This module contains the entry, exit, and transition functions
  necessary to implement the following state machines:

  @detail
  LTE_RRC_IRAT_FROM_DO_MGR_SM ( 1 instance/s )


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_from_DO_mgri.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/2010 sk      Initial revision
===========================================================================*/

#ifndef LTE_RRC_IRAT_FROM_DO_RATI_H
#define LTE_RRC_IRAT_FROM_DO_RATI_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

//! @todo Include necessary files here
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_irat_utils.h"
#include "lte_as.h"

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*! @brief Enum to represent various deactivation causes
*/
typedef enum
{
    /*! LTE stack deactivated due to procedure failure */
    LTE_RRC_FROM_DO_PROC_FAILED = 0,

    /*! LTE stack deactivated due to procedure being aborted */
    LTE_RRC_FROM_DO_PROC_ABORTED,

}lte_rrc_from_DO_deactivation_cause_type_e;

/*! @brief Structure for reselection to G
*/
typedef struct
{
  uint16 freq;                        /*!< Freq to which the LTE cell belongs */

  lte_phy_cell_id_t cell_id;          /*!< Physical cell id of the LTE cell */

  lte_rrc_cdma_irat_nas_info_s nas_params; /*!< NAS related params from CDMA for LTE acq */

  /*!< Reason for reselection failure */
  lte_rrc_irat_to_lte_failure_cause_type_e resel_fail_cause; 

  /*!< Cell bar time */
  uint32                                   cell_bar_time;

}lte_rrc_from_DO_resel_s;

/*! @brief Structure for redirection to G
*/
typedef struct
{
  /*!< Composite of LTE Frequency  and PCI info etc */
  lte_rrc_hdr_redir_earfcn_list_s   redir_earfcn_list;

  /*!< NAS related params for LTE acq */
  lte_rrc_cdma_irat_nas_info_s nas_params; 

  /*!< n+4 sec timer for redirection */
  lte_rrc_tmr_s redir_timer; 

  /*! < Redirection phase i.e Initial List or NeighborList/Full srch */
  lte_rrc_to_lte_redirection_phase_type_e redir_phase;

}lte_rrc_from_DO_redir_s;

/*! @brief Structure for storing the inter-RAT procedure details
*/
typedef struct
{
  /*!< Procedure indicator */
  lte_rrc_irat_proc_e   proc_type;
  union
  {
    lte_rrc_from_DO_resel_s from_DO_resel_info;
    lte_rrc_from_DO_redir_s from_DO_redir_info;
  }proc;
}lte_rrc_from_DO_proc_s;

/*! @brief Structure for dynamic private data
*/
typedef struct
{
  /*!< Inter-RAT procedure details */
  lte_rrc_from_DO_proc_s from_DO_proc_info;

  /*! Wait Timer for Conn. Reject Redir and disabling UTRA resel. */
  lte_rrc_tmr_s                           redir_wait_tmr;

  /*! Wait Timer value stored for testcases to verify calculation (Redir) */
  uint64                                  redir_wait_tmr_val;  

  /*! Remaining wait timer value */
  int64                                   rem_wait_time;

  /* Reason for deactivation of LTE stack */
  lte_rrc_from_DO_deactivation_cause_type_e deact_cause;

}lte_rrc_irat_from_DO_dynamic_data_s;

/*! @brief Structure for static private data
*/
typedef struct
{
  /*!< Procedure indicator */
  lte_rrc_irat_proc_e  proc_type_for_abort_during_deact;
}lte_rrc_irat_from_DO_static_data_s;

/*! @brief Structure for state-machine per-instance local variables
*/
typedef struct
{
  /* Dynamic data pointer */
  lte_rrc_irat_from_DO_dynamic_data_s *dd_ptr;
  /* Static data pointer */
  lte_rrc_irat_from_DO_static_data_s  *sd_ptr;
  
} lte_rrc_irat_from_DO_s;

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#endif /* LTE_RRC_IRAT_FROM_DO_RATI_H */



/*!
  @file
  lte_rrc_irat_from_G_mgri.h

  @brief
  This module contains the entry, exit, and transition functions
  necessary to implement the following state machines:

  @detail
  LTE_RRC_IRAT_FROM_G_MGR_SM ( 1 instance/s )


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_from_G_mgri.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/31/2011 gp      Added Min,Max values for G2L guard timer value
01/24/2009 sk      Initial revision
===========================================================================*/

#ifndef LTE_RRC_IRAT_FROM_G_RATI_H
#define LTE_RRC_IRAT_FROM_G_RATI_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

//! @todo Include necessary files here
#include "lte_variation.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_irat_utils.h"
#include "lte_as.h"

/* Timer value is ms*/
#define LTE_RRC_IRAT_FROM_G_REDIR_TIMER_VAL 10000
#define LTE_RRC_IRAT_FROM_G_REDIR_TIMER_VAL_MIN 1000

/* Timer value is ms (default is 2 sec) */
#define LTE_RRC_IRAT_FROM_G_REDIR_GUARD_TIMER_VAL 2000
#define LTE_RRC_IRAT_FROM_G_REDIR_GUARD_TIMER_VAL_MIN (500)
#define LTE_RRC_IRAT_FROM_G_REDIR_GUARD_TIMER_VAL_MAX (20000)

/*! G2L redir priority timer duration for TA cases : 1.2s */
#define LTE_RRC_IRAT_G2L_REDIR_HIGH_PRIORITY_TIMER_VAL   1200

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*! @brief Enum to represent various deactivation causes
*/
typedef enum
{
    /*! LTE stack deactivated due to procedure failure */
    LTE_RRC_FROM_G_PROC_FAILED = 0,

    /*! LTE stack deactivated due to procedure being aborted */
    LTE_RRC_FROM_G_PROC_ABORTED,

    /*! LTE stack deactivated due to normal completion of PLMN srch */
    LTE_RRC_FROM_G_PLMN_SRCH_NORMAL,

}lte_rrc_from_G_deactivation_cause_type_e;

/*! @brief Enum to represent various redirection phases
*/
typedef enum
{
    /* Value none */
    LTE_RRC_FROM_G_PROC_REDIR_NONE = 0,

    /*! Redirection - Initial Phase */
    LTE_RRC_FROM_G_PROC_REDIR_INITIAL = 1,

    /*! Redirection - Neighbor List / Full Search */
    LTE_RRC_FROM_G_PROC_REDIR_FULL_SRCH = 2

}lte_rrc_from_G_redirection_phase_type_e;

/*! @brief Enum to represent various redirection types
*/
typedef enum
{
    /* Value none */
    LTE_RRC_FROM_G_PROC_REDIR_NONE_TYPE = 0,

    /*! Redirection - Initial Phase */
    LTE_RRC_FROM_G_PROC_REDIR_UE_INITIATED_TYPE = 1,

    /*! Redirection - Neighbor List / Full Search */
    LTE_RRC_FROM_G_PROC_REDIR_NW_INITIATED_TYPE = 2

}lte_rrc_from_G_redirection_type_e;

/*! @brief Structure for reselection to G
*/
typedef struct
{
  uint16 freq;                        /*!< Freq to which the LTE cell belongs */

  lte_phy_cell_id_t cell_id;          /*!< Physical cell id of the LTE cell */

  lte_rrc_irat_nas_info_s nas_params; /*!< NAS related params for LTE acq */

  /*!< Reason for reselection failure */
  lte_rrc_irat_to_lte_failure_cause_type_e resel_fail_cause; 

  /*!< Cell bar time */
  uint32                                   cell_bar_time;

}lte_rrc_from_G_resel_s;

/*! @brief Structure for PLMN SRCH from G
*/
typedef struct
{

  lte_irat_plmn_srch_req_s                plmn_srch_req; /*!< PLMN search Req*/
 

  lte_irat_plmn_srch_cnfi_s                plmn_srch_cnfi; /*!< PLMN search Rsp*/


}lte_rrc_from_G_plmn_srch_s;

/*! @brief Structure for redirection to G
*/
typedef struct
{
  /*! Num frequencies present */
  uint8                                     earfcn_count;

  /*!< Composite of LTE Frequency with black listed cells , PCI info etc */
  lte_rrc_irat_to_lte_redir_earfcn_info_s   earfcn_info[LTE_RRC_IRAT_MAX_REDIR_FREQ];

  /*! < Neighbor list obtained from SI messages */
  lte_rrc_irat_nbr_earfcn_list_s nbr_list_info;

  /*!< NAS related params for LTE acq */
  lte_rrc_irat_nas_info_s nas_params; 

  /*!< 10 sec timer for redirection */
  lte_rrc_tmr_s redir_timer; 

  /*!< Blind Redir Guard Timer  */
  lte_rrc_tmr_s redir_guard_timer; 

  /*! < Redirection phase i.e Initial List or NeighborList/Full srch */
  lte_rrc_from_G_redirection_phase_type_e redir_phase;

  /*! < Redirection type i.e NW Initiated or UE Initiated */
  lte_rrc_from_G_redirection_type_e redir_type;

}lte_rrc_from_G_redir_s;

/*! @brief Structure for storing the inter-RAT procedure details
*/
typedef struct
{
  /*!< Procedure indicator */
  lte_rrc_irat_proc_e   proc_type;
  union
  {
    lte_rrc_from_G_resel_s from_G_resel_info;
    lte_rrc_from_G_redir_s from_G_redir_info;
    lte_rrc_from_G_plmn_srch_s from_G_plmn_srch_info;
  }proc;
}lte_rrc_from_G_proc_s;

/*! @brief Structure for dynamic private data
*/
typedef struct
{
  /*!< Inter-RAT procedure details */
  lte_rrc_from_G_proc_s from_G_proc_info;

  /* Reason for deactivation of LTE stack */
  lte_rrc_from_G_deactivation_cause_type_e deact_cause;

}lte_rrc_irat_from_G_dynamic_data_s;

/*! @brief Structure for static private data
*/
typedef struct
{
  /*! < Blind Redir guard timer value */
  uint32        redir_guard_time_value;
  /*! < timer value */
  uint64        redir_time_value;
  /*! < decide which route to follow */
  boolean       timer_set_externally;
  /*! < decide whether to carry list search only */
  boolean       list_srch_only;

}lte_rrc_irat_from_G_static_data_s;

/*! @brief Structure for state-machine per-instance local variables
*/
typedef struct
{
  /* Dynamic data pointer */
  lte_rrc_irat_from_G_dynamic_data_s *dd_ptr;

    /* Static data pointer */
  lte_rrc_irat_from_G_static_data_s *sd_ptr;
  
} lte_rrc_irat_from_G_s;


#endif /* LTE_RRC_IRAT_FROM_G_RATI_H */

extern void lte_rrc_irat_from_G_set_redir_guard_timer_value( uint32 redir_guard_expiry_time );



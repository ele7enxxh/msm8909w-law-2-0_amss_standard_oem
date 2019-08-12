/*!
  @file
  lte_rrc_misci.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_misci.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef LTE_RRC_MISCI_H
#define LTE_RRC_MISCI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include "lte_rrc_int_msg.h"
#include "lte_rrc_misc.h"
#include "sys.h"
#include "policyman.h"


typedef struct
{
  boolean is_trm_reserved;
  lte_rrc_misc_trm_reserve_reason_e reserve_reason;
  lte_rat_e reserve_rat_type;
  lte_rrc_misc_trm_release_reason_e release_reason;
}lte_rrc_misc_trm_info_s;

/*! @brief Structure for static private data
*/
typedef struct
{
  /*! Current UE mode */
  sys_ue_mode_e_type ue_mode;
  boolean            is_ue_mode_substate_srlte;
  sys_as_irat_status_e_type irat_meas_type[SYS_RAT_MAX]; /* This is for SGLTE measurements or Not SGLTE meas*/
  /*! Current fequency list type per sub  */
  policyman_freq_list_type_t  freq_list_type[SYS_MODEM_AS_ID_NO_CHANGE];

}lte_rrc_misc_static_data_s;

/*! @brief This structure holds the Dynamic part of private data
*/
typedef struct
{
  /*!< Flag to indicate if any procedure is active or not */
  boolean is_procedure_active;
  
  boolean csfb_call;
  lte_rrc_misc_trm_info_s trm_info;

  /*!< Duration for which TRM has to be acquired with high priority. Applicable to cases like G2L blind redir */
  lte_rrc_tmr_s high_priority_timer;

  /*!< Client which requested high priority */
  lte_rrc_trm_client_id_e high_priority_req_client;
  
  /*!< Is common msim crat cnf pending for irat capability cnf */
  boolean update_msim_crat_feat_needed;

  /*!< Is c irat capability cnf pending for NAS*/
  boolean lte_irat_cap_cnf_pending;

  /*!< Waiting on msim_crat_feature_cnf to unpend other triggers */
  boolean msim_crat_feature_cnf_pending;
  
} lte_rrc_misc_dynamic_data_s;

/*! @brief Structure for state-machine per-instance local variables
*/
typedef struct
{
  /*!< Static part of private data */
  lte_rrc_misc_static_data_s  *sd_ptr;
  /*!< Dynamic part of private data */
  lte_rrc_misc_dynamic_data_s *dd_ptr;
} lte_rrc_misc_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* LTE_RRC_MISCI_H */

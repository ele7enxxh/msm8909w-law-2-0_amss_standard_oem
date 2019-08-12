/*!
  @file
  lte_ml1_dlm_embms_priority.h

  @brief
  eMBMS priority related data.
*/

/*===========================================================================

  Copyright (c) 2011- 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/ML1/dlm/src/lte_ml1_dlm_embms_priority.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

12/03/14   as      Initial version

===========================================================================*/
#ifndef LTE_ML1_DLM_EMBMS_PRIORITY_H
#define LTE_ML1_DLM_EMBMS_PRIORITY_H

typedef struct lte_ml1_dlm_embms_priority_db_s  lte_ml1_dlm_embms_priority_db_t;

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lte_variation.h"
#include "lte_ml1_comdef.h"
#include "lte_ml1_common_timer.h"

#include "lte_ml1_dlm_common_if.h"
#include "lte_ml1_dlm_embms_stm.h"
#include "lte_ml1_mgr_struct.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

struct lte_ml1_dlm_embms_priority_db_s
{
  lte_ml1_dlm_embms_priority_cfg_t config[LTE_CPHY_EMBMS_MAX_PRIORITY_CFG];
  lte_ml1_dlm_embms_priority_cfg_t pending[LTE_CPHY_EMBMS_MAX_PRIORITY_CFG];

  /* Warmup time in usec */
  uint32 warmup_time_usec;
};


/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_hpt_is_active

===========================================================================*/
/*!
  @brief
  Determines if an HPT is active

  @return
  TRUE/FALSE
*/
/*=========================================================================*/
boolean lte_ml1_embms_priority_hpt_is_active
(
  lte_mem_instance_type instance
);

/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_schedule_hpt

===========================================================================*/
/*!
  @brief
  Schedule's eMBMS priority

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_embms_priority_schedule_hpt
(
  lte_mem_instance_type  instance,
  lte_ml1_dlm_embms_priority_cfg_t* priority
);

/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_abort_hpt

===========================================================================*/
/*!
  @brief
  Aborts scheduled eMBMS HPT

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_embms_priority_abort_hpt
(
  lte_mem_instance_type instance,
  lte_ml1_dlm_embms_priority_cfg_t*  hpt
);


/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_create_hpt_timer

===========================================================================*/
/*!
  @brief
  Create High Priority Traffic (HPT) Timer

  @return

*/
/*=========================================================================*/
void lte_ml1_embms_priority_create_hpt_timer
(
  lte_ml1_dlm_embms_db_s  *mgr_data_inst
);

/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_delete_hpt_timer

===========================================================================*/
/*!
  @brief
  Delete High Priority Traffic (HPT) Timer

  @return

*/
/*=========================================================================*/
void lte_ml1_embms_priority_delete_hpt_timer
( 
  lte_ml1_dlm_embms_db_s  *mgr_data_inst
);

/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_hpt_timer_expiry

===========================================================================*/
/*!
  @brief
  Handle's the HPT timer expiry

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_embms_priority_hpt_timer_expiry( uint32 id );

/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_hpt_conflict

===========================================================================*/
/*!
  @brief
  Determines if current HPT conflicts with an unlock request

  @return
  TRUE if conflicts with unlock
*/
/*=========================================================================*/
boolean lte_ml1_embms_priority_hpt_conflict
(
  lte_mem_instance_type    instance,
  lte_ml1_schdlr_sf_time_t obj_start_time,
  trm_client_enum_t         client,
  trm_reason_enum_t         reason
);


/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_get_pending_hpt_data

===========================================================================*/
/*!
  @brief
  Get pending eMBMS priority increase timing data

  @return
  eMBMS priority increase timing
*/
/*=========================================================================*/
lte_ml1_dlm_embms_priority_cfg_t* lte_ml1_embms_priority_get_pending_hpt_data
(
  lte_mem_instance_type instance,
  uint8     index
);

/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_is_equal

===========================================================================*/
/*!
  @brief
  Compares the priority is equal to stored priority

  @return
  TRUE/FALSE
*/
/*=========================================================================*/
boolean lte_ml1_embms_priority_is_equal
(
  lte_cphy_embms_data_priority_s const * lhs,
  lte_cphy_embms_data_priority_s const * rhs
);

/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_set_priority

===========================================================================*/
/*!
  @brief
  Sets the priority for eMBMS

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_embms_priority_set_priority
(
  lte_mem_instance_type instance,
  lte_cphy_embms_data_priority_s const * data
);

/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_set_hpt_active

===========================================================================*/
/*!
  @brief
  Sets the activity of the priority for eMBMS

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_embms_priority_set_hpt_active
(
  lte_ml1_dlm_embms_priority_cfg_t* hpt_ptr,
  boolean               active
);

/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_set_hpt_action

===========================================================================*/
/*!
  @brief
  Sets the action of the priority for eMBMS on timer expiry

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_embms_priority_set_hpt_action
(
  lte_ml1_dlm_embms_priority_cfg_t* hpt,
  lte_ml1_embms_priority_action_e action
);


/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_chain_reserve_time_ind

===========================================================================*/
/*!
  @brief
  Handle LTE_ML1_MGR_CHAIN_RESERVE_ISR_IND

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_embms_priority_chain_reserve_time_ind
(
  const lte_ml1_sleepmgr_chain_reserve_time_ind_s *rsrv_ind,
   /*! Pointer to right instance */
   lte_ml1_manager_data_s                    *mgr_data_inst
);
/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_sleep_start_ind

===========================================================================*/
/*!
  @brief
  Handle LTE_ML1_SLEEPMGR_SLEEP_START_IND

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_embms_priority_sleep_start_ind
(
   const lte_ml1_sleepmgr_sleep_start_ind_s *msg,
   /*! Pointer to right instance */
   lte_ml1_manager_data_s                  *mgr_data_inst
);

/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_wakeup_isr_ind

===========================================================================*/
/*!
  @brief
  Handle LTE_ML1_SLEEPMGR_WAKEUP_ISR_IND

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_embms_priority_wakeup_isr_ind
(
   const lte_ml1_sleepmgr_wakeup_isr_ind_s *msg,
      /*! Pointer to right instance */
   lte_ml1_manager_data_s                  *mgr_data_inst
);

/*===========================================================================

  FUNCTION:  lte_ml1_embms_priority_trm_chain_grant_ind

===========================================================================*/
/*!
  @brief
  Handle LTE_ML1_RFMGR_TRM_CHAIN_GRANT_IND

  @return
  
*/
/*=========================================================================*/
void lte_ml1_embms_priority_trm_chain_grant_ind
(
  lte_mem_instance_type    instance
);
#endif /* LTE_ML1_DLM_EMBMS_PRIORITY_H */

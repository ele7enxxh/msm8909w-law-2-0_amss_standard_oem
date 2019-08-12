/*!
  @file lte_rrc_stm.h

  @brief
  Definition of the DEBUG_HOOK and ERROR_HOOK functions for the entire RRC
  thread i.e. all state machines defined for RRC.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_stm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/2008 sureshs Changed doxygen comment
07/20/2008 sureshs Initial version
===========================================================================*/

#ifndef LTE_RRC_STM_H
#define LTE_RRC_STM_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <stm2.h>
#include "lte_rrc_diag.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Mask to enable logging of SM Entry fns */
#define LTE_RRC_STM_LOG_SM_ENTRY        ((uint32)1<<(uint32)STM_ENTRY_FN)

/*! @brief Mask to enable logging of SM Exit fns */
#define LTE_RRC_STM_LOG_SM_EXIT         ((uint32)1<<(uint32)STM_EXIT_FN)

/*! @brief Mask to enable logging of SM State Entry fns */
#define LTE_RRC_STM_LOG_ST_ENTRY        ((uint32)1<<(uint32)STM_STATE_ENTRY_FN)

/*! @brief Mask to enable logging of SM State Exit fns */
#define LTE_RRC_STM_LOG_ST_EXIT         ((uint32)1<<(uint32)STM_STATE_EXIT_FN)

/*! @brief Mask to enable logging of SM transition fns */
#define LTE_RRC_STM_LOG_TRANS           ((uint32)1<<(uint32)STM_POST_TRANS_FN)


/*\
 *  Grow STM-definition-based masks from the LSB upward above    /\
 *
 *   - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Grow 'internal' debugging flags from the MSB downward below  \/
\*/

/*! @brief Mask to disable transitions from being stored in trace buffer */
#define LTE_RRC_STM_DISABLE_TRANS_TRACE     ((uint32)1<<30)

/*! @brief Mask to enable all STM LOG bits */

#define LTE_RRC_STM_LOG_ALL_MASK                       \
          (LTE_RRC_STM_LOG_SM_ENTRY   |                \
           LTE_RRC_STM_LOG_SM_EXIT    |                \
           LTE_RRC_STM_LOG_ST_ENTRY   |                \
           LTE_RRC_STM_LOG_ST_EXIT    |                \
           LTE_RRC_STM_LOG_TRANS)

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

void lte_rrc_stm_error
(
  stm_status_t error, /*!< error number */
  const char *filename, /*!< file name */
  uint32 line, /*!< line number */
  stm_state_machine_t *sm /*!< state machine */
);

void lte_rrc_stm_debug
(
  stm_debug_event_t event, /*!< Debug event type code */
  stm_state_machine_t *sm, /*!< State Machine instance */
  stm_state_t state, /*!< Next state on transition */
  void *payload /*!< Payload pointer */
);

#endif /* LTE_RRC_STM_H */


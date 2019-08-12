/*!
  @file
  lte_rlcul_cfg.h

  @brief
  Header file for the RLC UL configuration related procedures

*/

/*==============================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcul_cfg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/25/16   sb      CR987470:Static mutex for PDU Build and Retx Q locks
09/16/15   ru      CR896720: PDCP Discard Timer Overwrite
10/12/15   el      CR 884865: FR 29101 Fast RLF Declaration for fast resync with eNB
11/03/14   sb      CR748072: Handle a corner case where NACKed PDU is enqueued to
                   to retx Q after RLC re-establishment is processed
12/10/12   sb      CR399603: Avoid flow control for high priority bearers to maintain 
                         voice quality
08/22/12   ax      CR383618: L2 modem heap use reduction phase I  
02/17/09   ar      Externalized function lte_rlcul_cfg_add_rb()
11/17/08   ar      1. Added task startup initialization function
                      lte_rlcul_cfg_init() for initializing cfg related vars
09/16/08   ar      Added basic RLCUL configuration support
09/16/08   ar      initial version

==============================================================================*/

#ifndef LTE_RLCUL_CFG_H
#define LTE_RLCUL_CFG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <lte_rlc_rrc.h>
#include "lte_rlculi.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  lte_rlcul_cfg_mutex_deinit

==============================================================================*/
/*!
    @brief
    This function deinitializes all the mutexes belonging to lte_rlcul_cfg.c
    file.

    @detail
    
    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_cfg_mutex_deinit
(
  boolean destroy_mutex
);
/* lte_rlcul_cfg_mutex_deinit() */

/*==============================================================================

  FUNCTION:  lte_rlcul_cfg_init

==============================================================================*/
/*!
    @brief
    This function initializes all the data members belonging to lte_rlcul_cfg.c
    file.

    @detail
    Following variables are initialized:
    - LTE_RLCUL_CFG_CNF message header

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_cfg_init
(
  void
);
/* lte_rlcul_cfg_init() */

/*==============================================================================

  FUNCTION:  lte_rlcul_cfg_req_handler

==============================================================================*/
/*!
    @brief
    This function handles the configuration request from RRC and applies the
    new/updated configuration provided by RRC in the request message.

    @detail
    The configuration reason can be Reconfig, Handover or Release. Appropriate
    functions are called for all the three cases. At the end, a config confirm
    message is sent back to RRC indicating the status of the config request
    message.

    @return
    void
*/
/*============================================================================*/
EXTERN void lte_rlcul_cfg_req_handler
(
  const lte_rlcul_cfg_req_s* cfg_ptr /*!< Ptr to the new config from RRC */
);
/* lte_rlcul_cfg_req_handler() */


/*==============================================================================

  FUNCTION:  lte_rlcul_cfg_release_all_rbs

==============================================================================*/
/*!
    @brief
    This function handles the connection release configuration request.

    @detail
    Connection release request translated into releasing of all the RBs

    @return
    Status indicating whether the new configuration was applied successfully or
    not

    @see related_function()
    lte_rlcul_cfg_apply_regular_cfg - For applying regular configuration
    lte_rlcul_cfg_apply_handover_cfg - For applying HO configuration request

*/
/*============================================================================*/
EXTERN lte_errno_e lte_rlcul_cfg_release_all_rbs
(
  void
);
/* lte_rlcul_cfg_release_all_rbs() */


/*==============================================================================

  FUNCTION:  lte_rlcul_cfg_add_rb

==============================================================================*/
/*!
    @brief
    This function adds a single Radio Bearer in RLCUL

    @detail
    As part of adding an RB, following actions are performed:
    - Get a free control block pointer for the free q
    - Initialize the RB
    - Update the active rb information arrays

    @return
    Status indicating whether the new configuration was applied successfully or
    not

    @see related_function()
    lte_rlcul_cfg_release_rb - For releasing a single RB
    lte_rlcul_cfg_modify_rb - For modifying a particular RB

*/
/*============================================================================*/
EXTERN lte_errno_e lte_rlcul_cfg_add_rb
(
  const lte_rlcul_rb_info_s* add_rb_info_ptr  /*!< Pointer to the to-be-added
                                                   RB info database */
);
/* lte_rlcul_cfg_add_rb() */

/*==============================================================================

  FUNCTION:  lte_rlcdl_cfg_dealloc_cb

==============================================================================*/
/*!
    @brief
    deallocate resources held by the control block.

    @return
    None
*/
/*============================================================================*/
void lte_rlcul_cfg_dealloc_cb
(
  lte_rlculi_ctrl_blk_s*  ctrl_blk_ptr   /*!< control block pointer */
);
/* lte_rlcul_cfg_dealloc_cb() */

EXTERN void lte_rlcul_qos_info_handler
(
  ds_3gpp_bearer_qos_info_msg_s* qos_info
);
/*lte_rlcul_qos_info_handler()*/

/*==============================================================================

  FUNCTION:  lte_rlcul_cfg_flush_retx_q

==============================================================================*/
/*!
    @brief
    Function to flushes the RLCUL retransmission queue

    @detail
    This functions dequeues all the nacked pdus/pdu segments from the retx q
    and updates the retx_q_size variable accordingly

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_cfg_flush_retx_q
(
  lte_rlculi_ctrl_blk_s* ctrl_blk_ptr /*!< UL control block pointer of the
                                           to-be-released RB */
);

/*==============================================================================

  FUNCTION:  lte_rlcul_overrite_rlc_detection_cfg

==============================================================================*/
/*!
    @brief
    This function handles the rlc detection cfg

    @return
    void
*/
/*============================================================================*/

EXTERN void lte_rlcul_overrite_rlc_detection_cfg
(
  int16              dl_pathloss
);

/*==============================================================================

  FUNCTION:  lte_rlcul_get_qci_info

==============================================================================*/
/*!
    @brief
    Function to get the qci info according to the eps_id

    @detail

    @return
    ds_3gpp_lte_qci_enum_type

*/
/*============================================================================*/
EXTERN ds_3gpp_lte_qci_enum_type lte_rlcul_get_qci_info
(
  uint8 eps_id
);

/*==============================================================================

  FUNCTION:  lte_rlcul_get_rb_mutex_info

==============================================================================*/
/*!
    @brief
    Function to get the pointer to RB mutex structure

    @detail

    @return
    lte_rlculi_rb_mutex_info_s

*/
/*============================================================================*/
EXTERN lte_rlculi_rb_mutex_info_s* lte_rlcul_get_rb_mutex_info
(
  
);

#endif /* LTE_RLCUL_CFG_H */

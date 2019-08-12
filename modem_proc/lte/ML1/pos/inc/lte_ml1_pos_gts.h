#ifndef LTE_ML1_POS_GTS_H
#define LTE_ML1_POS_GTS_H
/*!
  @file
  lte_ml1_pos_gts.h

  @brief
  This module contains the interface for GPS Global Time Services

*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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



$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/ML1/pos/inc/lte_ml1_pos_gts.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/12   tjc     Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte_cphy_msg.h>
#include <intf_sys_msg.h>
#include <msgr.h>

#include <lte_ml1_mem.h>

/*===========================================================================

                       EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief LTE POS LPPM state */
typedef enum {
  LTE_ML1_POS_GTS_STATE_NO_SERVICE, /*!< No service state */
  LTE_ML1_POS_GTS_STATE_IDLE,       /*!< Idle state */
  LTE_ML1_POS_GTS_STATE_CONNECTED,  /*!< Connected state */
  LTE_ML1_POS_GTS_STATE_MAX,        /*!< Max states */
} lte_ml1_pos_gts_state_t;  


/*===========================================================================

                       EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_ml1_pos_gts_init

===========================================================================*/
/*!
    @brief
    This function initializes LTE GPS Global Time Services module.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_pos_gts_init
(
  lte_mem_instance_type instance,
  msgr_client_t *client_ptr,   /*!< Message router client pointer          */
  msgr_id_t      mq_id         /*!< Lower layer message queue              */
);

/*===========================================================================

  FUNCTION:  lte_ml1_pos_gts_deinit

===========================================================================*/

/*!
    @brief
    This function deinitializes LTE GPS Global Time Services module.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_pos_gts_deinit(lte_mem_instance_type instance);

/*===========================================================================

  FUNCTION:  lte_ml1_pos_gts_process_message

===========================================================================*/
/*!
    @brief
    This function processes a LTE GPS Global Time Services module.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_pos_gts_process_message
(
  lte_mem_instance_type instance,
  /*! Message UMID */
  msgr_umid_type  msg_umid,
  /*! Message buf pointer */
  uint8          *msg_buf
);

/*===========================================================================

  FUNCTION:  lte_ml1_pos_gts_enable

===========================================================================*/
/*!
    @brief
    This function enables GPS LTE GPS Global Time Services module.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_pos_gts_enable(lte_mem_instance_type instance);

/*===========================================================================

  FUNCTION:  lte_ml1_pos_gts_disable

===========================================================================*/
/*!
    @brief
    This function disables GPS LTE GPS Global Time Services module

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_pos_gts_disable(lte_mem_instance_type instance);

/*===========================================================================

  FUNCTION:  lte_ml1_pos_timexfer_set_state

===========================================================================*/
/*!
    @brief
    This function sets the state for LTE GPS Global Time Services module

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_pos_gts_set_state
(
  lte_mem_instance_type instance,
  /*! TIMEXFER state being entered into */
  lte_ml1_pos_gts_state_t next_state
);

/*===========================================================================

  FUNCTION:  lte_ml1_pos_gts_send_wakeup_complete_ind

===========================================================================*/
/*!
    @brief
    This function sends a message indicating that LTE wakeup is complete.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_pos_gts_send_wakeup_complete_ind(lte_mem_instance_type instance);

/*===========================================================================

  FUNCTION:  lte_ml1_pos_gts_timer_expiry_ind

===========================================================================*/
/*!
    @brief
    This function sends a message indicating that LTE wakeup is complete.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_pos_gts_timer_expiry_ind(lte_mem_instance_type instance);

/*===========================================================================

  FUNCTION:  lte_ml1_pos_gts_paging_ind

===========================================================================*/
/*!
    @brief
    This function sends a message indicating
    that LTE paging occasion happened.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_pos_gts_paging_ind(lte_mem_instance_type instance);

#endif 

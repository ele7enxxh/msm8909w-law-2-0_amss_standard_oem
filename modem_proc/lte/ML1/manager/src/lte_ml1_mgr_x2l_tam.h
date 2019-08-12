/*!
  @file
  lte_ml1_mgr_x2l_tam.h

  @brief
  Tune Away Manager header
*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/ML1/manager/src/lte_ml1_mgr_x2l_tam.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/16   gp      Initial Version 
 
===========================================================================*/
#include "lte_ml1_comdef.h"
#include "lte_variation.h"
#include "lte_ml1_mgr_msg.h"
#include <lte_cphy_msg.h>


#ifndef LTE_ML1_MGR_X2L_TAM_H
#define LTE_ML1_MGR_X2L_TAM_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


extern lte_ml1_mgr_tam_data_s mgr_tam_data;


#define LTE_ML1_G2L_TAM_MIN_RESERVE_DURATION_MS          100//60ms for min for PS call sustain + TA overhead

#define LTE_ML1_MGR_X2L_TAM_NUM_MS_OBJS   1


/*===========================================================================

  FUNCTION:  lte_ml1_mgr_x2l_tam_set_priority

===========================================================================*/
/*!
    @brief
    API to change the TRM priority when G2L mode enabled

    @return
    None
*/
/*=========================================================================*/
void lte_ml1_mgr_x2l_tam_set_priority
(
  lte_mem_instance_type instance
);

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_x2l_tam_data_init

===========================================================================*/
/*!
    @brief
    Init TAM data

    @return
    None
*/
/*=========================================================================*/
void lte_ml1_mgr_x2l_tam_data_init
(
  lte_ml1_manager_data_s  *mgr_data_inst
);

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_x2l_tam_data_deinit

===========================================================================*/
/*!
    @brief
    Init TAM data

    @return
    None
*/
/*=========================================================================*/
void lte_ml1_mgr_x2l_tam_data_deinit
(
  lte_ml1_manager_data_s  *mgr_data_inst
);

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_x2l_tam_update_mode

===========================================================================*/
/*!
    @brief
    Returns the ML1 g2L mode

    @return
    ML1 manager state
*/
/*=========================================================================*/
void lte_ml1_mgr_x2l_tam_update_mode
(
	lte_mem_instance_type instance,
	/*! umid of the request message */
	msgr_umid_type umid,
	/*! payload of the message */
	void * payload
);

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_x2l_tam_register_objs

===========================================================================*/
/*!
    @brief
    Register ML1 mgr TAM schdlr objects

    @return
    None
*/
/*=========================================================================*/
void lte_ml1_mgr_x2l_tam_register_objs( lte_ml1_manager_data_s  *mgr_data_inst );
/*===========================================================================

  FUNCTION:  lte_ml1_mgr_x2l_tam_deregister_objs

===========================================================================*/
/*!
    @brief
    De-register ML1 mgr  TAM schdlr objects

    @return
    None
*/
/*=========================================================================*/
void lte_ml1_mgr_x2l_tam_deregister_objs( void );

/*===========================================================================
  FUNCTION:  lte_ml1_mgr_x2l_tam_schdl_obj

===========================================================================*/
/*!
    @brief
    Schdl Tune Away Object 

    @detail
    Currently, use to schedule cfg obj with ASAP/TNXT type on current cell

    @return
    None
*/
/*=========================================================================*/
void lte_ml1_mgr_x2l_tam_schdl_obj
( 
  /*! Pointer to right instance */
  lte_ml1_manager_data_s             *mgr_data_inst,
    /* Object id */
  lte_ml1_schdlr_obj_id_e            obj_id
);


/*===========================================================================
  FUNCTION:  lte_ml1_mgr_x2l_tam_desched_obj

===========================================================================*/
/*!
    @brief
    Deschedule X2L TAM object

    @detail
    Currently, use to schedule cfg obj with ASAP/TNXT type on current cell

    @return
    None
*/
/*=========================================================================*/

void lte_ml1_mgr_x2l_tam_desched_obj
(
lte_mem_instance_type instance,
lte_ml1_schdlr_obj_id_e  obj_id
);


/*===========================================================================

  FUNCTION:  lte_ml1_mgr_x2l_update_reserve_trm_sfn_cnt

===========================================================================*/
/*!
  @brief
  Compute TRM Reservation time based on SIB scheduling and paging and update 
  in X2L DB

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_mgr_x2l_update_reserve_trm_sfn_cnt
(
  /*! MGR Data Pointer*/
  lte_ml1_manager_data_s *mgr_data_inst
);

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_x2l_tam_handle_msgs

===========================================================================*/
/*!
  @brief
  Used for reception of messages by TAM  
  It is expected that the L1 message redirector directs messages to this
  callback for processing by the downlink manager.

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_mgr_x2l_tam_handle_msgs
(
  uint32      length,
  /**< Pointer to to the TAM msg message */
  void const *x2l_tam_msg, 
  /*! Pointer to right instance */
  lte_ml1_manager_data_s  *mgr_data_inst
);

#endif /* LTE_ML1_MGR_X2L_TAM_H */

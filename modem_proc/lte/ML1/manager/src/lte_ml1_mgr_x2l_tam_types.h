/*!
  @file
  lte_ml1_mgr_x2l_tam_types.h

  @brief
  Tune Away Manager types header
*/

/*===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/ML1/manager/src/lte_ml1_mgr_x2l_tam_types.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/03/16   gp     Initial Version

===========================================================================*/

#ifndef LTE_ML1_MGR_X2L_TAM_TYPES_H
#define LTE_ML1_MGR_X2L_TAM_TYPES_H

#include "lte_ml1_comdef.h"
#include "lte_variation.h"
#include "lte_cphy_msg.h"


typedef struct lte_ml1_mgr_x2l_tam_db_s lte_ml1_mgr_x2l_tam_db_s;


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


struct lte_ml1_mgr_x2l_tam_db_s
{
  lte_ml1_schdlr_sf_time_t			  reserve_trm_sfn_cnt;
  trm_time_t                          reserve_sclk;
  boolean                             x2l_mode_enabled;
  lte_ml1_rfmgr_trm_ml1_state_e       ml1_state;
};


#endif /* LTE_ML1_MGR_X2L_TAM_TYPES_H */

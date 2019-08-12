
/*!
  @file
  emm_sim_handler.h

  @brief
  EMM SIM card lib.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_sim_handler.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
12/18/09   RI      Updated with unified EFS and USIM reads.
10/06/09   RI      Fix new line warning at EOF.
05/29/09   zren    Added support for TAU feature
02/03/09   vdr     Added new function prototype emm_save_mobility_info
===========================================================================*/
#ifndef _EMM_SIM_HANDLER_H_
#define _EMM_SIM_HANDLER_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "emm.h"
#include "rex.h"
#include "emm_database.h"
#include "emm_rrc_if.h"
#include "msgr.h"
#include "msgr_lte.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/

#define EMM_USIM_GUTI_OFFSET 0
#define EPSLOCI_LENGTH       18
#define EMM_GUTI_LENGTH      12
#define EMM_UPDATE_STATUS    17

/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/

/*===========================================================================

                        EXTERNAL DEFINITION

===========================================================================*/

extern
boolean emm_load_epsloci_data
( 
  emm_ctrl_data_type *emm_ctrl_data_ptr
) ;

extern
void emm_unload_sim_data
( 
  emm_ctrl_data_type *emm_ctrl_data_ptr
) ;

extern
void emm_invalidate_mobility_info
(
  emm_eps_update_status_type  emm_update_status,
  sys_sim_state_e_type        sim_state,
  emm_ctrl_data_type         *emm_ctrl_data_ptr,
  boolean                     delete_EPLMN_list
) ;

extern
void emm_save_mobility_info
(
  emm_ctrl_data_type         *emm_ctrl_data_ptr
) ;

lte_rrc_imsi_s lte_convert_imsi_to_digits(imsi_data_T imsi);

extern boolean is_tai_invalid
( 
  lte_nas_tai_lst1_type  tai
);

#endif /* _EMM_SIM_HANDLER_H_ */

#endif /*FEATURE_LTE*/


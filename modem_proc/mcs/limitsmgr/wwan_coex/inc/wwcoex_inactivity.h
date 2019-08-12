#ifndef __WWCOEX_INACTIVITY_H__
#define __WWCOEX_INACTIVITY_H__
/*!
  @file wwan_coex_inactivity.h

  @brief
   Support for WWAN Coex inactivity registration/deregistration 
 
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/wwan_coex/inc/wwcoex_inactivity.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/30/13   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "wwan_coex_mgr.h"

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/

/*=============================================================================

                             TYPEDEFS

=============================================================================*/
typedef struct
{
  /*! Start time (in USTMR units) of the activity in the next frame */
  uint32                    start_time;

  /*! End time (in USTMR units) of the activity in the next frame */
  uint32                    end_time;

  /*! Period (USTMR units) with which the activity repeats */
  uint32                    period;

  /*! Type of activity being registered, Uplink/Downlink/Both */
  cxm_tech_link_direction   direction;
}wwcoex_activity_info_type;

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/
/*===========================================================================
FUNCTION wwcoex_init_inactivity_st

DESCRIPTION
  This API will init the wwcoex conflict tables

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_init_inactivity_st(void);

/*===========================================================================
FUNCTION wwcoex_deregister_activity

DESCRIPTION
  This API will deregister tech's activity

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_deregister_activity
(
  cxm_tech_type               tech_id
);

/*===========================================================================
FUNCTION wwcoex_register_activity

DESCRIPTION
  This API will register tech's activity

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_register_activity
(
  cxm_tech_type               tech_id,
  wwcoex_activity_info_type   active_info
);

/*===========================================================================
FUNCTION wwcoex_set_send_flag

DESCRIPTION
  This API will set or clear the send flag for a tech

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_set_send_flag
(
  cxm_tech_type         tech_id,
  boolean               send_flag 
);

#endif /* __WWCOEX_INACTIVITY_H__ */

/*===========================================================================

             GM_MOTION_STATE_BASED_BACKOFF Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM Motion State Based Backoff

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/12   ss   Initial creation of file.
===========================================================================*/
#ifndef GM_MOTION_STATE_BASED_BACKOFF_H
#define GM_MOTION_STATE_BASED_BACKOFF_H

#include "gm_core.h"
#include "gm_ebee.h"

/*===========================================================================

FUNCTION gm_motion_state_based_backoff_predictor_init

DESCRIPTION
  This function enables the predictor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_state_based_backoff_predictor_init(void);

/*===========================================================================

FUNCTION gm_motion_state_based_backoff_handle_pos_event

DESCRIPTION
  This function handles position  event
  
DEPENDENCIES

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
void gm_motion_state_based_backoff_handle_pos_event(sm_gm_cached_pos_fix_type *p_gm_pos_fix);


#endif /*GM_MOTION_STATE_BASED_BACKOFF_H*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

	mcfg_refresh_i.h

GENERAL DESCRIPTION

  MCFG header file for auto refrsh
  
Copyright (c) 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_proc/inc/mcfg_refresh_i.h#1 $
  $DateTime: 2016/12/13 08:00:27 $


  when        who     what, where, why
  ---------   ---     ---------------------------------------------------------------------------
 2014/04/11   cys     Create API file

===========================================================================*/
#ifndef _MCFG_REFRESH_I_H_
#define _MCFG_REFRESH_I_H_

#include "mcfg_task.h"
#include "mcfg_refresh.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
typedef enum mcfg_refresh_state_e {
  MCFG_REFRESH_STATE_MIN = 0,
  MCFG_REFRESH_STATE_IDLE = MCFG_REFRESH_STATE_MIN,
  MCFG_REFRESH_STATE_ACTIVATING_SLOT,
  MCFG_REFRESH_STATE_UPDATING_SLOT,
  MCFG_REFRESH_STATE_WAIT_FOR_SESSION,
  MCFG_REFRESH_STATE_ONHOLD,
  MCFG_REFRESH_STATE_WAIT_CM_INIT,
  MCFG_REFRESH_STATE_TEAR_DOWN_REQ,
  MCFG_REFRESH_STATE_TEAR_DOWN_ACK,
  MCFG_REFRESH_STATE_TEAR_DOWN_NACK,
  MCFG_REFRESH_STATE_ACTIVATING_SUBS,
  MCFG_REFRESH_STATE_UPDATING_SUBS,
  MCFG_REFRESH_STATE_INVALID,
  MCFG_REFRESH_STATE_MAX = MCFG_REFRESH_STATE_INVALID,
} mcfg_refresh_state_e_type;

typedef enum mcfg_refresh_orig_e {
  MCFG_REFRESH_ORIG_INVALID = 0,   
  MCFG_REFRESH_ORIG_UIM,      //Originated by uim hot swap
  MCFG_REFRESH_ORIG_PDC,      //Originated by MCFG PDC activation
  MCFG_REFRESH_ORIG_APP,      //Forced by APP centric refresh
  MCFG_REFRESH_ORIG_PDC_UPDATE_ONLY, //Originated by MCFG PDC activation for MBN update only
  MCFG_REFRESH_ORIG_NOCHANGE, //There's no configuration change
  MCFG_REFRESH_ORIG_FLEX,     //Originated by uim flex mapping
} mcfg_refresh_orig_e_type;


/*==============================================================================

                    PUBLIC FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

void mcfg_refresh_cmd_proc
(
  mcfg_task_cmd_s_type *cmd
);

void mcfg_refresh_init
(
  void
);

mcfg_refresh_state_e_type mcfg_refresh_get_state
(
  mcfg_refresh_index_type sub_index
);

boolean mcfg_refresh_set_state
(
  mcfg_refresh_index_type slot_index, 
  mcfg_refresh_state_e_type state
);

boolean mcfg_refresh_set_orig
(
  mcfg_refresh_index_type sub_index, 
  mcfg_refresh_orig_e_type orig
);

mcfg_refresh_orig_e_type mcfg_refresh_get_orig
(
  mcfg_refresh_index_type sub_index
);

mcfg_error_e_type mcfg_refresh_clients
(
  mcfg_refresh_type_e_type  type,
  mcfg_refresh_index_type   slot
);

mcfg_refresh_index_type mcfg_refresh_sub_to_slot_index
(
  mcfg_refresh_index_type sub_index
);

mcfg_refresh_index_type mcfg_refresh_slot_to_sub_index
(
  mcfg_refresh_index_type slot_index
);

mcfg_error_e_type mcfg_refresh_tear_down
(
  mcfg_refresh_index_type sub_index
);

boolean mcfg_refresh_enabled
(
  mcfg_refresh_index_type slot_index
);

boolean mcfg_refresh_is_ongoing
(
  mcfg_refresh_index_mask_type slot_mask
);

boolean mcfg_refresh_is_ongoing_by_type
(
  mcfg_refresh_index_mask_type slot_mask,
  mcfg_refresh_type_e_type refresh_type
);

void mcfg_refresh_cm_init
(
  void
);

#endif /* _MCFG_REFRESH_I_H_ */
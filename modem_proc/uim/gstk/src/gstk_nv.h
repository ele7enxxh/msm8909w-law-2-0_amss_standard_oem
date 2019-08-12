#ifndef GSTK_NV_H
#define GSTK_NV_H
/*===========================================================================


           G S T K  N V   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_nv.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/07/14   shr     Memory optmization to the GSTK NV logic
07/11/14   shr     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                   ENUMERATED DATA DECLARATIONS

=============================================================================*/

/*===========================================================================

  ENUM: GSTK_NV_INIT_CATEG_ENUM_TYPE

  DESCRIPTION:
    This enum describes the various GSTK NV initalization request categories
==========================================================================*/
typedef enum {
  GSTK_NV_INIT_CATEG_POWER_UP_INIT = 0x0,
    /* GSTK NV initalization due to task/card power-up */
  GSTK_NV_INIT_CATEG_NV_REFRESH,
    /* GSTK NV initalization due to NV Refresh request from MCFG */
  GSTK_NV_INIT_CATEG_NV_REFRESH_NON_STANDARD
    /* GSTK NV initalization due tp NV Refresh request
      from MCFG for a non-standard card */
}
gstk_nv_init_categ_enum_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================
FUNCTION gstk_nv_init_slot_cache

DESCRIPTION
  This function triggers the initalization for the slot specific NV items

PARAMETERS:
  init_categ: [Input] Category of Slot NV cache initalization
  slot_id:    [Input] Slot ID on which initalization is requested

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_nv_init_slot_cache(
  gstk_nv_init_categ_enum_type init_categ,
  gstk_slot_id_enum_type       slot_id
);

/*===========================================================================
FUNCTION gstk_nv_init_sub_cache

DESCRIPTION
  This function triggers the initalization for the subscription specific
  NV items

PARAMETERS:
  init_categ:[Input] Category of Sub NV cache initalization
  slot_id:   [Input] Slot ID on which initalization is requested
  sub_id:    [Input] Subscription ID on which initalization is requested

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_nv_init_sub_cache(
  gstk_nv_init_categ_enum_type init_categ,
  gstk_slot_id_enum_type       slot_id,
  sys_modem_as_id_e_type       sub_id
);

/*===========================================================================
FUNCTION gstk_nv_init_task

DESCRIPTION
  This function triggers the initalization of ME and slot specific NV items during
  GSTK task initalization

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_nv_init_task(void);

/*===========================================================================
FUNCTION gstk_nv_mcfg_process_refresh

DESCRIPTION

  This function processes the NV refresh command from MCFG

PARAMETERS
  cmd_ptr: [Input] Pointer to the command to be processed

RETURN VALUE
  gstk_status_enum_type

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_nv_mcfg_process_refresh(
  gstk_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION gstk_nv_mcfg_refresh_register

DESCRIPTION
  This function registers for NV refresh notification with MCFG

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_nv_mcfg_refresh_register(void);

#endif /* GSTK_NV_H */

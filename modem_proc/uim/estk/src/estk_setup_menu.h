#ifndef ESTK_SETUP_MENU_H
#define ESTK_SETUP_MENU_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  SETUP MENU


GENERAL DESCRIPTION

  This header file contains ESTK functions that handle SETUP MENU command.

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2009 QUALCOMM Technologies, Inc (QTI) and its licensors.  
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_setup_menu.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/09   xz      Initial version (Dual SIM Dual Standby implementation)
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_ESTK
#include "estk.h"

/*===========================================================================

FUNCTION: estk_free_setup_menu_data

DESCRIPTION:
  Free memory allocated for SETUP MENU cmd

PARAMETERS:
  void

DEPENDENCIES:
  cmd_ptr Pointer to the setup menu command to be freed

RETURN VALUE:
  gstk_status_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
void estk_free_setup_menu_data(
  gstk_setup_menu_req_type *cmd_ptr
);

/*===========================================================================

FUNCTION: estk_process_setup_menu_req

DESCRIPTION:
  Called  from GSTK when a SETUP MENU command comes

PARAMETERS:
  void

DEPENDENCIES:
  None

RETURN VALUE:
  gstk_status_enum_type
    GSTK_SUCCESS
    GSTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
estk_result_enum_type estk_process_setup_menu_req(
  const estk_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION: estk_process_menu_sel_req

DESCRIPTION:
  Called  from GSTK when a MENU SELECTION command comes

PARAMETERS:
  void

DEPENDENCIES:
  None

RETURN VALUE:
  gstk_status_enum_type
    GSTK_SUCCESS
    GSTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
estk_result_enum_type estk_process_menu_sel_req(
  const estk_cmd_type *cmd_ptr
);

#endif /*FEATURE_ESTK*/
#endif /*ESTK_SETUP_MENU_H*/

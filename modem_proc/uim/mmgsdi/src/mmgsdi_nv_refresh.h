#ifndef MMGSDI_NV_REFRESH_H
#define MMGSDI_NV_REFRESH_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   N V   R E F R E S H  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used to handle the NV Refresh
  commands.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS



                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and
its licensors. All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_nv_refresh.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/14   hh      Fix compliation warnings
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmgsdi.h"


/*===========================================================================
FUNCTION MMGSDI_PROCESS_CONTINUE_AFTER_NV_REFRESH

DESCRIPTION
  Processing of continue after nv refresh req from MCFG which posts 
  CARD INSERTED evt or SESSION CHANGED evt based on the state
  of the card or state of application.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_continue_after_nv_refresh(
  const mmgsdi_continue_after_nv_refresh_req_type *req_ptr);

/*===========================================================================
FUNCTION MMGSDI_PROCESS_NV_REFRESH_REQ

DESCRIPTION
  This function processes nv refresh req - slot and session nv/efs items
  are refreshed.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_nv_refresh_req(
  const mmgsdi_handle_nv_refresh_req_type *req_ptr);

/*===========================================================================
FUNCTION MMGSDI_NV_REFRESH_MCFG_CB

DESCRIPTION
  Call back registered with MCFG

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_nv_refresh_mcfg_cb(
  mcfg_refresh_info_s_type *mcfg_refresh_info);

#endif /* MMGSDI_NV_REFRESH_H */


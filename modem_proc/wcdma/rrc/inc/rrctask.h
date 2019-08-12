#ifndef RRCTASK_H
#define RRCTASK_H
/*===========================================================================
                        R R C    T A S K  

DESCRIPTION

   This file contains data types and declarations associated with the RRC
   task entry point and maintaining RRC state.
   
      
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001, 2003, 2007-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrctask.h_v   1.6   23 Apr 2001 18:54:42   krishnan  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrctask.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   sks     Made changes to suppress compilation errors for RRC UTF.
05/08/09   ss      Updated Copyright Information
03/01/09   rm      Added new featurization for FEATURE_CMI.
04/23/01   ram     Moved the functionality related to RRC state change to the rrcscmgr
                   module.
01/08/00   ram     Added a wildcard state RRC_STATE_WILDCARD to the rrc_state_e_type
                   structure for the purpose of state change notification to procedures. 
                   Updated copyright.
11/29/00   vn      Changes for REX++.
11/15/00   vn      Incorporated code review comments.
11/07/00   kmp     Added prototype for rrc_init_procedures(). Removed all
                   Task Controller (TC) specific info.
10/27/00   vn      Added additional declarations for RRC state.
10/23/00   vn      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#ifndef FEATURE_CMI
/* -------------------------------------- */
/* PUBLIC API of rrctask.h               */
/* -------------------------------------- */
/*===========================================================================

FUNCTION RRC_TASK

DESCRIPTION
  This function is the entry point for RRC Task. It initializes RRC state
  and individual procedures by calling rrc_init(). The ACK signal is sent back
  to TMC and a wait is done on Task Start Signal from TMC. Once a Task Start
  Signal is received, rrc_dispatcher() is called that takes control now onwards.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  void  rrc_task(dword param);

/*===========================================================================

FUNCTION RRC_INIT

DESCRIPTION
  This function does the initialization for RRC layer. It means setting the 
  state and mode of RRC and initializing all RRC global data. It calls 
  rrc_init_procedures() to initialize each procedure specific data.

  NOTE: We may add NV read here later. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrc_init( void );

#else
/* Public portion of the API exists in wcdmamvsif.h */
#include "wcdmatmcif.h"

#endif /* FEATURE_CMI */

/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

#endif /* RRCTASK_H */


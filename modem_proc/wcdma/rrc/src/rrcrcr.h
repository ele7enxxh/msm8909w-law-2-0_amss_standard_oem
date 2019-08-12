#ifndef RRCRCR_H
#define RRCRCR_H
/*===========================================================================
              R R C R C R  H E A D E R  F I L E

DESCRIPTION
 The RRCRCR module consists of RRC CONNECTION RELEASE PROCEDURE 
 implementation. The 3GPP TS 25.331 RRC Protocol specification is used as
 basis for implementation.
  
Copyright (c) 2001, 2003-2005, 2007-2009  Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rrc/vcs/rrcrcr.h_v   1.0   07 Feb 2001 18:29:54   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcrcr.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
04/07/10   gkg     ASN.1 migration to 8.a.0
03/15/10   as      Updated RRC Code to support ASN.1 8.9 version
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
05/08/09   ss      Updated Copyright Information
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
11/09/05   kc      Added prototype to update release cause var
06/01/04   vk      Declared new fn rrcrcr_is_proc_active() that specifies if
                   RCR is active
02/06/01   rj      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "rrccmd_v.h"

/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/


/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION rrcrcr_init_procedure

DESCRIPTION
  This function will initialize the RRC Connection Release procedure
  substate to RRCRCR_INITIAL Substate. This will be called in the RRCTASK 
  modules along with other Init Procedures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcrcr_init_procedure
(    
  void  
);
/*===========================================================================

FUNCTION rrcrcr_procedure_event_handler

DESCRIPTION
  This is the entry point for RRC Connection Release procedure. All
  events for RCR come to this function from RRC Dispatcher. Depending on the
  UE state of RRC Connection Release Procedure, it calls the corresponding state 
  handler. These state handlers call substate handlers depending on the 
  the current substate of RRC Connection Release procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to the RRC command */
);


/*===========================================================================

FUNCTION rrcrcr_process_690non_critical_ext

DESCRIPTION
  Processes 690 non critical extension of RRC Conn Rel OTA.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_690non_critical_ext
(
  rrc_RRCConnectionRelease_v690NonCriticalExtensions_1 * rcr_msg_ptr
);

/*===========================================================================

FUNCTION rrcrcr_is_proc_active

DESCRIPTION
  Returns rcr sub-state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean rrcrcr_is_proc_active
(
  void
);

/*===========================================================================

FUNCTION rrcrcr_update_release_cause

DESCRIPTION
  This function converts the release cause from ASN1 format into RRC-MM 
  interface format.

DEPENDENCIES
  None

RETURN VALUE
  rrc_rel_cause_e_type: 

SIDE EFFECTS
  None
===========================================================================*/
rrc_rel_cause_e_type rrcrcr_update_release_cause
(
  rrc_ReleaseCause release_cause  
);

#endif /* RRCRCR_H */


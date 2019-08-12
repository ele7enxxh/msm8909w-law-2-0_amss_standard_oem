#ifndef RRCUECE_H
#define RRCUECE_H
/*===========================================================================
                  R R C  U E  C A P A B I L I T Y  E N Q U I R Y

DESCRIPTION
  This header file contains function prototypes for UE Capability Enquiry
  procedure

Copyright (c) 2001, 2003-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcuece.h_v   1.1   14 Sep 2001 21:04:08   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcuece.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/11   rl      Added the macro RRC_MAX_SYS_CAPABILITY.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
05/08/09   ss      Updated Copyright Information
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
11/23/04   sm      Support for Dynamic Power Class feature
09/14/01   rj      Updated templates to function prototypes.
06/05/01   rj      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "rrcasn1.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define RRC_MAX_SYS_CAPABILITY 16


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*===========================================================================

FUNCTION rrcuece_set_radio_capability_update_requirement

DESCRIPTION
  This function reset the variable 'rrc_capability_update_requirement' and
  set 'rrc_capability_update_requirement' in this variable to TRUE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcuece_set_radio_capability_update_requirement (void);

/*===========================================================================

FUNCTION rrcuece_init_procedure

DESCRIPTION
  This function initializes required parameters for UE Capability Enquiry
  procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void rrcuece_init_procedure(void);

/*===========================================================================

FUNCTION rrcuece_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the UE Capability Enquiry procedure.
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (received from RLC) containing UE 
    CAPABILITY ENQUIRY message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void rrcuece_procedure_event_handler(rrc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION rrcuece_copy_capability_update_requirement

DESCRIPTION
  This function copy the Radio Capability Requirement into internal format.
  This stored information will be utilized by any RRC procedure to build 
  UE Capabilities.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcuece_copy_capability_update_requirement
(
  rrc_CapabilityUpdateRequirement capability_update_requirement
                                          /* Capability Update requirement */
);


/*===========================================================================

FUNCTION rrcuece_copy_capability_update_requirement_later_than_r3

DESCRIPTION
  This function copy the Radio Capability Requirement into internal format.
  This stored information will be utilized by any RRC procedure to build 
  UE Capabilities.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcuece_copy_capability_update_requirement_later_than_r3
(
  rrc_CapabilityUpdateRequirement_r5 capability_update_requirement
                                          /* Capability Update requirement */
);

/*===========================================================================

FUNCTION rrcuece_copy_capability_update_requirement_r7

DESCRIPTION
  This function copy the Radio Capability Requirement into internal format.
  This stored information will be utilized by any RRC procedure to build 
  UE Capabilities.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcuece_copy_capability_update_requirement_r7
(
  rrc_CapabilityUpdateRequirement_r7 *capability_update_requirement
                                          /* Capability Update requirement */
);
/*===========================================================================
FUNCTION rrcuece_copy_capability_update_requirement_r8

DESCRIPTION
  This function copy the Radio Capability Requirement into internal format.
  This stored information will be utilized by any RRC procedure to build 
  UE Capabilities.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcuece_copy_capability_update_requirement_r8
(
  rrc_CapabilityUpdateRequirement_r8 *capability_update_requirement
                                          /* Capability Update requirement */
);
#endif /* RRCUECE_H */


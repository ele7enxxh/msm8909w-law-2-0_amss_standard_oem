#ifndef RRCTFCC_H
#define RRCTFCC_H
/*===========================================================================
         R R C  TRANSPORT FORMAT COMBINATION  CONTROL MODEL

DESCRIPTION

  This module is the header module for the Transport Format Combination Control.
  This contains the external interface functions to be used by
  by other RRC modules. This must not be used by modules outside RRC.
  
Copyright (c) 2001, 2004, 2007, 2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrctfcc.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/03/14   sp      Made changes to save procedure substates in crash debug info
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/02/10   su      Provided fix to process TFCC message while another 
                   Reconfiguration is on going.
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
05/08/09   ss      Updated Copyright Information
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7. Removed
                   feature flag FEATURE TFCS CONTROL ENABLE
11/14/07   rm      Added enum for TFCC substates
05/10/01   ttl     Created file.


===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "rrccmd_v.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef enum
{
  RRCTFCC_INITIAL,                   /* Initial Substate                    */
  RRCTFCC_WAIT_FOR_MAC_CFG_CNF,
  RRCTFCC_WAIT_FOR_RECONFIG_CFG_CNF,
  RRCTFCC_MAX_SUBSTATES
}rrctfcc_substates_e_type;
/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION   RRCTFC_PROCEDURE_EVENT_HANDLER

DESCRIPTION
  
  This procedure processes the Transport Format Combination Control 
  message to config the TFC Subset in MAC.

        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
void rrctfc_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);

/*===========================================================================

FUNCTION  RRCTFCC_INIT_PROCEDURE

DESCRIPTION

  This function initializes the TFCC Procedure
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrctfcc_init_procedure
(
  void
);



/*===========================================================================

FUNCTION  RRCTFCC_CLEAR_PROCEDURE

DESCRIPTION

  This function clears the TFCC Procedure
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrctfcc_clear_procedure
(
void
);


/*===========================================================================

FUNCTION   RRCTFCC_PROCESS_MAC_CONFIG_CNF

DESCRIPTION
  This function process the received MAC CFG CNF
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
void rrctfcc_process_mac_config_cnf
(
rrc_cmd_type * cmd_ptr
);

/*===========================================================================

FUNCTION  rrctfcc_is_reconfig_in_progress

DESCRIPTION

  This function will check if TFCC Procedure substate is reconfig in progress
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE: if TFCC procedure is wating for reconfig in progress
  FALSE: othewise.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrctfcc_is_reconfig_in_progress
(
  void
);
/*===========================================================================

FUNCTION  rrctfcc_get_current_substate

DESCRIPTION

  This function will return the current substate
    
DEPENDENCIES

  None.
 
RETURN VALUE
rrctfcc_substates_e_type


SIDE EFFECTS

  None.

===========================================================================*/

rrctfcc_substates_e_type rrctfcc_get_current_substate(void);

#endif /* RRCTCRC_H */


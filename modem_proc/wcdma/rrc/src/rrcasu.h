#ifndef RRCASU_H
#define RRCASU_H
/*===========================================================================
         R R C   A C T I V E    S E T    U P D A T E    M O D U L E 

DESCRIPTION

  This module is the header module for the Active Set Update module.
  This contains the external interface functions to be used by
  by other RRC modules. This must not be used by modules outside RRC.
  
Copyright (c) 2001,2003,2007,2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/msm5200/rrc/vcs/rrcasu.h_v   1.0   30 Aug 2001 16:57:20   krishnan  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcasu.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/14   sp      Made changes for Mav debug snapshot
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
05/08/09   ss      Updated Copyright Information
08/14/01   ram     Created file.


===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "rrccmd_v.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
  
/* Enumerated type of the different substates for this procedure           */
typedef enum
{
  RRCASU_INITIAL,                   /* Initial Substate                    */
  RRCASU_WAIT_FOR_CHAN_CFG_CNF,     /* Waiting for confirmation of channel  
                                    config from LLC                        */                      
  RRCASU_MAX_SUBSTATES
}rrcasu_substates_e_type;

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

FUNCTION  RRCASU_INIT_PROCEDURE

DESCRIPTION

  This function initializes the ASU procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcasu_init_procedure( void );




/*===========================================================================

FUNCTION RRCASU_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Active Set Update procedure. This function is called by the
  dispatcher whenever an event is received for this module.  
  
DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
  
===========================================================================*/
void rrcasu_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);

/*===========================================================================

FUNCTION  rrcasu_return_curr_substate

DESCRIPTION
returns curr substate    
DEPENDENCIES

  None.
 
RETURN VALUE
  rrcasu_substates_e_type


SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif

rrcasu_substates_e_type rrcasu_return_curr_substate(void);

#endif /* RRCASU_H */


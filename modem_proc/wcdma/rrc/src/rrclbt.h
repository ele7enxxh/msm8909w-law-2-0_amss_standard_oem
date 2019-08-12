#ifndef RRCLBT_H
#define RRCLBT_H
/*===========================================================================
         R R C   L O O P B A C K   T E S T   M O D U L E 

DESCRIPTION

  This module is the header module for the loopback test module.
  This contains the external interface functions to be used by
  by other RRC modules. This must not be used by modules outside RRC.
  
Copyright (c) 2001-2003, 2007-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrclbt.h_v   1.0   05 Feb 2002 13:59:42   xgao  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrclbt.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
05/08/09   ss      Updated Copyright Information
05/02/08   da      Added API rrclbt_get_lb_mode_and_dummy_sig_enabled() which 
                   returns the loopback mode status
02/03/03   kc      Removed Lint errors
01/31/02   xfg     Created file.


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
  RRCLBT_INITIAL,                   /* Initial Substate                    */
  RRCLBT_WAIT_FOR_CHAN_CFG_CNF,     /* Waiting for confirmation of channel  
                                       config from LLC                     */                      
  RRCLBT_LB_IN_PROGRESS,            /* LB Test is in progress              */
  RRCLBT_MAX_SUBSTATES
}rrclbt_substates_e_type;
/* Below #defines are added for logging purpose only */  
#define rrclbt_substates_e_type_value0 RRCLBT_INITIAL, 				  
#define rrclbt_substates_e_type_value1 RRCLBT_WAIT_FOR_CHAN_CFG_CNF  			 
#define rrclbt_substates_e_type_value2 RRCLBT_LB_IN_PROGRESS	  
#define rrclbt_substates_e_type_value3 RRCLBT_MAX_SUBSTATES

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

FUNCTION  RRCLBT_INIT_PROCEDURE

DESCRIPTION

  This function initializes the Loopback Test module.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrclbt_init_procedure( void );




/*===========================================================================

FUNCTION RRCLBT_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Loopback test module. This function is called by the
  dispatcher whenever an event is received for this module.  
  
DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
  
===========================================================================*/
void rrclbt_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);



/*===========================================================================

FUNCTION  RRCLBT_IS_LB_TEST

DESCRIPTION

  This function returns True if RRC is in the loopback test mode, otherwise,
  returns False.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrclbt_is_lb_on( void );


/*===========================================================================

FUNCTION  RRCLBT_GET_LB_MODE_AND_DUMMY_SIG_ENABLED

DESCRIPTION

  This functions returns TRUE and the LB mode and signalling enabled status.
  If LB is not active, returns false
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrclbt_get_lb_mode_and_dummy_sig_enabled( rrc_lb_test_mode_e_type *lb_mode,
                                                  boolean  *dummy_signaling_enabled);

#endif /* RRCLBT_H */


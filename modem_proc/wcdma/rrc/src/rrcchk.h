#ifndef RRCCHK_H
#define RRCCHK_H
/*========================================================================================

                                    R R C C H K

DESCRIPTION

  This file contains the declaration of the interface functions and
  data types used by RRC

  Copyright (c) 2005, 2007-2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*===========================================================================


                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcchk.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
  
 when       who     what, where, why
--------    ---     ----------------------------------------------------------
12/31/10   rl       Merged with VU_MODEM_WCDMA_RRC.01.86.50
05/08/09   ss       Updated Copyright Information
10/19/05   vr       Created the file to support Counter Check Procedure 

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

FUNCTION rrcchk_procedure_event_handler

DESCRIPTION
  This function is the entry point of rrcchk

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcchk_procedure_event_handler
( 
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);  

/*===========================================================================

FUNCTION RRCCHK_INIT_PROCEDURE

DESCRIPTION
  This function will register for the necessary state change indication for
  Counter Check procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcchk_init_procedure
(    
  void  
);

#endif



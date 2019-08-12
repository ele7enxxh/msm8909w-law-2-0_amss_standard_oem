#ifndef RRCBMC_H
#define RRCBMC_H
/*========================================================================================

                                    R R C B M C 

DESCRIPTION
  This file contains the declaration of the interface functions and
  data types used by RRC and LSM

  Copyright (c) 2005, 2007, 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcbmc.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $  
  
 when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/18/14   vi      Added missing changes under feature TRIPLE_SIM
01/11/12    sn     Changes to send ETWS security info to upper layers 
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/22/10   su      Made code changes to support FEATURE_WCDMA_ETWS.
05/08/09   ss      Updated Copyright Information
15/07/05   da      Moved prototype of rrcbmc_get_lac_id() to rrcdata.h.
                   Changed name to rrc_get_lac_id().
06/06/05   da      Added prototype for rrcbmcfunc get_lac_id
04/13/05   da      Putting #includes outside FEATURE UMTS BMC to take care
                   of compiler warning when feature is undef 
04/06/05   da      Added Initial support for BMC.  Prototype definitions for 
                   rrcbmc_send_rrc_new_cell_ind_message(), 
                   rrcbmc_procedure_event_handler(), rrcbmc_init_procedure()
                   

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/
/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "wcdma_variation.h"
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

FUNCTION rrcbmc_send_new_cell_ind

DESCRIPTION
  This function sends RRC_NEW_CELL_IND primitive to BMC 

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcbmc_send_rrc_new_cell_ind_message(void); 
 

#ifdef FEATURE_TRIPLE_SIM
/*===========================================================================

FUNCTION rrcbmc_send_cell_change_on_wcdma_stop

DESCRIPTION
  This function sends BMC not supported indication during WCDMA stop

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcbmc_send_cell_change_on_wcdma_stop
(    
  void  
);
#endif

#ifdef FEATURE_WCDMA_ETWS

/*============================================================================
FUNCTION: rrcbmc_send_etws_information_ind_message

DESCRIPTION:
This function is responsible for filling up data structures
for etws information sent  to BMC .
This also allocates cmd buffer and sends the command.


DEPENDENCIES:
  None.

RETURN VALUE:
SUCCESS: Command was successfully sent to BMC.
FAILURE: Command could not be sent due to any one of multiple
             reasons.


SIDE EFFECTS:
=============================================================================*/

void rrcbmc_send_etws_information_ind_message
(
  rrc_ETWS_Information etws_information,
  rrc_ETWS_WarningSecurityInfo *etws_WarningSecurityInfo
);

#endif




/*===========================================================================

FUNCTION rrcbmc_procedure_event_handler

DESCRIPTION
  This function is the entry point of rrcbmc 

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcbmc_procedure_event_handler
( 
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);  

/*===========================================================================

FUNCTION RRCBMC_INIT_PROCEDURE

DESCRIPTION
  This function will register for the necessary state change indication for
  bmc procedure, and SIB 5/6 change notification

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcbmc_init_procedure
(    
  void  
);


#endif /* RRCBMC_H */


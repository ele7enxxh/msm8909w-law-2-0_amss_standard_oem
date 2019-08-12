/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      R F   C U S T O M E R   D R I V E R

GENERAL DESCRIPTION
  This module contains all customer-specific functions.  These functions
  are called within the GSM RF library.  These functions give the customers
  the ability to overwrite certain configurations or settings set by the
  RF driver.

EXTERNALIZED FUNCTIONS
  rfgsmcust_init();

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 by Qualcomm Technologies Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsmcust.c#1 $ 
$DateTime: 2016/12/13 07:59:51 $ 
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/10   lcl    Changed MSG_LVL to MSG_LEGACY to avoid too many F3 messages
12/22/09   sr      changed the debug msg macro from legacy to new one.
09/13/02   thh     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "msg.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================

                                 CONSTANTS

===========================================================================*/


/*===========================================================================

                                  MACROS

===========================================================================*/



/*===========================================================================

                                   DATA

===========================================================================*/


/*===========================================================================

                            FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION rfgsmcust_init

DESCRIPTION
  This functions initializes the RF HW.

DEPENDENCIES
  This function is invoked after the GSM RF library has finished with its own
  initializations.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rfgsmcust_init( void )
{
/*-------------------------------------------------------------------------*/

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfgsmcust_init()", 0);

  /* Customer's initialization codes followed here.
  */

} /* end of rfgsmcust_init */

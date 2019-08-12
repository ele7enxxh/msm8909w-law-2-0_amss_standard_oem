#ifndef PS_INBOUND_ACL_H
#define PS_INBOUND_ACL_H
/*===========================================================================

                   P R O T O C O L   S E R V I C E S

       A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S

            F O R   T H E   I N B O U N D   I P   F I L T E R

GENERAL DESCRIPTION
  The PS inbound filter is used to protect the mobile device
  from malicious traffic.
  This filter is application specific and should be re-evaluated
  for each model.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/02    om     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "ps_aclrules.h"

/* <EJECT> */ 
/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/

ACL_DEF( ps_inbound_acl );
#endif    /* PS_INBOUND_ACL_H */


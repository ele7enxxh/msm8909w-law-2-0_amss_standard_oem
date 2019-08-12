#ifndef MCCSYOBJ_H
#define MCCSYOBJ_H
/*===========================================================================

             P A C K E T   Z O N E   I D   S U P P O R T

GENERAL DESCRIPTION
  This module contains supporting functions for packet zone ID.

DESCRIPTION
  This module contains definitions and interfaces for service configuration
  and negotiation for Data Services service options.  This functions are
  created for support of MDR Release 1.0.  Note that once Call Manager is
  implemented, these function may become obsolete.

  Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccsyobj.h_v   1.2.1.0   30 Nov 2001 17:21:46   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/mccsyobj.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/09   jj      Meta-Comments cleanup for API's exported to DS.
08/31/09   adw     Removed unnecessary customer.h and target.h includes.
05/14/09   jj      Split this file as part of CMI phase2.
05/15/08   bb      Code changes for IS707B addendum feature
08/16/06   pg      Added support for release D Packet Zone Hysteresis.
06/23/06   pg      Added support for RPC and clean up.
03/31/05   fc      Added support for EPZID.
10/23/03   yll     Added support for updating HDR PANID.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
01/24/03   lh      Modified pzid change processing interface.
10/01/01   kk      Merge from common archive
                   09/14/01   lcc     Added reg_change_handler member in nid 
                   of mccsyobj_system_object_type.
02/27/01   kk      Added functions to set and get old system parameters where
                   the last packet data call is successfully originated.
11/13/00   lcc     Added members related to NID in mccsyobj_system_object_type.
11/03/99   lcc     Merged from GD4021:
           doj     Made reg_prev_change_handler, process_prev and get_prev
                   only defined for FEATURE_SPECIAL_MDR.
02/18/99   lcc     Corrected SID to use a word instead of byte.
01/25/99   lcc     Initial release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"



/*===========================================================================

                      SOME CONSTANTS USED IN THIS FILE

===========================================================================*/

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/* Function to get current stored value of PZID */
/*===========================================================================

FUNCTION mccsyobj_get_pzid

DESCRIPTION
  This function gets current stored value of PZID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
byte mccsyobj_get_pzid( void );

/* Function to get current stored value of SID */
/*===========================================================================

FUNCTION mccsyobj_get_sid

DESCRIPTION
  This function gets current stored value of SID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccsyobj_get_sid( void );

/* Function to get current stored value of NID */
/*===========================================================================

FUNCTION mccsyobj_get_nid

DESCRIPTION
  This function gets current stored value of NID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccsyobj_get_nid( void );

    /* Registers handler for changing EPZID */ 
/*===========================================================================

FUNCTION mccsyobj_reg_epzid_change_handler

DESCRIPTION
  This function registers handler for changing EPZID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef void (*epzid_change_handler_type) (word sid, word nid, byte pzid);

void mccsyobj_reg_epzid_change_handler
(
  epzid_change_handler_type epzid_change_handler 
);



   /* Registers handler to know when mobile enters into the registration sub state */ 
/*===========================================================================

FUNCTION mccsyobj_epzid_reg_sub_state_handler 

DESCRIPTION
  This function registers the handler for knowing when mobile enters into the registration 
  sub state due to either "timer based" or "parameter based" or "distance based" and 
  "UZ based" registration types

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
typedef boolean (*epzid_reg_change_handler_type) (word sid, word nid, byte pzid);

void mccsyobj_epzid_reg_sub_state_handler
(
  epzid_reg_change_handler_type epzid_reg_change_handler 
);


/* Function to get current stored value of EPZID (SID, NID and PZID) */
/*===========================================================================

FUNCTION mccsyobj_get_epzid

DESCRIPTION
  This function gets current stored value of EPZID (SID, NID and PZID).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsyobj_get_epzid
( 
  word *sid, 
  word *nid, 
  byte *pzid 
);



#endif /* MCCSYOBJ_H */


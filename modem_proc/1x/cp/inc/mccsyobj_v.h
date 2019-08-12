#ifndef MCCSYOBJ_V_H
#define MCCSYOBJ_V_H
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccsyobj_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/29/12   ssh     1xCSFB: Updated the EPZID value based on the received
                   SIB8/C2K parameter message.
02/04/12   ppr     Feature Cleanup
08/10/11   ssh     CW fixes.
05/14/09   jj      Split the file as part of CMI phase-II
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

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "caii.h"
#include "mccsyobj.h"


/*===========================================================================

                      SOME CONSTANTS USED IN THIS FILE

===========================================================================*/

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/

#define MCCSYOBJ_PZID_UNKNOWN 0


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FEATURE_IS707B_ADDENDUM
    /* Function to call when mobile enters the Registration sub state */
/*===========================================================================

FUNCTION mccsyobj_call_reg_change_handler

DESCRIPTION
  Function to call when mobile station enters into the registration sub state due to
  either "timer based" or "parameter based" or "distance based" and "UZ based" registration
  types.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  -  Packet Zone Hysteresis timer is active for current EPZID parameters.
  FALSE -  Either Packet Zone Hysteresis timer is active for current EPZID parameters or Data module
           is not registsred call back method.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsyobj_call_reg_change_handler (void);
#endif /* FEATURE_IS707B_ADDENDUM */

#ifdef FEATURE_IS2000_REL_A
/* Packet Data related system parameters */

/* Function to get old pzid */
/*===========================================================================

FUNCTION mccsyobj_get_old_pd_orig_pzid

DESCRIPTION
  This function get old pzid.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
byte mccsyobj_get_old_pd_orig_pzid( void );

/* Function to set old pzid from the current one */
/*===========================================================================

FUNCTION mccsyobj_set_old_pd_orig_pzid

DESCRIPTION
  This function sets old pzid from the current onet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsyobj_set_old_pd_orig_pzid( byte pzid );

/* Function to get old sid */
/*===========================================================================

FUNCTION mccsyobj_get_old_pd_orig_sid

DESCRIPTION
  This function gets old sid.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccsyobj_get_old_pd_orig_sid( void );

/* Function to set old sid from the current one */
/*===========================================================================

FUNCTION mccsyobj_set_old_pd_orig_sid

DESCRIPTION
  This function sets old sid from the current one.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsyobj_set_old_pd_orig_sid( word sid );

/* Function to get old nid */
/*===========================================================================

FUNCTION mccsyobj_get_old_pd_orig_nid

DESCRIPTION
  This function gets old nid.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccsyobj_get_old_pd_orig_nid( void );

/* Function to set old nid from the current one */
/*===========================================================================

FUNCTION mccsyobj_set_old_pd_orig_nid

DESCRIPTION
  This function sets old nid from the current one.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsyobj_set_old_pd_orig_nid( word nid );

#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION mccsyobj_initialize

DESCRIPTION
  This function initializes the mccsyobj_system object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyobj_initialize
(
  void
);

/*===========================================================================

FUNCTION mccsyobj_process_epzid

DESCRIPTION
  Function to call when EPZID (i.e. SID, NID and PZID) are received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsyobj_process_epzid
(
  word sid,
  word nid,
  byte pzid,
  word curr_state
);

/*===========================================================================

FUNCTION MCCSYOBJ_GET_SID

DESCRIPTION
  This function gets the current stored value of SID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccsyobj_get_sid(void);

/*===========================================================================

FUNCTION mccsyobj_get_nid

DESCRIPTION
  This function gets the current stored value of NID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccsyobj_get_nid(void);

/*===========================================================================

FUNCTION MCCSYOBJ_UPDATE_EPZID

DESCRIPTION
  This function checks if the SID or NID or PZID has changed.
  If so, the corresponding values will be updated and if the change_handler
  is registered, it will be invoked.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyobj_update_epzid
(
  boolean sid_inc,
  word sid,
  boolean nid_inc,
  word nid,
  boolean pzid_inc,
  byte pzid,
  word curr_state
);

#endif /* MCCSYOBJ_V_H */


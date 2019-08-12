#ifndef DS707_RRM_H
#define DS707_RRM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 
                         D S 7 0 7 _ R R M

GENERAL DESCRIPTION
  This file contains functions that are used to handle resource request msgs
  in IS2000 systems.  Currently, it manages the SCRM and Control/Hold 
  modules.
  
EXTERNALIZED FUNCTIONS
  DS707_RRM_BUILD_BLOB
    Registered with MC at startup. If DS wants MC to send an RRM (either a
    SCRM or request to come out of control/hold), then this is called
    by MC.  This in turn calls either the SCRM blob function or the 
    control hold blob function
  
  DS707_RRM_CALL_INIT
    Called at beginning of every call.  Initializes state variables in
    this module as well as in SCRM and Control/Hold.
    
  DS707_RRM_REG_FUNCS
    Called at startup.  Registers BLOB function with MC.  Also has 
    SCRM and Control/Hold modules register themselves with MC.
    
  DS707_RRM_RETRY_DELAY_CBACK  
    Called whenever a retry delay is received.  Can be for SCRM, Control/
    Hold, or for pkt re-originations.  Parses message and calls appropos
    function in sub-module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_rrm_reg_funcs() at startup.

  Must call ds707_rrm_call_init() every time a SO 33 call starts.
  
 Copyright (c) 2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_rrm.h_v   1.1   19 Nov 2002 18:57:20   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_rrm.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
11/17/02   ak         Updated the file header comments.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "mccds.h"
#include "mccdma.h"

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/
extern boolean ds707_rrm_awaiting_mc;


/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION     DS707_RRM_BUILD_BLOB.

DESCRIPTION  Called by MC at our request, to either build a SCRM or build an
             RRM.

DEPENDENCIES None.

RETURN VALUE None

SIDE EFFECTS None.
===========================================================================*/
mccds_resource_req_type ds707_rrm_build_blob
(
#ifdef FEATURE_IS2000_R_SCH
  uint8 *blob_size,   /* eventual size of the blob                         */
  uint8 *blob_ptr,    /* points to BLOB that is to be filled out           */
  word   so           /* SCRM is for this active service option            */
#else
  void
#endif
);

/*===========================================================================
FUNCTION     DS707_RRM_CALL_INIT

DESCRIPTION  Clears SCRM and RRM call state at beginning of call.

DEPENDENCIES None.

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
void ds707_rrm_call_init(void);

/*===========================================================================
FUNCTION     DS707_RRM_REG_FUNCS

DESCRIPTION  Register functions with MC which are used for RRM and SCRM.

DEPENDENCIES Assumes MC has already started.  Called only at powerup.

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
void ds707_rrm_reg_funcs(void);

/*===========================================================================
FUNCTION DS707_RRM_RETRY_DELAY_CBACK

DESCRIPTION
  Called from MC whenever MC receives a message from the base station
  indicating what the retry callback should be. 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rrm_retry_delay_cback
(
  retry_delay_type retry_parm
);
#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_RRM_H        */

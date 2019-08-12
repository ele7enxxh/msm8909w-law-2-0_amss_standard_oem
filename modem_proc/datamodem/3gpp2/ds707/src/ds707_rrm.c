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

 Copyright (c) 2001 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 Copyright (c) 2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_rrm.c_v   1.2   21 Jan 2003 22:25:20   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_rrm.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
01/21/03   ak         Autonomous retry delays for pkt orig's are no longer
                      ignored.
11/17/02   ak         Updated the file header comments.
09/25/01   ak         mccsch callback struct has changed.
08/09/01   ak         Changed DEBUG level to MSG_HIGH.
05/11/01   ak         Initial revision.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_rrm.h"
#include "ds707_chs.h"
#include "ds707_scrm.h"
#include "ds707_so_retrydel.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "mccds.h"
#include "mccsch.h"


/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
  Message has been sent to MC asking for either a SCRM or an RRM.
---------------------------------------------------------------------------*/
boolean ds707_rrm_awaiting_mc = FALSE;

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
)
{
  mccds_resource_req_type ret_val = MCCDS_RES_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_IS2000_R_SCH
  /*-------------------------------------------------------------------------
    See if want to send a SCRM first.  Always give priority to SCRM over
    RRM
  -------------------------------------------------------------------------*/
  if (ds707_scrm_build_blob(blob_size,blob_ptr,so) == TRUE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Sending SCRM to MC");
    ret_val = MCCDS_RES_SCRM;
  }
#endif

#ifdef FEATURE_IS2000_CHS
  if (ret_val == MCCDS_RES_NONE)
  {
    /*-------------------------------------------------------------------------
      Could not/did not want to build a SCRM.  So see if can build an RRM.
    -------------------------------------------------------------------------*/
    if (ds707_chs_build_blob() == TRUE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Sending RRM to MC");
      ret_val = MCCDS_RES_RRM;
    }
  }
#endif /* FEATURE_IS2000_CHS */

  if (ret_val == MCCDS_RES_NONE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Sending nothing to MC");
  }

  ds707_rrm_awaiting_mc = FALSE;
  return(ret_val);
} /* ds707_rrm_build_blob() */


/*===========================================================================
FUNCTION     DS707_RRM_CALL_INIT

DESCRIPTION  Clears SCRM and RRM call state at beginning of call.

DEPENDENCIES None.

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
void ds707_rrm_call_init(void)
{
  ds707_rrm_awaiting_mc = FALSE;

#ifdef FEATURE_IS2000_CHS
  ds707_chs_call_init();
#endif

#ifdef FEATURE_IS2000_R_SCH
  ds707_scrm_call_init();
#endif
} /* ds707_rrm_call_init() */


/*===========================================================================
FUNCTION     DS707_RRM_REG_FUNCS

DESCRIPTION  Register functions with MC which are used for RRM and SCRM.

DEPENDENCIES Assumes MC has already started.  Called only at powerup.

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
void ds707_rrm_reg_funcs(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  reg_retry_delay(ds707_rrm_retry_delay_cback);
  mccds_reg_resource_generator(ds707_rrm_build_blob);

#ifdef FEATURE_IS2000_R_SCH
  ds707_scrm_reg_funcs();
#endif

#ifdef FEATURE_IS2000_CHS
  ds707_chs_reg_funcs();
#endif
} /* ds707_rrm_reg_funcs() */


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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(retry_parm.retry_type)
  {
    case CAI_RETRY_SCRM:
#ifdef FEATURE_IS2000_R_SCH
      ds707_scrm_retry_delay(retry_parm);
#endif /* FEATURE_IS2000_R_SCH */
      break;

    case CAI_RETRY_ORIG:
      ds707_so_retrydel_set_delay(retry_parm.infinite_delay,
                                  retry_parm.retry_delay_duration);
      break;

    case CAI_RETRY_CLR_ALL:
      ds707_so_retrydel_clr_delay();

#ifdef FEATURE_IS2000_R_SCH
      ds707_scrm_retry_processing();
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_CHS
      ds707_chs_retry_processing();
#endif /* FEATURE_IS2000_CHS */
      break;

    case CAI_RETRY_RESA:
#ifdef FEATURE_IS2000_CHS
      ds707_chs_retry_delay(retry_parm);
#endif /* FEATURE_IS2000_CHS */
      break;

    default:
      break;
  }
} /* ds707_rrm_retry_delay_cback */

#endif /* FEATURE_DATA_IS707 */

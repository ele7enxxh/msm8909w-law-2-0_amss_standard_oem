 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                  M A I N   C O N T R O L   T A S K
                  S R I D   M A N A G E R

GENERAL DESCRIPTION
  This module is responsible for managing the SRIDs.

EXTERNALIZED FUNCTIONS

  Copyright (c) 2001 - 2016 Qualcomm Technologies, Inc. 
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

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccsrid.c_v   1.8   28 Jun 2002 12:50:26   lhang  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccsrid.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   ppr     Chgs to reduce F3 logging 
12/28/12   gga     Klocwork Reviews
05/24/12   ppr     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
02/15/11   ag      Replaced last remaining REX_ENABLE_PREMPTION with
                   REX_ISR_UNLOCK.
02/14/11   ag      Used REX_ISR_LOCK and REX_ISR_UNLOCK for protecting shared
                   data from simultaneous access by multiple tasks/ISRs.
01/27/11   azh     Lint and KW fixes.
07/12/10   jj      Replaced CP's QDSP6 feature with an umbrella feature.
05/14/09   jj      CMI Phase-II update.
03/03/09   jj      Featurized Q6 porting.
12/10/08   jj      QDSP6 migration porting effort, as a part of which replaced
                   INTLOCK and INTFREE with REX_ENABLE_PREMPTION and
                   REX_DISABLE_PREMPTION.
05/11/07   pg      Lint Cleanup.
11/21/06   sb      Added support for VOIP-1X handoffs.
05/19/06   fc      Fixed bogus error message.
03/28/05   fc      Made FCS also available to Rel D.
12/10/04   sb      Fixed lint warnings.
05/19/04   sb      Made following merge official via integrate.
05/14/04   sb      Merged the following from msmshared:
                   03/05/04   bkm     Lint cleanup.
03/12/04   fc      Added support for Release C fast call setup.
03/05/04   bkm     Lint cleanup.
04/26/02   lh      Code review comments added.
04/04/02   lh      Fixed a INTLOCK/FREE bug, and use INTLOCK/FREE_SAV.
02/08/02   lh      Free srid only changes owner to NUll if that srid is active.
02/07/02   lh      Added mccsrid_free_all_owned_srids().
01/08/02   lh      Function name change.
08/28/01   lh      Bug fixes.
05/14/00   lh      Initial revision of the file.
===========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "modem_1x_defs.h"

#include "msg.h"
#include "m1x_diag.h"
#include "err.h"

#include "mc_i.h"
#include "mccsrid_v.h"

#ifdef FEATURE_IS2000_REL_A_SVD

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/* Enum for the state of an SR_ID */
typedef enum
{
  SRID_AVAILABLE,       /* SRID is available for allocation */
  SRID_USED,            /* SRID is allocated */
  SRID_ACTIVE           /* SRID is used by an active service connection */
} mccsrid_srid_state_type;


/* Data Type for SR_ID objects */
typedef struct
{
  mccsrid_srid_state_type state;   /* The state of the SRID object */
  mccsrid_srid_owner_type owner;   /* The owner of the SRID object */
} mccsrid_srid_type;


/* SRID objects array */
mccsrid_srid_type sr_id[ MCCSRID_SRID_NUM_MAX ];  /* 7 items to begin with */


/*===========================================================================

FUNCTION mccsrid_initialize

DESCRIPTION
  Performs necessary initialization for the SR_ID objects.  This function
  is called upon power up initialization.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  This function erase all the SR_ID information. Caller needs to make sure
  that it is safe to call this function.
===========================================================================*/
void mccsrid_initialize(void)
{
  byte i;    /* sr_id array index */

  /*
  ** SR_ID 0 is reserved for signaling. Set it to USED so we don't allocate
  ** it by mistake. And set the owner to reserved so that it should be
  ** always be allocated.
  */
  sr_id[0].state = SRID_USED;
  sr_id[0].owner = SRID_OWNER_RESERVED;

  /* Initialize the rest of the SR_ID array. */
  for (i=1; i<MCCSRID_SRID_NUM_MAX; i++)
  {
    sr_id[i].owner = SRID_OWNER_NULL;
    sr_id[i].state = SRID_AVAILABLE;
  }
} /* mccsrid_initialize */

/*===========================================================================

FUNCTION mccsrid_print

DESCRIPTION
  Prints all the SR_ID to screen for diagnostic purpose.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  This function is not protected by INTLOCK/REX_ENABLE_PREMPTION.

===========================================================================*/
void mccsrid_print(void)
{
  byte i;    /* sr_id array index */

  #ifdef T_WINNT
  #error code not present
#else
  i = 1; /* Initialize the loop var value */

  M1X_MSG( DCP, LEGACY_MED,
      "(sr_id: owner, state) (%d: %d, %d), (%d: %d, %d), (%d: %d, %d)",
      i, sr_id[i].owner, sr_id[i].state,
      i+1, sr_id[i+1].owner, sr_id[i+1].state,
      i+2, sr_id[i+2].owner, sr_id[i+2].state);

  M1X_MSG( DCP, LEGACY_MED,
      "(sr_id: owner, state) (%d: %d, %d), (%d: %d, %d), (%d: %d, %d)",
      i+3, sr_id[i+3].owner, sr_id[i+3].state,
      i+4, sr_id[i+4].owner, sr_id[i+4].state,
      i+5, sr_id[i+5].owner, sr_id[i+5].state);

  #endif /* T_WINNT */
} /* mccsrid_print */

/*===========================================================================

FUNCTION mccsrid_available_srid

DESCRIPTION
  This function returns the lowest unused SR_ID number to the caller.  But
  it does not allocate the sr_id.

DEPENDENCIES
  None

RETURN VALUE
  SR_ID number [1, MCCSRID_SRID_NUM_MAX-1], if a SR_ID is available.
  Returns MCCSRID_INVALID_SRID if none of the SRID is available.

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
uint8 mccsrid_available_srid
(
 void
)
{
  uint8    i;           /* index */
  uint8    srid = MCCSRID_INVALID_SRID;     /* return value */
  boolean found = FALSE;   /* flag used to stop the loop */

  for (i=1; i<MCCSRID_SRID_NUM_MAX && !found; i++)
  {
    /* -----------------------------------------------------------------
    ** This function can be called from multiple task. Protect the
    ** critical section using REX_ISR_LOCK
    ** ----------------------------------------------------------------- */

    REX_ISR_LOCK(&mc_crit_sect);

    if (sr_id[i].state == SRID_AVAILABLE)
    {
      found = TRUE;
      srid = i;
    }

    REX_ISR_UNLOCK(&mc_crit_sect);

  }

  if (!found)
  {
    /* There must be an SR_ID leak */
    srid = MCCSRID_INVALID_SRID;
    M1X_MSG( DCP, LEGACY_ERROR,
      "Out of available sr_id!");
  }

  return srid;
} /* mccsrid_available_srid */

/*===========================================================================

FUNCTION mccsrid_allocate_srid

DESCRIPTION
  This function returns the lowest unused SR_ID number to the caller.

DEPENDENCIES
  None

RETURN VALUE
  SR_ID number [1, MCCSRID_SRID_NUM_MAX-1], if a SR_ID is available.
  Returns MCCSRID_INVALID_SRID if none of the SRID is available.

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
byte mccsrid_allocate_srid
(
  mccsrid_srid_owner_type owner
    /* Owner ID of the allocation request */
)
{
  byte    i;           /* index */
  byte    srid = MCCSRID_INVALID_SRID;     /* return value */
  boolean found = FALSE;   /* flag used to stop the loop */

  for (i=1; i<MCCSRID_SRID_NUM_MAX && !found; i++)
  {
    /* -----------------------------------------------------------------
    ** This function can be called from multiple task. Protect the
    ** critical section using REX_ISR_LOCK
    ** ----------------------------------------------------------------- */
    REX_ISR_LOCK(&mc_crit_sect);

    if(sr_id[i].state == SRID_AVAILABLE)
    {
      sr_id[i].state = SRID_USED;       /* Set state to USED */
      sr_id[i].owner = owner;           /* Set the owner */
      found = TRUE;
      srid = i;
    }

    REX_ISR_UNLOCK(&mc_crit_sect);

  }

  if(!found)
  {
    /* There must be an SR_ID leak */
    srid = MCCSRID_INVALID_SRID;
    M1X_MSG( DCP, LEGACY_ERROR,
      "Out of available sr_id!");
  }

  mccsrid_print();

  return srid;
} /* mccsrid_allocate_srid */

#if defined(FEATURE_IS2000_REL_C)
/*===========================================================================

FUNCTION MCCSRID_ALLOCATE_SPECIFIC_SRID

DESCRIPTION
  This function marks specified SR_ID as used if it is available

DEPENDENCIES
  None

RETURN VALUE
  Specified SR_ID number if it is available.
  Otherwise, returns MCCSRID_INVALID_SRID.

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.

===========================================================================*/
byte mccsrid_allocate_specific_srid
(
  byte id,
    /* SR_ID number */
  mccsrid_srid_owner_type owner
    /* Owner ID of the allocation request */
)
{
  byte    srid = MCCSRID_INVALID_SRID;     /* return value */

  if ((id > 0) && (id < MCCSRID_SRID_NUM_MAX))
  {
    /* -----------------------------------------------------------------
    ** This function can be called from multiple task. Protect the
    ** critical section using REX_ISR_LOCK
    ** ----------------------------------------------------------------- */

    REX_ISR_LOCK(&mc_crit_sect);

    if (sr_id[id].state == SRID_AVAILABLE)
    {
      sr_id[id].state = SRID_USED;       /* Set state to USED */
      sr_id[id].owner = owner;           /* Set the owner */
      srid = id;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "sr_id %d: owner %d, state %d not available",
        id,
        sr_id[id].owner,
        sr_id[id].state);
    }
    REX_ISR_UNLOCK(&mc_crit_sect);

  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid sr_id %d",
      id);
  }

  return srid;

} /* mccsrid_allocate_specific_srid */
#endif /* FEATURE_IS2000_REL_C */

/*===========================================================================

FUNCTION mccsrid_check_specific_srid

DESCRIPTION
  This function is used to verify if an SR_ID is still reserved for the same
  owner. This function is called by MC when it receives an SR_ID from DS.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the SI_ID is used by the same owner.

SIDE EFFECTS
===========================================================================*/
boolean mccsrid_check_specific_srid
(
  byte id
    /* SR_ID number to check */
)
{
  boolean ret_val = TRUE;  /* Return value */


  /*
  ** There is no REX_ISR_LOCK required in this function, because
  ** the only other entity that can change SRID state is DS, and it thinks
  ** an SR_ID is reserved for them already.
  */

  if ((id > 0) && (id < MCCSRID_SRID_NUM_MAX))
  {
    if (sr_id[id].state == SRID_ACTIVE)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "SR_ID already active.");
      ret_val = FALSE;
    }
  }
  else
  {
    /* Invalid SR_ID range */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid SR_ID range to check!");
    ret_val = FALSE;
  }

  return ret_val;
} /* mccsrid_check_specific_srid */



/* <EJECT> */
/*===========================================================================

FUNCTION mccsrid_free_srid

DESCRIPTION
  This function deallocates an SRID owned by the owner.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
void mccsrid_free_srid
(
  byte id,
    /* SR_ID number to free */
  mccsrid_srid_owner_type owner
    /* Current owner of the SR_ID to free */
)
{
  boolean free_from_non_owner = FALSE;

  M1X_MSG( DCP, LEGACY_HIGH,
    "Free SRID %d requested by %d",
    id,
    owner);

  if ( (id > 0) && (id < MCCSRID_SRID_NUM_MAX))
  {
    /*
    ** The owner can change after DS calling this function. Critical
    ** section is protected by REX_ISR_LOCK.
    */
    REX_ISR_LOCK(&mc_crit_sect);

    if (sr_id[id].owner == owner)
    {
      /* Free SR_ID */
      sr_id[id].owner = SRID_OWNER_NULL;

      /* If SR_ID is reserved by this owner, free will cause
      ** that SR_ID to be available. If it is currently active,
      ** then there is only the onwership change, no state change.
      ** e.g., if the sr_id is in the current SCR, but user wants
      ** to release it. Owner's call to this function will give
      ** the ownership to BS, but the SR_ID is not usable till
      ** BS release the call.
      */
      if ( sr_id[id].state == SRID_USED )
      {
        sr_id[id].state = SRID_AVAILABLE;
      }
    }
    else
    {
      free_from_non_owner = TRUE;
    }

    REX_ISR_UNLOCK(&mc_crit_sect);

    if (free_from_non_owner)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Free attempt from non-owner ");
    }
  }
  else if ( id > 0 )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "SR_ID %d out of range, from %d!",
      id,
      owner);
  }

  mccsrid_print();

} /* mccsrid_free_srid */

/* <EJECT> */
/*===========================================================================

FUNCTION mccsrid_free_all_owned_srids

DESCRIPTION
  This function deallocates all SRID owned by the owner.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
void mccsrid_free_all_owned_srids
(
  mccsrid_srid_owner_type owner
    /* Current owner of the SR_ID to free */
)
{
  int id;

  for (id = 1; id<MCCSRID_SRID_NUM_MAX; id++)
  {
    /*
    ** The owner can change after DS calling this function. Critical
    ** section is protected by REX_ISR_LOCK.
    */
    REX_ISR_LOCK(&mc_crit_sect);

    if (sr_id[id].owner == owner)
    {
      /* Free SR_ID */
      sr_id[id].owner = SRID_OWNER_NULL;

      if ( sr_id[id].state == SRID_USED )
      {
        sr_id[id].state = SRID_AVAILABLE;
      }
    }

    REX_ISR_UNLOCK(&mc_crit_sect);

  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "All SR_ID owned by %d freed!",
    owner);

  mccsrid_print();

} /* mccsrid_free_all_owned_srids */



/* <EJECT> */
/*===========================================================================

FUNCTION mccsrid_update_srid

DESCRIPTION
  This function updates SR_ID from the connection record.

DEPENDENCIES
  SR_ID range checking has been performed by the caller.
  cfg_ptr should not be a NULL pointer.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
void mccsrid_update_srid
(
  const caii_srv_cfg_type  *cfg_ptr
    /* Ptr to the service configuration record for this update */
)
{
  byte i;  /* iteration index */

  /* Caller should have checked this pointer */
  if (cfg_ptr == NULL)
  {
    ERR_FATAL("cfg_ptr is NULL", 0,0,0);
    /* return; */
  }

  REX_ISR_LOCK(&mc_crit_sect);

  /*
  ** First update the state of all the srid objects.
  ** This procedure is the same as releaes, i.e., it
  ** is treat like the old SCR is released
  */
  mccsrid_release();


  /* Then update those SRID with the connection record */

 if (cfg_ptr != NULL)
 {
   for (i=0; (i<cfg_ptr->num_con) && (i < CAII_SRV_CON_MAX); i++)
    {
      /* Range checking should have been done. But just check again
      ** for safty.
      */
      if ( (cfg_ptr->con[i].sr_id > 0) &&
           (cfg_ptr->con[i].sr_id < MCCSRID_SRID_NUM_MAX)
         )
      {
        sr_id[cfg_ptr->con[i].sr_id].state = SRID_ACTIVE;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "SR_ID %d OUT OF RANGE!",
          cfg_ptr->con[i].sr_id);
      }

    }

    REX_ISR_UNLOCK(&mc_crit_sect);

    M1X_MSG( DCP, LEGACY_HIGH,
      "Updating SR_IDs");
    mccsrid_print();

 }
} /* mccsrid_update_srid */


/* <EJECT> */
/*===========================================================================

FUNCTION mccsrid_release

DESCRIPTION
  This function is called when LL3 exits TC state. SR_ID manager changes
  all the active SR_ID to available if the owner is NULL, otherwise, SR_ID
  manager changes active SR_ID to used if owner is not NULL. Notice that
  this function does not free an SRID if it has an owner.


DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
void mccsrid_release(void)
{
  byte i;  /* index for iteration */

  for (i=1; i<MCCSRID_SRID_NUM_MAX; i++)
  {
    /* Make sure one array element access is atomic. */
    REX_ISR_LOCK(&mc_crit_sect);

    if (sr_id[i].state == SRID_ACTIVE)
    {
      /* Release only the active sr_id */
      if (sr_id[i].owner == SRID_OWNER_NULL)
      {
        sr_id[i].state = SRID_AVAILABLE;
      }
      else
      {
        sr_id[i].state = SRID_USED;
      }
    }

    REX_ISR_UNLOCK(&mc_crit_sect);
  }

} /* mccsrid_release */

#endif /* FEATURE_IS2000_REL_A_SVD */


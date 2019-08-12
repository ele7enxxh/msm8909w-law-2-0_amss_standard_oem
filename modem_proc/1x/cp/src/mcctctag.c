/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                  M A I N   C O N T R O L   T A S K
                  T A G   M A N A G E R

GENERAL DESCRIPTION
  This module is responsible for managing the TAGs used in concurrent service.

EXTERNALIZED FUNCTIONS


  Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctctag.c_v   1.9   02 Oct 2002 16:35:22   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mcctctag.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/13   cjb     Resolve compiler warnings
12/28/12   gga     Klocwork Reviews
05/24/12   ppr     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
11/15/10   jtm     Initial SU API split for CP.
05/14/09   ag      CMI phase-II update
05/11/07   pg      More Lint Cleanup.
05/02/06   an      Fixed Lint Errors
04/25/04   fc      Merged the following from MSMSHARED:
04/20/05   fc       Added support of reporting L2 failure for pending EOM.
03/28/05   fc      Made FCS also available to Rel D.
01/17/05   fc      Fixed lint errors.
12/10/04   sb      Fixed lint warnings.
05/21/04   fc      Lint clean up.
03/12/04   fc      Added support for Release C fast call setup.
03/05/04   bkm     Lint cleanup.
11/18/02   va      Added fn mcctctag_get_first_found_data_call
10/02/02   az      Removed dead code under #if 0
09/19/02   lh      Lint clean up.
07/11/02   lh      Renamed some functions.
05/02/02   lh      Clean up and code review comments.
01/11/02   va      Featurized this file under SVD
11/20/01   lh      Unit Test Bug fix.
09/10/01   lh      Changed TAG incrementation.
08/28/01   lh      Added bug fixes.
08/08/01   ph      Integrated tag timers in to tag manager
05/21/00   lh      Initial revision of the file.
===========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_IS2000_REL_A_SVD

#include "msg.h"
#include "m1x_diag.h"
#include "err.h"
#include "mcctctag.h"
#include "mc_v.h"
#include "mccdma_v.h"
#include "mccdma_i.h"



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


#define MAX_TAG_ITEM_NUM    (CAI_MAX_NUM_CONNECTIONS - 1)
  /* The first call does not need a TAG */

#define NUM_OF_TAG        16
  /* TAG is a 4 bit number, it wraps around at 16. This number is for the
     mod operation. Please do not change */

#if  MAX_TAG_ITEM_NUM > NUM_OF_TAG
        #error Too many connections for tag manager!
#endif

/* Data type of a tag item. */
typedef struct
{
  boolean active;                /* TURE indicates this tag is on the
                                    TAG_OUTSTANDING_LIST */
  byte tag_id;                   /* tag ID of this tag item */
  mcctctag_tag_info_type info;   /* Info to be saved with the tag */
  rex_timer_type tag_timer;      /* Timer associated with this tag */
} mcctctag_tag_item_type;


/* Data type for the tag manager */
typedef struct
{
  byte next_tag;                    /* TAGs as in standard */
  mcctctag_tag_item_type tags[MAX_TAG_ITEM_NUM];      /* An array of tag items */
} mcctctag_tag_mgr_type;


/* Allocate internal data structure for the tag manager */
mcctctag_tag_mgr_type tag_mgr;

/*===========================================================================

FUNCTION mcctctag_initialize

DESCRIPTION
  Performs necessary initialization for the TAG objects.  This function
  is called upon traffic channel initialization.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  All tags will be removed from the outstanding list.
===========================================================================*/
void mcctctag_initialize(void)
{
  byte i;    /* array index */

  /* Standard sets TAGs to 0 in TCI, but increment before every EOM.
  ** Our next_tag is post incremented after EOM, pointing to the next
  ** possible TAG value. So it starts from 1 to match the standard.
  */
  tag_mgr.next_tag = 1;

  /* Initialize the TAG array. */
  for (i=0; i<MAX_TAG_ITEM_NUM; i++)
  {
    tag_mgr.tags[i].active = FALSE;

    /* clear tag timer, just in case it was not deleted in delete_tag() */
    (void) mcc_clr_callback_timer( & (tag_mgr.tags[i].tag_timer), MCC_TAG_MGR_TIMER_TYPE | i );
  }

  M1X_MSG( DCP, LEGACY_MED,
    "Tag manager initialized.");

} /* mcctctag_initialize */

/*===========================================================================

FUNCTION MCCTCTAG_INITIALIZE_TIMERS

DESCRIPTION
  This function initializes the tag timers associated with all tags.
  This should only be called once and is invoked from mcc_powerup_init()
  in mccdma.c

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctctag_initialize_timers( void )
{
  byte i;

    /* Initialize Tag Timers */
  for (i=0; i < MAX_TAG_ITEM_NUM ; i++)
  {
    rex_def_timer_ex( &(tag_mgr.tags[i].tag_timer),
                      mcc_expired_timer_callback_func,
                      MCC_TAG_MGR_TIMER_TYPE | i       /* OR the Index to TYPE for identifying timer */
                    );
  }

}

/*===========================================================================

FUNCTION mcctctag_get_new_tag

DESCRIPTION
  This function allocates a tag for the caller. It also stores related info
  for the pending call. It returns the next tag to the caller.

DEPENDENCIES
  None

RETURN VALUE
  Returns a tag number from 0, to 15 inclusive.
  Returns MCCTCTAG_INVALID_TAG when there is no tag items available.

SIDE EFFECTS
  Out of Tag can happen because we don't overwrite when tag wraps around.
===========================================================================*/
byte mcctctag_get_new_tag
(
  mcctctag_tag_info_type *info
    /* Information to be saved in the tag manager */
)
{
  byte i, j;                             /* loop variable */
  byte tag_id = MCCTCTAG_INVALID_TAG;    /* return value */
  boolean tag_found = FALSE;
  byte inactive_idx;
  boolean bad_tag = FALSE;               /* indicate if a tag is bad */

  /*
  ** First we need to see if there is any free tag object left.
  */
  for( i=0; i<MAX_TAG_ITEM_NUM; i++)
  {
    if (!tag_mgr.tags[i].active)
    {
      break;
    }
  }

  if (i < MAX_TAG_ITEM_NUM)
  {

    /* Yes, we've found a free tag item. Now select an unused
    ** tag_id for it.
    */

    inactive_idx = i;

    /* Because we only need 2 tag item, there is no need to define
    ** 16 items. So as long as tag item is less then tag id number
    ** there has to be an unused tag_id.
    */
    for (i=0; i<=MAX_TAG_ID; i++)    /* Only iterate for MAX_TAG_ID times */
    {
      bad_tag = FALSE;  /* initialize the test result */

      /* Choose a tag, test it against all active tag items */
      /* Under normal condition, next_tag should be good to use. But if
      ** next_tag is already used by an existing tag item (TAG wrap around),
      ** we need to skip that tag value.
      */
      for (j=0; (j<MAX_TAG_ITEM_NUM) && (!bad_tag); j++)
      {
        if (tag_mgr.tags[j].active && (tag_mgr.tags[j].tag_id == tag_mgr.next_tag))
        {
          /* Cannot use this tag, try next one */
          bad_tag = TRUE;
        }
      }

      if (!bad_tag)
      {
        /* we just found a good tag i */
        tag_found = TRUE;
        tag_id = tag_mgr.next_tag;
        tag_mgr.next_tag = (tag_mgr.next_tag + 1) % NUM_OF_TAG;
                                           /* increment next_tag */
        break;
      }
      else
      {
        tag_mgr.next_tag = (tag_mgr.next_tag + 1) % NUM_OF_TAG;
                                           /* increment and try again */
      }
    }

    /* Do we found anything? */
    if (tag_found)
    {
      /* Save the information into the chosen tag item */
      tag_mgr.tags[inactive_idx].info = *info;

      /* Mark the tag as outstanding */
      tag_mgr.tags[inactive_idx].active = TRUE;

      /* Set tag ID */
      tag_mgr.tags[inactive_idx].tag_id = tag_id;

      /* Set the tag timer to 12 seconds */
      (void) mcc_set_callback_timer( &(tag_mgr.tags[inactive_idx].tag_timer), CAI_T42M, MCC_TAG_MGR_TIMER_TYPE | inactive_idx );

      M1X_MSG( DCP, LEGACY_MED,
        "Tag %d available",
        tag_id);
    }
    else
    {
      /* This is impossible if tag item is less than 16 */
      M1X_MSG( DCP, LEGACY_ERROR,
        "No valid tag!!!");
    }
  }
  else
  {
    /* No tag is free! */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Out of free TAG item!!!");
  }

  return tag_id;

} /*lint !e818 */ /* mcctctag_get_new_tag */

/*===========================================================================

FUNCTION delete_tag

DESCRIPTION
  This function deletes a tag by its tag index. This function is for internal
  use.

DEPENDENCIES
  Caller make sure that index is within the range, and the tag is active.

RETURN VALUE
  None.

SIDE EFFECTS
  The specified tag is taken out of the outstanding list.
===========================================================================*/
void delete_tag
(
  byte idx
    /* index of the tag */
)
{
    /* Mark the tag as not outstanding */
    tag_mgr.tags[idx].active = FALSE;

    /* clear tag timer */
    (void)mcc_clr_callback_timer( & (tag_mgr.tags[idx].tag_timer), MCC_TAG_MGR_TIMER_TYPE | idx );

} /* delete_tag */

/*===========================================================================

FUNCTION mcctctag_get_by_tag_id

DESCRIPTION
  This function check to see if a tag identified by tag id is active. If the
  tag is found, it's info will be returned to the caller, and the tage will
  be removed from the outstanding list.

  This function is called by L3 when a CLAM is received, to check if the tag
  in the CLAM is pending.

DEPENDENCIES
  We assume that tag id is uniquely maintained, i.e., no two active tags have
  the same tag id.

RETURN VALUE
  TURE if a tag is active. Tag info is copied to the pointer supplied by the
  caller.
  FALSE if a tag is not active.

SIDE EFFECTS
  If a tag is found and active, it will become inactive.
===========================================================================*/
boolean mcctctag_get_by_tag_id
(
  byte id,
    /* Tag ID to check */
  mcctctag_tag_info_type *info
    /* Information to be returned by the tag manager */
)
{
  byte i;                 /* index */
  boolean status = FALSE; /* return value */

  /* Let's do another no-brainer linear search */
  for( i=0; i<MAX_TAG_ITEM_NUM; i++)
  {
    if (tag_mgr.tags[i].active && (tag_mgr.tags[i].tag_id == id ))
    {
      /* Match found */
      status = TRUE;

      /* Copy the info */
      if (info == NULL)
      {
        ERR_FATAL("info is Null Ptr", 0,0,0);
        /* return FALSE; */
      }
      else
      {
        *info = tag_mgr.tags[i].info;
      }
      /* Delete the tag */
      delete_tag(i);

      break;

    }
  }

  return status;

} /* mcctctag_get_by_tag_id */

/*===========================================================================

FUNCTION mcctctag_get_first_found_data_call

DESCRIPTION
  This function retruns tag information for the first outstanding
  data call found in the list.  This is needed because when we get
  "retry_order" for data EOM, there is no tag info in the order but
  CP needs to tell CM about the retry delay so that DS can update value
  accordingly. So the only way CP knows which call this is, is by doing a
  serach for data SO in the tag outstanding list. If there is more than 1
  data call in the list, only the first one will be removed (This is due
  to the std pitfall)

DEPENDENCIES
  None

RETURN VALUE
  TURE if a tag with data SO is active. Tag info is copied to the pointer supplied by the
  caller.
  FALSE if there is no active TAG for a DATA SO.

SIDE EFFECTS
  If a matching tag is found and active, it will become inactive.
===========================================================================*/
boolean mcctctag_get_first_found_data_call
(
  mcctctag_tag_info_type *info
    /* Information to be returned by the tag manager */
)
{
  byte i;                 /* index */
  boolean status = FALSE; /* return value */

  /* Let's do another no-brainer linear search */
  for( i=0; i<MAX_TAG_ITEM_NUM; i++)
  {
    if (tag_mgr.tags[i].active && DS_ISDATA(tag_mgr.tags[i].info.so))
    {
      /* Match found */
      status = TRUE;

      /* Copy the info */
      if (info == NULL)
      {
        ERR_FATAL("info is Null Ptr", 0,0,0);
        /* return FALSE; */
      }
      else
      {
        *info = tag_mgr.tags[i].info;
      }

      /* Delete the tag */
      delete_tag(i);

      break;

    }
  }

  return status;

} /* mcctctag_get_first_found_data_call*/

/*===========================================================================

FUNCTION mcctctag_get_by_call_id

DESCRIPTION
  This function checks to see if a tag identified by the call id is
  outstanding. If yes, this tag will be removed from the outstanding list.
  The info associated with the tag will be returned to the caller.

  This function is called when a release is received from CM.

DEPENDENCIES
  We assume that tag id is uniquely maintained, i.e., no two active tags have
  the same tag id.

RETURN VALUE
  Tag info is copied to the pointer supplied by the
  caller.
  If a tag is found, its index will be returned.
  Otherwise, MCCTCTAG_INVALID_TAG is returned.

SIDE EFFECTS
  If a tag is found and active, it will become inactive.
===========================================================================*/
byte mcctctag_get_by_call_id
(
  byte id,
    /* Call ID to check */
  mcctctag_tag_info_type *info
    /* Information to be returned by the tag manager */
)
{
  byte i;                 /* index */
  boolean tid = MCCTCTAG_INVALID_TAG; /* return value */

  /* Let's do another no-brainer linear search */
  for( i=0; i<MAX_TAG_ITEM_NUM; i++)
  {
    if (tag_mgr.tags[i].active && (tag_mgr.tags[i].info.call_id == id ))
    {
      /* Match found */
      tid = tag_mgr.tags[i].tag_id;

      /* Copy the info */
      if (info == NULL)
      {
        ERR_FATAL("info is Null Ptr", 0,0,0);
        /* return MCCTCTAG_INVALID_TAG; */
      }
      else
      {
        *info = tag_mgr.tags[i].info;
      }
      /* Delete the tag */
      delete_tag(i);

      break;

    }
  }

  return tid;

} /* mcctctag_get_by_call_id */

/*===========================================================================

FUNCTION mcctctag_is_call_id_outstanding

DESCRIPTION
  This function check to see if a call_id is in the outstanding list.

DEPENDENCIES

RETURN VALUE
  TRUE if a tag is active.
  FALSE if a tag is not active.

SIDE EFFECTS
 NONE
===========================================================================*/
boolean mcctctag_is_call_id_outstanding
(
  byte id
    /* Call ID to check */
)
{
  byte i;                 /* index */
  boolean status = FALSE; /* return value */

  /* Let's do another no-brainer linear search */
  for( i=0; i<MAX_TAG_ITEM_NUM; i++)
  {
    if (tag_mgr.tags[i].active && (tag_mgr.tags[i].info.call_id == id ))
    {
      /* Match found */
      status = TRUE;

      break;

    }
  }

  return status;

} /* mcctctag_is_call_id_outstanding */

/*===========================================================================

FUNCTION mcctctag_is_tag_outstanding

DESCRIPTION
  This function check to see if a tag id is in the outstanding list.

DEPENDENCIES

RETURN VALUE
  TRUE if a tag is active.
  FALSE if a tag is not active.

SIDE EFFECTS
 NONE
===========================================================================*/
boolean mcctctag_is_tag_outstanding
(
  byte id,
    /* Tag ID to check */
  mcctctag_tag_info_type *info
    /* Information to be returned by the tag manager */
)
{
  byte i;                 /* index */
  boolean status = FALSE; /* return value */

  /* Let's do another no-brainer linear search */
  for( i=0; i<MAX_TAG_ITEM_NUM; i++)
  {
    if (tag_mgr.tags[i].active && (tag_mgr.tags[i].tag_id == id ))
    {
      /* Match found */
      status = TRUE;

      if (info != NULL)
      {
        /* Copy the tag info */
        *info = tag_mgr.tags[i].info;
      }
      break;
    }
  }

  return status;

} /* mcctctag_is_tag_outstanding */

/*===========================================================================

FUNCTION mcctctag_num_tag_outstanding

DESCRIPTION
  This function returns number of tags in the outstanding list.

DEPENDENCIES

RETURN VALUE
  Number of tags in the outstanding list.

SIDE EFFECTS
 NONE
===========================================================================*/
byte mcctctag_num_tag_outstanding (void)
{
  byte i;                 /* index */
  byte num = 0; /* return value */

  /* Let's do another no-brainer linear search */
  for( i=0; i<MAX_TAG_ITEM_NUM; i++)
  {
    if (tag_mgr.tags[i].active)
    {
      num++;
    }
  }

  return num;

} /* mcctctag_num_tag_outstanding */

/*===========================================================================

FUNCTION mcctctag_process_tag_timer

DESCRIPTION
  This function is called upon tag timer expiration. The assumption now is
  that the LSB of the timer ID is used as index into the tag item.
  When this function is called, the tag item pointed by the index will be
  taken out of the outstanding list.

DEPENDENCIES
  None.

RETURN VALUE
  TURE if a tag is active. Tag info & ID is copied to the pointers supplied by the
  caller.
  FALSE if a tag is not active or other error is detected, in which case,
  no info is copied to the caller's pointer.

SIDE EFFECTS
  If a tag is active, it will become inactive.
===========================================================================*/
boolean mcctctag_process_tag_timer
(
  dword timer_id,
    /* Timer ID */
  byte *tag_id_ptr,
    /* Tag corresponding to the expired timer */
  mcctctag_tag_info_type *info_ptr
    /* Information to be returned by the tag manager */
)
{
  byte idx;               /* index */
  boolean status = FALSE; /* return value */


  idx = (byte)(timer_id & MCC_TIMER_SEQUENCE_MASK);


  if ( ((timer_id & (dword)MCC_TIMER_TYPE_MASK) != MCC_TAG_MGR_TIMER_TYPE ) ||
       (idx >= MAX_TAG_ITEM_NUM)                                     ||
       (!(tag_mgr.tags[idx].active))
     )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Bad Tag Manager timer ID received %x",
      timer_id);
  }
  else
  {
    /* clear timer */
    (void) mcc_clr_callback_timer( & (tag_mgr.tags[idx].tag_timer), MCC_TAG_MGR_TIMER_TYPE | idx );

    /* Match found */
    status = TRUE;

    /* Copy the info */
    if (info_ptr == NULL)
    {
      ERR_FATAL("info_ptr is Null", 0,0,0);
      /* return FALSE; */
    }
    else
    {
      *info_ptr = tag_mgr.tags[idx].info;
    }

    if (tag_id_ptr == NULL)
    {
      ERR_FATAL("tag_id_ptr is Null", 0,0,0);
      /* return FALSE; */
    }
    else
    {
      *tag_id_ptr = tag_mgr.tags[idx].tag_id;
    }

    /* Take tag out of the outstanding list */
    tag_mgr.tags[idx].active = FALSE;
  }

  return status;

} /* mcctctag_process_tag_timer */

/*===========================================================================

FUNCTION mcctctag_set_l2_ack_rxed_by_tag_id

DESCRIPTION
  This function check to see if a tag identified by tag id is active. If the
  tag is found, the eom is marked as L2 ACK received.

  This function is called by L3 when ACK is received for EOM.

DEPENDENCIES
  We assume that tag id is uniquely maintained, i.e., no two active tags have
  the same tag id.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcctctag_set_l2_ack_rxed_by_tag_id
(
  byte id
    /* Tag ID to check */
)
{
  byte i; /* index */

  /* Let's do another no-brainer linear search */
  for( i=0; i<MAX_TAG_ITEM_NUM; i++)
  {
    if (tag_mgr.tags[i].active && (tag_mgr.tags[i].tag_id == id ))
    {
      tag_mgr.tags[i].info.l2_ack_rxed = TRUE;
      M1X_MSG( DCP, LEGACY_MED,
        "L2 ack rxed for EOM");
      break;
    }
  }

} /* mcctctag_set_l2_ack_rxed_by_tag_id */

#if defined(FEATURE_IS2000_REL_C)
/*===========================================================================

FUNCTION mcctctag_get_by_sr_id

DESCRIPTION
  This function checks to see if a tag identified by the SR ID is
  outstanding. If yes, this tag will be removed from the outstanding list.
  The info associated with the tag will be returned to the caller.

  This function is called when a connection restored by SCM.

DEPENDENCIES
  We assume that tag id is uniquely maintained, i.e., no two active tags have
  the same tag id.

RETURN VALUE
  Tag info is copied to the pointer supplied by the
  caller.
  If a tag is found, its index will be returned.
  Otherwise, MCCTCTAG_INVALID_TAG is returned.

SIDE EFFECTS
  If a tag is found and active, it will become inactive.
===========================================================================*/
byte mcctctag_get_by_sr_id
(
  byte sr_id,
    /* SR ID to check */
  mcctctag_tag_info_type *info
    /* Information to be returned by the tag manager */
)
{
  byte i;                 /* index */
  boolean tid = MCCTCTAG_INVALID_TAG; /* return value */

  /* Let's do another no-brainer linear search */
  for( i=0; i<MAX_TAG_ITEM_NUM; i++)
  {
    if (tag_mgr.tags[i].active && (tag_mgr.tags[i].info.sr_id == sr_id ))
    {
      /* Match found */
      tid = tag_mgr.tags[i].tag_id;

      /* Copy the info */
      if (info == NULL)
      {
        /* Should never happen */
        ERR_FATAL("info is Null Ptr", 0,0,0);
        /* return tid; */
      }
      else
      {
        *info = tag_mgr.tags[i].info;
      }
      /* Delete the tag */
      delete_tag(i);

      break;

    }
  }

  return tid;

} /* mcctctag_get_by_sr_id */
#endif /* FEATURE_IS2000_REL_C */

#endif //FEATURE_IS2000_REL_A_SVD

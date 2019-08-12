#ifndef MCCTCTAG_H
#define MCCTCTAG_H

/*===========================================================================

          MAIN CONTROL TRAFFIC CHANNEL TAG MANAGER

GENERAL DESCRIPTION
  This task manages tags for pending originations in concurrent service.

DESCRIPTION
  This module is part of the MCCTC. It manages call tags, and associated
  information for origination a call when the first call is already in
  progress.

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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctctag.h_v   1.4   11 Jul 2002 13:05:02   lhang  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mcctctag.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/12   ppr     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
04/24/05   fc      Merged the following from MSMSHARED:
04/20/05   fc       Added support of reporting L2 failure for pending EOM.
03/28/05   fc      Made FCS also available to Rel D.
03/12/04   fc      Added support for Release C fast call setup.
11/18/02   va      Added prototype for mcctctag_get_first_found_data_call
07/11/02   lh      Renamed some functions.
05/02/02   lh      Clean up and code review comments.
01/11/02   va      Featurized this file under SVD
08/08/01   ph      Integrated tag timers in to Tag manager
05/21/01   lh      Initial revision of the file.

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "caii.h"
#include "cmd.h"
#include "queue.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_IS2000_REL_A_SVD

#define MCCTCTAG_INVALID_TAG    0xff
/* This value is returned when something is wrong during getting a TAG */

#define MAX_TAG_ID    15
/* Maximun tag_id allowed by 4 bits */


/* Type for tag data queries */
typedef struct
{
  byte call_id;        /* Call id (from CM) of the pending origination */
  byte sr_id;          /* SR ID of the pending call */
  word so;             /* SO for the pending origination */
  boolean l2_ack_rxed; /* Indicates if L2 ACK received or not. */
} mcctctag_tag_info_type;

/*===========================================================================

                    PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* <EJECT> */
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
extern void mcctctag_initialize(void);

/* <EJECT> */
/*===========================================================================

FUNCTION mcctctag_exit

DESCRIPTION
  Performs opposite operation of initialization. Delete all timers.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  All tags will be removed from the outstanding list.
===========================================================================*/
extern void mcctctag_exit(void);

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
extern void mcctctag_initialize_timers( void );


/* <EJECT> */
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
  If there is an active tag with the next_tag_num, it will be over-written.
===========================================================================*/
extern byte mcctctag_get_new_tag
(
  mcctctag_tag_info_type *info
    /* Information to be saved in the tag manager */
);


/* <EJECT> */
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
extern boolean mcctctag_get_by_tag_id
(
  byte id,
    /* Tag ID to check */
  mcctctag_tag_info_type *info
    /* Information to be returned by the tag manager */
);


/* <EJECT> */
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
extern byte mcctctag_get_by_call_id
(
  byte id,
    /* Call ID to check */
  mcctctag_tag_info_type *info
    /* Information to be returned by the tag manager */
);

/* <EJECT> */
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
extern boolean mcctctag_is_call_id_outstanding
(
  byte id
    /* Call ID to check */
);

/* <EJECT> */
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
extern boolean mcctctag_is_tag_outstanding
(
  byte id,
    /* Tag ID to check */
  mcctctag_tag_info_type *info
    /* Information to be returned by the tag manager */
);

/* <EJECT> */
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
byte mcctctag_num_tag_outstanding (void);


/* <EJECT> */
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
extern boolean mcctctag_process_tag_timer
(
  dword tid,
    /* Timer ID */
  byte *tag_id_ptr,
    /* To return tag ID corresponding to expired timer */
  mcctctag_tag_info_type *info_ptr
    /* Information to be returned by the tag manager */
);

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
);

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
);

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
);
#endif /* FEATURE_IS2000_REL_C */

#endif //FEATURE_IS2000_REL_A_SVD

#endif /* MCCTCTAG_H */


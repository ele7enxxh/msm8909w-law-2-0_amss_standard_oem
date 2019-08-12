#ifndef DSRLPDEL_H
#define DSRLPDEL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  R A D I O   L I N K   P R O T O C O L   D E L A Y E D   F R A M E 
  D E T E C T I O N  F I L E

GENERAL DESCRIPTION
  This file contains the module which is used in implementing delayed frame 
  detection.
  
EXTERNALIZED FUNCTIONS
  dsrlpdel_init()
     Called at phone power-up.  Initializes delayed frame detection vars.
  dsrlpdel_get_array
     Called when an RLP session first starts.  Returns an id to the arrays
     used for delayed frame detection.
  dsrlpdel_put_array
     Called when an RLP session finishes, returning the delayed frame
     detection arrays back to the global pool.
  dsrlpdel_reset
     Called when RLP is reset.  Cleans out the delayed frame detection
     arrays.
  dsrlpdel_q_empty
     Returns TRUE if the delayed frame detection arrays for a particular
     RLP session are empty.  Else returns FALSE.
  dsrlpdel_add
     Adds an element to the "add array".
  dsrlpdel_remove
     Removes elements from the "remove array" and adjusts the add and remove
     indices (which are internal to this module) accordingly.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Before calling functions, must first call dsrlpdel_init().
  
  Copyright (c) 2000 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/dsrlpdel.h_v   1.0   18 Oct 2000 18:07:02   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsrlpdel.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
05/15/00   na/ak      Created file.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "dsrlpi.h"
#include "dsm.h"

/*===========================================================================
                       EXTERNAL DECLARATIONS
===========================================================================*/
/*---------------------------------------------------------------------------
   this is returned when there are no delayed frame arrays available 
---------------------------------------------------------------------------*/
#define DSRLPDEL_NO_ARRAY   0xFFFFFFFF


/*===========================================================================
                       EXTERNAL  FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION       DSRLPDEL_INIT

DESCRIPTION    Called at phone powerup.  Should only be called once per
               power-cycle.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlpdel_init(void);

/*===========================================================================
FUNCTION       DSRLPDEL_GET_ARRAY

DESCRIPTION    Called when an rlp session is initialized.  Assigns a delayed
               frame detection struct to the calling task.

DEPENDENCIES   None

RETURN VALUE   DSRLPDEL_NO_ARRAY if no queue could be assigned, else returns
               the index of the queue.

SIDE EFFECTS   None
===========================================================================*/
uint32 dsrlpdel_get_array(void);

/*===========================================================================
FUNCTION       DSRLPDEL_PUT_ARRAY

DESCRIPTION    Called when an rlp session is over.  Returns the array back to
               the free pool of delayed frame detect arrays.  Clears out the
               struct.

DEPENDENCIES   None

RETURN VALUE   NONE

SIDE EFFECTS   None
===========================================================================*/
void dsrlpdel_put_array
(
  uint32      rlpdel_id
);

/*===========================================================================
FUNCTION       DSRLPDEL_RESET

DESCRIPTION    Cleans up the arrays, and resets struct back to initial state.

DEPENDENCIES   None

RETURN VALUE   NONE

SIDE EFFECTS   None
===========================================================================*/
void dsrlpdel_reset
(
  uint32  rlpdel_id
);

/*===========================================================================
FUNCTION       DSRLPDEL_Q_EMPTY

DESCRIPTION    Returns TRUE if both arrays which make up the delayed frame
               mechanism are empty.  Else returns FALSE.

DEPENDENCIES   None

RETURN VALUE   TRUE if both arrays are empty, else FALSE.

SIDE EFFECTS   None
===========================================================================*/
boolean dsrlpdel_q_empty
( 
  uint32  rlpdel_id
);

/*===========================================================================
FUNCTION       DSRLPDEL_ADD

DESCRIPTION    Adds an element to the frame detection array.  Puts it in
               order in the array.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlpdel_add
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,
  dsm_item_type            *fr_item_ptr,
  dsrlpi_curr_fr_info_type *curr_info_ptr
);

/*===========================================================================
FUNCTION       DSRLPDEL_REMOVE

DESCRIPTION    Removes the elements from the delayed frame queue.  Updates
               the add and remove indexes.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlpdel_remove
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr
);

#endif

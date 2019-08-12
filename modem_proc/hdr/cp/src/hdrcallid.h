#ifndef HDRCALLID_H
#define HDRCALLID_H

/*===========================================================================

             H D R   C A L L   I D   H E A D E R   F I L E

DESCRIPTION
  This is the header file for the HDR file that manages the HDR Call IDs.

Copyright (c) 2003-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrcallid.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/13   vko     Fixed de-allocation of all call-id's due to call orig failure
02/14/10   wsh     Fixed reporting old callid after ConnectionClose
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
04/04/05   pba     Removed Lint errors
11/29/04   dna     Send END_CNF to CM with call_id of user ended call.
10/22/03    km     Deleted functions hdrcallid_get_cur_call_id & 
                   hdrcallid_get_call_id
09/12/03   mpa     Added missing declaration for hdrcallid_init()
09/11/03    km     Added function hdrcallid_get_all_call_ids
06/18/03   vas     Added hdrcallid_deallocate_call_id()
04/26/03   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "cm.h"

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif


/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
                           FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION HDRCALLID_SET_CUR_CALL_ID

DESCRIPTION
  Associates a Call Id with a HDR call. It result in adding an entry into the
  list of active call ids.

DEPENDENCIES
  None.

PARAMETERS
  call_id: The call Id to be associated with the HDR call.

RETURN VALUE
  None.

SIDE EFFECTS
  Increments the number of active HDR call ids.

===========================================================================*/
void hdrcallid_set_cur_call_id
(
  cm_call_id_type call_id
);

/*===========================================================================

FUNCTION HDRCALLID_GET_CUR_CALL_ID

DESCRIPTION
  Retrieves the Call Id of current call pointer

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  call_id: The call id associated with the current HDR call ID.

SIDE EFFECTS
  None

===========================================================================*/
cm_call_id_type hdrcallid_get_cur_call_id(void);

/*===========================================================================

FUNCTION HDRCALLID_GET_ALL_CALL_IDS

DESCRIPTION
  Returns the list of active call IDs
  
DEPENDENCIES
  None.

PARAMETERS
  call_ids - Array into which active call ids need to be copied

RETURN VALUE
  Number of call ids that were copied to call_ids array

SIDE EFFECTS
  None.

===========================================================================*/
typedef cm_call_id_type  hdrcallid_all_callids[CM_CALL_ID_MAX];  /* call ids */
byte hdrcallid_get_all_call_ids
(
  hdrcallid_all_callids call_ids
);



/*===========================================================================

FUNCTION HDRCALLID_GET_NUM_ACTIVE_CALL_IDS

DESCRIPTION
  Returns the number of call ids that have been associated with the 
  particular HDR call.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  Number of call ids associated with the HDR call.

SIDE EFFECTS
  None.

===========================================================================*/
int hdrcallid_get_num_active_call_ids
(
  void
);

/*===========================================================================

FUNCTION HDRCALLID_DEALLOCATE_CALL_ID

DESCRIPTION
  This function deallocates the specified Call ID.

DEPENDENCIES
  None.

PARAMETERS
  call_id - Which Call ID to delete.

RETURN VALUE
  None.

SIDE EFFECTS
  The number of active call ids changes. If the call id deallocated happens
  to be the current call id as returned by the function 
  hdrcallid_get_cur_call_id, then it will be reassigned/set to NULL.
  
===========================================================================*/
void hdrcallid_deallocate_call_id
( 
  cm_call_id_type call_id
);

/*===========================================================================

FUNCTION HDRCALLID_SAVE_USER_CALLID

DESCRIPTION
  This function saves the user callid for calls that closes connection.
  After connection is closed, the saved user callid will be used to
  report to CM

DEPENDENCIES
  None.

PARAMETERS
  call_id - Call ID to save

RETURN VALUE
  None.

SIDE EFFECTS
  If the caller specifies that this was a user ended call and it is still
  pending completion, the call ID is saved and the flag 
  hdrcallid.user_end_call_id_is_valid is set to TRUE.
  
===========================================================================*/
void hdrcallid_save_user_callid
(
  cm_call_id_type call_id
);

/*===========================================================================

FUNCTION HDRCALLID_DEALLOCATE_ALL_CALL_IDS

DESCRIPTION
  This function deallocates all active HDR call IDs. Number of active call 
  ids for HDR gets set to 0.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcallid_deallocate_all_call_ids
( 
  void
);

/*===========================================================================

FUNCTION HDRCALLID_DEALLOCATE_USER_ENDED_CALL_ID

DESCRIPTION
  If a user end was pending, this function clears the flag and returns
  the call ID received in the end command.
  
DEPENDENCIES
  None.

PARAMETERS
  call_id_ptr - [output parameter] Pointer to variable that the user ended 
                call ID is returned into.  If no user end was pending 
                this value is unchanged.

RETURN VALUE
  True if a user ended call ID was pending, FALSE otherwise.

SIDE EFFECTS
  The flag hdrcallid.user_end_call_id_is_valid is set to FALSE.

===========================================================================*/
boolean hdrcallid_deallocate_user_ended_call_id
(
  cm_call_id_type *call_id_ptr
);

/*===========================================================================

FUNCTION HDRCALLID_INIT

DESCRIPTION
  Initializes the HDR Call ID module

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcallid_init
(
  void
);
#endif /* HDCALLID_H */



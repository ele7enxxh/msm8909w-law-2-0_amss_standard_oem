
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H D R   C A L L   I D

GENERAL DESCRIPTION
  This file deals manages keeping track and allocation/deallocation of Call 
  Ids associated with a particular HDR call.
  
EXTERNALIZED FUNCTIONS 


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrcallid.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/11/13   vko     Fixed de-allocation of all call-id's due to call orig failure
10/07/13   vko     Replaced ASSERTs on external/global variables as per 
                   go/assert guidelines
02/14/10   wsh     Fixed reporting old callid after ConnectionClose
10/07/10   pba     Cleared callid list when HDR stack is reactivated.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
12/10/08   smd     Replaced assert.h with amssassert.h
04/04/05   pba     Removed Lint errors
11/29/04   dna     Send END_CNF to CM with call_id of user ended call.
10/22/03    km     Deleted functions hdrcallid_get_cur_call_id & 
                   hdrcallid_get_call_id
10/09/03    km     Fixed callid deallocation and added to F3 msgs
09/12/03   mpa     Converted F3 messages to use new MSG2.0 SSID
09/11/03    km     Added function hdrcallid_get_all_call_ids
09/09/03    km     Fixed bug in hdrcallid_deallocate_call_id and modified
                   F3 msg macros to correspond to the  number of parms.
08/28/03   vas     Fixed bug in buffer search when deallocating call id 
07/16/03   vas     Added code to check if entry exists in the Call ID list
                   before adding it.
06/18/03   vas     Added hdrcallid_deallocate_call_id()
04/26/03   vas     Created module

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "customer.h"
#include "hdrdebug.h"
#include "amssassert.h"
#include "hdrcallid.h"
#include "cm.h"

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif

#include "err.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
LOCAL struct
{
  cm_call_id_type allocated_call_id[CM_CALL_ID_MAX]; /* List of Call ids   */
  int num_active_call_ids;       /* # of call ids associated with HDR call */
  cm_call_id_type *cur_call_id_ptr; /* Pointer to the current call id      */
  boolean user_end_call_id_is_valid;    /* Waiting to send ConnectionClose */
  cm_call_id_type user_end_call_id;                  /* User ended call id */
} hdrcallid;

/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/

/*===========================================================================
                           FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION HDRCALLID_SET_CUR_CALL_ID

DESCRIPTION
  Associates a Call Id with a HDR call. It result in adding an entry into the
  list of active call ids.
  
  If the Call Id already exists in the list, then it is not added again, but
  simply the Current Call ID is made to point to this entry.

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
)
{
  int i = 0; /* Loop index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Search through the list for the specified call_id
  -------------------------------------------------------------------------*/
  for( i = 0; i < hdrcallid.num_active_call_ids; i++ )
  {
    if( hdrcallid.allocated_call_id[i] == call_id )
    {
      break;
    }
  }

  /*-------------------------------------------------------------------------
    If the call_id is found, then set the current call id to point to this
    one, otherwise add a new entry.
  -------------------------------------------------------------------------*/
  if( i == hdrcallid.num_active_call_ids )
  {
    if( hdrcallid.num_active_call_ids == CM_CALL_ID_MAX )
    {
      ERR("Max Callid %d. Cant add more CallIds", 
                     hdrcallid.num_active_call_ids, 0, 0 );
      return;
    }
    else
    {
      /*-------------------------------------------------------------------------
        Add a new entry to the end of the list 
      -------------------------------------------------------------------------*/
      hdrcallid.allocated_call_id[hdrcallid.num_active_call_ids] = call_id;
      hdrcallid.cur_call_id_ptr = 
        &hdrcallid.allocated_call_id[hdrcallid.num_active_call_ids]; 
      hdrcallid.num_active_call_ids++;      
      HDR_MSG_PROT_2 (MSG_LEGACY_MED, "Add HDR CallId %d TotalHDRCallId=%d", 
                      call_id, hdrcallid.num_active_call_ids);
    }
  }
  else
  {
    /*-------------------------------------------------------------------------
      The call id already exists in the list. We need to reorder the list
      so that this entry becomes the last one.
    -------------------------------------------------------------------------*/
    HDR_MSG_PROT_2 (MSG_LEGACY_MED, "Set HDR CallId %d TotalHDRCallId=%d", 
                    call_id, hdrcallid.num_active_call_ids);
    hdrcallid.cur_call_id_ptr = &hdrcallid.allocated_call_id[i]; 
  }
}

/*===========================================================================

FUNCTION HDRCALLID_GET_CUR_CALL_ID

DESCRIPTION
  Retrieves the Call Id of current call pointer

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  call_id: The call d to be associated with the current HDR call ID.

SIDE EFFECTS
  None

===========================================================================*/
cm_call_id_type hdrcallid_get_cur_call_id(void)
{
  if(hdrcallid.cur_call_id_ptr != NULL)
     return *(hdrcallid.cur_call_id_ptr);
   else
      return CM_CALL_ID_INVALID;
}

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
byte hdrcallid_get_all_call_ids
(
  hdrcallid_all_callids call_ids
)
{
  byte num_callids = 0;
  int i;

  if( !(hdrcallid.num_active_call_ids <= CM_CALL_ID_MAX) )
  {
     ERR_FATAL("Check hdrcallid.num_active_call_ids <= CM_CALL_ID_MAX Failed", 
               0, 0, 0);
  }

  /*-------------------------------------------------------------------------
  Search through the list for valid call_ids
-------------------------------------------------------------------------*/
  for ( i = 0; ((i < CM_CALL_ID_MAX) && 
                (num_callids < hdrcallid.num_active_call_ids)) ; i++ )
  {
    if ( hdrcallid.allocated_call_id[i] != CM_CALL_ID_INVALID )
    {
      /* Copy callid to dest and increment num_callids */
      call_ids[ num_callids++ ] = hdrcallid.allocated_call_id [ i ];
      HDR_MSG_PROT_3 (MSG_LEGACY_MED, "hdrcallid_get_all_call_ids:CallId=%d,index=%d,num_callids=%d",
                      hdrcallid.allocated_call_id [ i ], i, num_callids); 
    }
  }

  /* Return num of call ids */
  return( num_callids );
}



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
int hdrcallid_get_num_active_call_ids(void)
{

  return hdrcallid.num_active_call_ids;

}

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
  
  If the caller specifies that this was a user ended call and it is still
  pending completion, the call ID is saved and the flag 
  hdrcallid.user_end_call_id_is_valid is set to TRUE.
  
===========================================================================*/
void hdrcallid_deallocate_call_id
( 
  cm_call_id_type call_id
)
{
  int i = 0; /* Loop index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "Dealloc CallId %d", call_id);

  if (hdrcallid.num_active_call_ids <= 0)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
           "Rcvd CallID Dealloc when num_active_call_ids is 0");
    return;
  }

  /*-------------------------------------------------------------------------
    Search through the list for the specified call_id
  -------------------------------------------------------------------------*/
  for( i = 0; i < CM_CALL_ID_MAX; i++ )
  {
    if( hdrcallid.allocated_call_id[i] == call_id )
    {
      break;
    }
  }

  /*-------------------------------------------------------------------------
    If the call_id is found, then remove it, otherwise print an error
  -------------------------------------------------------------------------*/
  if( i == CM_CALL_ID_MAX )
  {
    ERR("CallId %d Not found", call_id, 0, 0);
  }
  else
  {
    /* Found, remove the call id */
    hdrcallid.allocated_call_id[i] = CM_CALL_ID_INVALID;
    hdrcallid.num_active_call_ids--;
    
    /*If the Call id removed was the current call id, then set
      it to the 1st available remaining one in the list */
    if ( CM_CALL_ID_INVALID == *hdrcallid.cur_call_id_ptr )
    {

      /* Reset the curr call id ptr */
      hdrcallid.cur_call_id_ptr = NULL;

      /* If there is at least one more call id, set it to the 1st one */
      if(hdrcallid.num_active_call_ids > 0 )
      {

        for (i = 0; i < CM_CALL_ID_MAX; i++) {
          if( hdrcallid.allocated_call_id[i] != CM_CALL_ID_INVALID )
          {
              /* Update the curr call ptr */
              hdrcallid.cur_call_id_ptr = &hdrcallid.allocated_call_id[i];
              break;
          }

        } /* for... */
      } /* if(hdrcallid.num_active_call_ids > 0 ) */
 
    } /* if ( CM_CALL_ID_INVALID == *hdrcallid.cur_call_id_ptr ) */

    if (hdrcallid.cur_call_id_ptr != NULL) 
    {
      HDR_MSG_PROT_1( MSG_LEGACY_MED, "CallId callid=%d", 
                      *hdrcallid.cur_call_id_ptr);
    } 
    else 
    {
      HDR_MSG_PROT( MSG_LEGACY_MED, "curr CallId = NULL" ); 
    }
  } /* if call id not found */
} /* hdrcallid_deallocate_call_id() */

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
)
{
  /*-------------------------------------------------------------------------
  The requirement from CM is that we send CM_HDR_CALL_END_CNF_F for every 
  HDRMC_END_F we get, but if we need to send a ConnectionClose message we 
  must wait until after it is sent before we tell CM, so this flags
  that we are waiting to report the ended call ID.
  -------------------------------------------------------------------------*/
  hdrcallid.user_end_call_id_is_valid = TRUE;
  hdrcallid.user_end_call_id = call_id;
}

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
void hdrcallid_deallocate_all_call_ids( void )
{
  int i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "Dealloc all (Num=%d) HDR CallIds",
                  hdrcallid.num_active_call_ids);
  for( i = 0; i < CM_CALL_ID_MAX; i++ )
  {
    hdrcallid.allocated_call_id[i] = CM_CALL_ID_INVALID;
  }

  hdrcallid.num_active_call_ids = 0;
  hdrcallid.cur_call_id_ptr = NULL; 
} /* hdrcallid_deallocate_all_call_ids() */

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
)
{
  boolean ret_val = hdrcallid.user_end_call_id_is_valid; 
  /* if a user ended call ID was pending */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (hdrcallid.user_end_call_id <= CM_CALL_ID_MAX);

  if (hdrcallid.user_end_call_id_is_valid)
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_MED, "Ending HDR Call ID %d",
                    hdrcallid.user_end_call_id);
    *call_id_ptr = hdrcallid.user_end_call_id;
    hdrcallid.user_end_call_id_is_valid = FALSE;
  }

  return (ret_val);
}

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
)
{
  hdrcallid.num_active_call_ids = 0;
  hdrcallid.user_end_call_id_is_valid = FALSE;
  hdrcallid.cur_call_id_ptr = NULL;
  hdrcallid_deallocate_all_call_ids();
} /* hdrcallid_init() */

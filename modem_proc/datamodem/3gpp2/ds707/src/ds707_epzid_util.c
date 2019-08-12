
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S   E P Z I D

                                  U T I L 

GENERAL DESCRIPTION
  This file contains utility functions to operate on EPZID 

EXTERNALIZED FUNCTIONS
  ds707_epzid_shrink_epzid_list()
    Function to shrink the epzid list to a known size
  ds707_epzid_copy_epzid()
    Function to copy epzid from source to destination
  ds707_epzid_print_epzid()
    Function to print the contents of epzid
  ds707_epzid_compare_epzids()
    Function to compare two epzids

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2012 by Qualcomm Technologies Incorporated.All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_epzid.c_v   1.2   24 Jan 2003 16:09:22   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_epzid_util.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/11/12   sb      Fixed KW Errors
03/30/12   msh     Remove assert if timer expire when epzid list len < 2 
02/02/12   msh     Support for epzid list len change event
03/15/11   mg      Global variable cleanup
03/04/11   jee     Merged compiler warning fixes
03/02/11   op      Compiler warning fix
10/14/10   sa      Adding debug messages before ASSERT.
02/17/10   ps      Fixed KlocWork errors.
02/12/09   ss      Off Target Lint Fixes
12/18/08   ss      Lint Fixes
08/05/08   ms      IS707B addendum Feature support.
07/01/08   psng    KlocWork Fixes
05/10/07   ac      Lint fix
03/20/07  as/sq    Fixed Lint error and banned apis
09/09/05   vrk/gr  EPZID enhancements releated to L2/L3 ack
04/05/05   gr      Integrated the EPZID functionality into the main line
12/09/04   gr/sk   Initial version 
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "amssassert.h"
#include "nv_items.h"
#include "ds707_epzid.h"
#include "ds707_epzid_util.h"
#include "ds707_epzid_hyst.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "time_svc.h"
#include "time_tod.h"
#include "ds3gmgr.h"
#include "ds707_pkt_mgr.h"
#include "mccsyobj.h"
#include "ps_sys.h"
#include "ps_sys_conf.h"

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
LOCAL ds707_epzid_change_type ds707_epzid_compare_epzids
(
  ds707_epzid_type *epzid_ptr1,
  ds707_epzid_type *epzid_ptr2
);

LOCAL void ds707_epzid_print_epzid 
( 
  byte index
);

LOCAL boolean ds707_epzid_remove_from_list
(
  void
);

static void ds707_epzid_set_epzid_list_len
(
  byte new_list_len
);

static void ds707_epzid_ps_sys_event_update
(
  void
);

/*---------------------------------------------------------------------------
  Variables to maintain the vistied EPZID list.
  The list is organized as an array. The list is sorted by the order in 
  which EPZID zones were visited, with the most recently visited EPZID at the 
  head of the list.
---------------------------------------------------------------------------*/
static byte ds707_epzid_list_len  = 0;           /* Length of the EPZID List    */
static byte ds707_epzid_max_list_len  = 0;       /* List len allowed by BS      */

/* Type of the currently active Enhanced PZID */
static byte ds707_active_epzid_type; //TODO: change to const type 

/* Array to hold Enhanced PZID list     */
static ds707_epzid_type ds707_epzid_list[ DS707_EPZID_MAX_LEN];  
/* The EPZID where the Base Station's A10 is connected with PDSN for this Mobile*/
static ds707_epzid_type ds707_epzid_curr_epzid; 

/*===========================================================================
FUNCTION      DS707_EPZID_INIT_LIST

DESCRIPTION   This function initializes all the list element to invalid value.
			

DEPENDENCIES  List is already created

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_init_list(void)
{
  int list_counter;

  for(list_counter = 0; list_counter<DS707_EPZID_MAX_LEN; list_counter++)
  {
    /* These values will not be used. Store invalid values */
    ds707_epzid_list[list_counter].pzid = INVALID_PZID;                       
    ds707_epzid_list[list_counter].sid  = INVALID_SID;                 
    ds707_epzid_list[list_counter].nid  = INVALID_NID; //check if this valid value  
#ifdef FEATURE_DS_EPZID_HYSTERESIS
    ds707_epzid_list[list_counter].hyst_exp_time  = 0;
    ds707_epzid_list[list_counter].ht_expired = FALSE;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
  }

  ds707_epzid_list_len = 0;

  /* Initialize the current epzid */
  ds707_epzid_curr_epzid.pzid = INVALID_PZID;                       
  ds707_epzid_curr_epzid.sid  = INVALID_SID;                 
  ds707_epzid_curr_epzid.nid  = INVALID_NID; //check if this valid value  
#ifdef FEATURE_DS_EPZID_HYSTERESIS
  ds707_epzid_curr_epzid.hyst_exp_time  = 0;
  ds707_epzid_curr_epzid.ht_expired = FALSE;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

}/*ds707_epzid_init_list() */

/*===========================================================================

FUNCTION       DS707_EPZID_COPY_EPZID

DESCRIPTION    Function to copy the individual fields of EPZID 
               i.e SID/NID/PZID from source EPZID  into dest EPZID

DEPENDENCIES   Valid input pointers

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_copy_epzid 
(
  ds707_epzid_type *dst_epzid_ptr,
  ds707_epzid_type *src_epzid_ptr
)
{ 
/*-------------------------------------------------------------------------*/
  if((dst_epzid_ptr == NULL)|| (src_epzid_ptr == NULL))
  {
    DATA_ERR_FATAL("NULL pointer passed for dst_ptr or src_ptr");
    return;
  }

  dst_epzid_ptr->pzid = src_epzid_ptr->pzid;
  dst_epzid_ptr->sid  = src_epzid_ptr->sid;
  dst_epzid_ptr->nid  = src_epzid_ptr->nid;
#ifdef FEATURE_DS_EPZID_HYSTERESIS
  dst_epzid_ptr->hyst_exp_time  = src_epzid_ptr->hyst_exp_time;
  dst_epzid_ptr->ht_expired     = src_epzid_ptr->ht_expired;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */ 	
}/* ds707_epzid_copy_epzid ()*/

/*===========================================================================

FUNCTION       DS707_EPZID_PRINT_EPZID

DESCRIPTION    Function to print the Entire EPZID Entry. It checks the 
               currently active EPZID type and accordingly prints the EPZID 
               entries

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
LOCAL void ds707_epzid_print_epzid 
(
  byte index
)
{
#ifdef FEATURE_DS_EPZID_HYSTERESIS
  dword  time_to_expire = 0;
  dword uptime         = 0;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */ 
/*-------------------------------------------------------------------------*/
  
  /* Check the range of index */
  if( index >= ds707_epzid_list_len || index >= DS707_EPZID_MAX_LEN )
  {
    DATA_IS707_MSG3(MSG_LEGACY_ERROR, "Out of Range EPZID Index: %d, list_len: %d, max_list_len: %d",
			  index,ds707_epzid_list_len, ds707_epzid_max_list_len);
    return;
  }

  /* Print the contents of an EPZID */
  if( ds707_epzid_get_active_epzid_type() == NV_EPZID_PZID)
  {                                             
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, "EPZID[%d] : PZID %d", index, ds707_epzid_list[index].pzid);
  }                                                               
  else if( ds707_epzid_get_active_epzid_type() == NV_EPZID_PZID_SID)
  {                                                              
    DATA_IS707_MSG3(MSG_LEGACY_HIGH, "EPZID[%d] : PZID %d SID %d",
                          index,
                          ds707_epzid_list[index].pzid,
                          ds707_epzid_list[index].sid );
  }                                                          
  else if( ds707_epzid_get_active_epzid_type() == NV_EPZID_PZID_SID_NID)
  {                             
    DATA_IS707_MSG4(MSG_LEGACY_HIGH,
                    "EPZID[%d] : PZID %d SID %d NID %d",
                    index,
                    ds707_epzid_list[index].pzid,
                    ds707_epzid_list[index].sid,
                    ds707_epzid_list[index].nid );
  }
#ifdef FEATURE_DS_EPZID_HYSTERESIS
  uptime = time_get_uptime_secs();
  if(index > 0)
  {
    if (ds707_epzid_list[index].hyst_exp_time >= uptime)
    {
      time_to_expire = ds707_epzid_list[index].hyst_exp_time - uptime;
    }
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, "Time to expire HT for EPZID[%d]:%d ", 
                    index, time_to_expire);
  }
#endif /* FEATURE_DS_EPZID_HYSTERESIS */ 
 
}/* ds707_epzid_print_epzid() */

/*===========================================================================
FUNCTION      DS707_EPZID_SET_EPZID_LIST_LEN

DESCRIPTION   This function sets the epzid length if passed value is 
              different from the original value and sends event with the new
              value through PS framework
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ds707_epzid_set_epzid_list_len
(
  byte new_list_len
)
{  

  if (new_list_len != ds707_epzid_list_len)
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, "epzid_list_len changed from [%d] to [%d]",
                    ds707_epzid_list_len, new_list_len);

    ds707_epzid_list_len = new_list_len;

    /*------------------------------------------------------------------------
    If epzid list len has changed we must call sys event update after updating 
    the list len 
    ------------------------------------------------------------------------*/
    ds707_epzid_ps_sys_event_update(); 
  }
} /*ds707_epzid_set_epzid_list_len*/

/*===========================================================================
FUNCTION      DS707_EPZID_PS_SYS_EVENT_UPDATE

DESCRIPTION   This function sends PS_SYS_EVENT_3GPP2_EPZID_INFO to PS framework
              It is called when ds707_epzid_list_len is updated.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ds707_epzid_ps_sys_event_update
(
  void
)
{
  int16  ps_error = 0;
  int16  rval = 0;
  ps_sys_3gpp2_epzid_info_type epzid_info;
/*--------------------------------------------------------------*/

  /* Initialize epzid info */
  memset( (void*)&epzid_info, 
          0x0, 
          sizeof(ps_sys_3gpp2_epzid_info_type) );

  /* Set epzid change info of the events */
  epzid_info.epzid_info_val = (uint8) ds707_epzid_list_len;
  rval = ps_sys_conf_set( PS_SYS_TECH_3GPP2, 
                          PS_SYS_CONF_3GPP2_EPZID_INFO,
                          &epzid_info,
                          &ps_error);

  if (( rval != DSS_SUCCESS ) || ps_error )
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR, 
                     "ds707_epzid_ps_sys_event_update(): Failed [%d] to send "
                     "EPZID info event with ps_error:[%d]", 
                     rval, 
                     ps_error);
    return;
  }
  DATA_IS707_MSG0( MSG_LEGACY_MED, 
                   "ds707_epzid_ps_sys_event_update():Sending EPZID info event" );

}

/*===========================================================================
FUNCTION      DS707_EPZID_COMPARE_EPZIDS

DESCRIPTION   Given two EPZIDs, this function compares them and returns
              if there is a change. Return value is an enumeration of 
              epzid_change_type

DEPENDENCIES  Valid input pointers

RETURN VALUE  ds707_epzid_change_type

SIDE EFFECTS  None
===========================================================================*/
LOCAL ds707_epzid_change_type ds707_epzid_compare_epzids
(
  ds707_epzid_type *epzid_ptr1,
  ds707_epzid_type *epzid_ptr2
)
{
  ds707_epzid_change_type epzid_change = EPZID_NO_CHANGE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((epzid_ptr1 == NULL) || (epzid_ptr2 == NULL))
  {
    DATA_ERR_FATAL("NULL pointer passed");
    return epzid_change;
  }

  
  /*-------------------------------------------------------------------------- 
   Three bits are used to represent the EPZID change. 

		    ----------------------------
		   |   |   |   |   | X | Y | Z |
		   ----------------------------
		   X - PZID CHANGE
		   Y - NID  CHANGE
		   Z - SID  CHANGE
  --------------------------------------------------------------------------*/ 
  /*lint -save -e655*/
  if (epzid_ptr1->pzid != epzid_ptr2->pzid)
  {
    epzid_change |= EPZID_PZID_CHANGE; 
  } 
 
  if ((ds707_epzid_get_active_epzid_type() != NV_EPZID_PZID) &&
      (epzid_ptr1->sid != epzid_ptr2->sid))
  {
    epzid_change |= EPZID_SID_CHANGE; 
  }
 
  if ((ds707_epzid_get_active_epzid_type() == NV_EPZID_PZID_SID_NID) &&
      (epzid_ptr1->nid != epzid_ptr2->nid))
  {
    epzid_change |= EPZID_NID_CHANGE; 
  } 
  /*lint -restore Restore lint error 655*/
  return epzid_change;

} /* ds707_epzid_compare_epzids() */

/*===========================================================================
FUNCTION      DS707_EPZID_ADD_EPZID

DESCRIPTION   This function adds a new epzid at location "0". If the list is 
			  full, remove the last entry and make room at location "0"

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_epzid_add_epzid
(
  ds707_epzid_type *epzid_ptr
)
{
  int  list_location;
  byte epzid_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check the range */ 
  if(epzid_ptr == NULL)
  {
    DATA_ERR_FATAL("epzid_ptr is NULL");
    return FALSE;
  }
  
  /* If the entry is already in the list, do not add it */
  list_location = ds707_epzid_is_epzid_in_list(epzid_ptr);
  if (list_location >=0)
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "EPZID already in list at location %d", 
                    list_location);
    ds707_epzid_print_epzid((byte)list_location);
    return FALSE;
  }

  epzid_index = ds707_epzid_list_len;

  /*-----------------------------------------------------------------------
    Shift each entry in the EPZID array to the right so that there is room 
	to put the most recently visited EPZID.
  -----------------------------------------------------------------------*/
  if(epzid_index == ds707_epzid_max_list_len)
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Deleting EPZID[%d] to add new EPZID", epzid_index-1);
    ds707_epzid_print_epzid(epzid_index-1);
  }
  while ( epzid_index > 0)
  {
     ds707_epzid_copy_epzid (
                              &ds707_epzid_list[epzid_index], 
                              &ds707_epzid_list[epzid_index-1]
                            );
     epzid_index--;
  }

  /*-------------------------------------------------------------------------
   Set the first entry in the list to the new pzid
  -------------------------------------------------------------------------*/
  ds707_epzid_copy_epzid(ds707_epzid_list, epzid_ptr);

#ifdef FEATURE_DS_EPZID_HYSTERESIS
  /*-------------------------------------------------------------------------
   Set the Hysteresis Timer on the element at location 1 to the default value
  -------------------------------------------------------------------------*/
  if(ds707_epzid_list_len > 0)
  {
    ds707_epzid_list[1].hyst_exp_time = time_get_uptime_secs() + 
                                        ds707_epzid_hysteresis_get_ht_timer_in_sec();  
  }
  /* No timer associated with current epzid */
  ds707_epzid_list[0].hyst_exp_time = 0;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */ 


  if (ds707_epzid_list_len < ds707_epzid_max_list_len)
  {
    ds707_epzid_set_epzid_list_len (ds707_epzid_list_len + 1 );
  }

  /* Print the contents of the updated EPZID List */
  ds707_epzid_print_epzid_list();

  return TRUE;
}/* ds707_epzid_add_epzid() */


/*===========================================================================
FUNCTION      DS707_EPZID_IS_FIRST_EPZID

DESCRIPTION   This function checks if the epzid is on location "0"

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  TRUE if epzid is on location"0" else FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_epzid_is_first_epzid
(
  ds707_epzid_type *epzid_ptr
) 
{ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(epzid_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL pointer passed");
    return FALSE;
  }
  
  if(ds707_epzid_is_epzid_in_list(epzid_ptr)== 0) 
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}/* ds707_epzid_is_first_epzid */

/*===========================================================================
FUNCTION      DS707_EPZID_GET_EPZID_LIST_LEN

DESCRIPTION   This function returns the length of the EPZID list

DEPENDENCIES  None

RETURN VALUE  length of the EPZID list

SIDE EFFECTS  None
===========================================================================*/
byte ds707_epzid_get_epzid_list_len
(
  void
)
{
  return ds707_epzid_list_len;
}/* ds707_epzid_get_epzid_list_len */

/*===========================================================================
FUNCTION      DS707_EPZID_GET_EPZID_MAX_LIST_LEN

DESCRIPTION   This function returns the length of the EPZID list

DEPENDENCIES  None

RETURN VALUE  length of the EPZID list

SIDE EFFECTS  None
===========================================================================*/
byte ds707_epzid_get_epzid_max_list_len
(
  void
)
{
  return ds707_epzid_max_list_len;
}/* ds707_epzid_get_epzid_max_list_len */

/*===========================================================================
FUNCTION      DS707_EPZID_GET_EPZID_FROM_LIST

DESCRIPTION   This function gets a copy of EPZID indicated by the index.
              Also note that this function function with index 0 can be 
	          used to obtain the last epzid corresponding to the last 
	          origination that was sent.

DEPENDENCIES  curr_epzid pointer is initialized and a valid index 

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_get_epzid_from_list
(
  byte epzid_index, 
  ds707_epzid_type *curr_epzid
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(curr_epzid == NULL)
  {
    DATA_ERR_FATAL("NULL pointer passed");
    return;
  }

  if(epzid_index >= ds707_epzid_list_len || epzid_index >= DS707_EPZID_MAX_LEN )
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                    "epzid_index passed invalid: epzid_index = %d, list_len = %d", 
                    epzid_index, ds707_epzid_list_len);
    ASSERT(0);
    return;
  }

  ds707_epzid_copy_epzid(curr_epzid, &ds707_epzid_list[epzid_index]);

}/* ds707_epzid_get_epzid_from_list */

/*===========================================================================
FUNCTION      DS707_EPZID_IS_EPZID_IN_LIST

DESCRIPTION   This function checks if the epzid is in epzid list

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  Location in the list if already in the list, -1 otherwise

SIDE EFFECTS  None
===========================================================================*/
int ds707_epzid_is_epzid_in_list
(
  ds707_epzid_type *epzid_ptr
)
{
  int   list_location;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    if(epzid_ptr == NULL)
    {
      DATA_ERR_FATAL("NULL pointer passed");
      return -1;
    }
  /*-------------------------------------------------------------------------
    Go through the EPZID list to see if new pzid is already on the list.
    If EPZID was on the list, i would be its index in the EPZID array.
   ------------------------------------------------------------------------*/
  for (list_location = 0; list_location < ds707_epzid_list_len; 
                          list_location++) 
  {
    if (ds707_epzid_compare_epzids(
                   &ds707_epzid_list[list_location],
                   epzid_ptr
                   ) == EPZID_NO_CHANGE)
    {
      break;
    }
  }

  /* If the the epzid is not in the list, return invalid location */
  if (list_location == ds707_epzid_list_len)
  {
    list_location = -1;
  }
    
  return list_location;

}/* ds707_epzid_is_epzid_in_list() */

/*===========================================================================
FUNCTION      DS707_EPZID_PRINT_EPZID_LIST

DESCRIPTION   This function prints the entire epzid list

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_print_epzid_list
(
  void
)
{
  byte i=0;                                /* index to traverse EPZID list */
  byte epzid_list_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  epzid_list_len = ds707_epzid_get_epzid_list_len();
  DATA_IS707_MSG1(MSG_LEGACY_MED, "EPZID List length = %d", epzid_list_len);

  while ( i < ds707_epzid_list_len)
  {
    ds707_epzid_print_epzid(i);
    i += 1;
  }
}/* ds707_epzid_print_epzid_list */

/*===========================================================================
FUNCTION      DS707_EPZID_SET_CURRENT_EPZID

DESCRIPTION   This function sets the current EPZID to the input EPZID.
              Note that this function returns the current EPZID in which we
	          are located which may not be the same as the one in which the 
	          last origination was sent

DEPENDENCIES  epzid pointer is initialized

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_set_current_epzid
(
  ds707_epzid_type *new_epzid_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(new_epzid_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL pointer passed");
    return;
  }  

  new_epzid_ptr->ht_expired = FALSE;
  ds707_epzid_copy_epzid(&ds707_epzid_curr_epzid, new_epzid_ptr);

}/* ds707_epzid_set_current_epzid */

/*===========================================================================
FUNCTION      DS707_EPZID_GET_CURRENT_EPZID_FROM_LIST

DESCRIPTION   This function gets a copy of current epzid.
              Also note that the return value corresponds to the EPZID
	          in which the last origination was sent (top of list, index 0) 
	          which may not be same as the EPZID in which we are in. The 
	          EPZID in which we are currently located can be in any index
	          in the list.

DEPENDENCIES  curr_epzid pointer is initialized

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_get_current_epzid_from_list
(
  ds707_epzid_type *curr_epzid_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(curr_epzid_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL pointer passed");
    return;
  }  
  if (ds707_epzid_list_len != 0)
  {
    ds707_epzid_get_epzid_from_list(0, curr_epzid_ptr);
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_LOW, "EPZID List, Empty max_list_len:%d", 
                    ds707_epzid_max_list_len); 
  }

}/* ds707_epzid_get_current_epzid_from_list() */

/*===========================================================================
FUNCTION      DS707_EPZID_GET_CURRENT_EPZID

DESCRIPTION   This function gets a copy of current epzid

DEPENDENCIES  curr_epzid pointer is initialized

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_get_current_epzid
(
  ds707_epzid_type *curr_epzid_ptr
)
{
  ds707_epzid_type curr_epzid;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(curr_epzid_ptr == NULL)
  {
    DATA_ERR_FATAL("Invalid curr_epzid_ptr passed");
    return;
  }
  if( ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA || 
      ( ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR &&
        ds3g_is_hdr_hybrid_stack_active(SYS_MODEM_AS_ID_1)
      )
    )
  {
    if( ds707_epzid_curr_epzid.pzid == INVALID_PZID &&
        ds707_epzid_curr_epzid.sid == INVALID_SID &&
        ds707_epzid_curr_epzid.nid == INVALID_NID )
    {
      mccsyobj_get_epzid(&curr_epzid.sid, &curr_epzid.nid,
                         &curr_epzid.pzid);
      DATA_IS707_MSG3(MSG_LEGACY_HIGH,"Curr PZID:%d,SID:%d, NID:%d", 
                             curr_epzid.pzid,
                             curr_epzid.sid,
                             curr_epzid.nid);
      ds707_epzid_copy_epzid(curr_epzid_ptr, &curr_epzid);
    }
    else
    {
      ds707_epzid_copy_epzid(curr_epzid_ptr, &ds707_epzid_curr_epzid);
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "Curr Sys mode is not CDMA, Hence return Invalid PZID");
    curr_epzid_ptr->pzid = INVALID_PZID;
    curr_epzid_ptr->sid = INVALID_SID;
    curr_epzid_ptr->nid = INVALID_NID;
  }
  return;

}/* ds707_epzid_get_current_epzid */

/*===========================================================================
FUNCTION      DS707_EPZID_REMOVE_FROM_LIST

DESCRIPTION   This function removes an epzid entry from the bottom of list

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
LOCAL boolean ds707_epzid_remove_from_list
(
  void
)
{
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*----------------------------------------------------------------------
  The list should atleast contain one entry to remove. Otherwise just 
  return FALSE since no entry was removed 
  ----------------------------------------------------------------------*/
  if(ds707_epzid_list_len >0)
  {
    ds707_epzid_list[ds707_epzid_list_len-1].pzid    = INVALID_PZID;
    ds707_epzid_list[ds707_epzid_list_len-1].sid     = INVALID_SID;
    ds707_epzid_list[ds707_epzid_list_len-1].nid     = INVALID_NID;
#ifdef FEATURE_DS_EPZID_HYSTERESIS
    ds707_epzid_list[ds707_epzid_list_len-1].hyst_exp_time  = 0;
    ds707_epzid_list[ds707_epzid_list_len-1].ht_expired = FALSE;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
    ds707_epzid_set_epzid_list_len (ds707_epzid_list_len - 1);
    ret_val = TRUE;
  }
  return ret_val;
}/* ds707_epzid_remove_from_list */

#ifdef FEATURE_DS_EPZID_HYSTERESIS
/*===========================================================================
FUNCTION       DS707_EPZID_PROCESS_TIMER

DESCRIPTION    This function processes the expiry of timer on last epzid
  
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_epzid_process_timer
(
  void
)                                
{
  ds707_epzid_type exp_epzid; 	/* EPZID for which the timer expired */
  ds707_epzid_type curr_epzid; 	/* Current EPZID from MC             */
  boolean reorig_flag = FALSE; 	/* Flag to indicate reorigination 	 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*----------------------------------------------------------------------
  The first entry in the list does not have timer associated with it. Only
  entries in the list starting at location=1 have a timer associated with 
  it. If there are less than two epzids in the list do not reorig timer. 
  ----------------------------------------------------------------------*/
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "ds707_epzid_list_len = %d", ds707_epzid_list_len);
  if( ds707_epzid_list_len <2 )
  {
    return reorig_flag;
  }  
  else if ( ds707_epzid_list_len >= DS707_EPZID_MAX_LEN )
  {
    ASSERT(0);
    return reorig_flag;
  }

  ds707_epzid_copy_epzid(&exp_epzid, &ds707_epzid_list[ds707_epzid_list_len-1]);

  DATA_IS707_MSG1(MSG_LEGACY_MED, "HT Expired EPZID[%d] removed from list", 
                                 ds707_epzid_list_len-1);
  ds707_epzid_print_epzid(ds707_epzid_list_len-1);

  (void) ds707_epzid_remove_from_list();
  
  ds707_epzid_get_current_epzid(&curr_epzid);
  if(ds707_epzid_compare_epzids(&curr_epzid, &exp_epzid) == EPZID_NO_CHANGE)
  {
    /*----------------------------------------------------------------------
	The expired epzid is the same as current epzid, add this epzid on top 
	of the list. This implies that the mobile moved to another EPZID and 
	came back to the original EPZID. Although the timer expired on the 
	current EPZID, it needs to be added back to the list after a re-orig
    ----------------------------------------------------------------------*/
    reorig_flag = ds707_epzid_change_handler(&curr_epzid);
  }
  return reorig_flag;

} /* ds707_epzid_process_timer() */

/*===========================================================================
FUNCTION       DS707_EPZID_GET_TIME_TO_EXPIRE_IN_SEC

DESCRIPTION    This function returns the time to expire on the last epzid
  
DEPENDENCIES   None

RETURN VALUE   Time to expire in seconds

SIDE EFFECTS   None
===========================================================================*/
dword ds707_epzid_get_time_to_expire_in_sec
(
  void
)                                
{
  dword time_to_expire = 0;
  dword uptime         = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* This function should not be called if the list has only one element */
  if( ds707_epzid_list_len <2 ||
      (ds707_epzid_list_len >= DS707_EPZID_MAX_LEN) )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid ds707_epzid_list_len: = %d", 
                                      ds707_epzid_list_len);
    ASSERT(0);
    return 0;
  }
  
  uptime = time_get_uptime_secs();
  if (ds707_epzid_list[ds707_epzid_list_len-1].hyst_exp_time >= uptime)
  {
    time_to_expire = ds707_epzid_list[ds707_epzid_list_len-1].hyst_exp_time 
                                                                - uptime;
  }

  return(time_to_expire);
} /* ds707_epzid_get_time_to_expire_in_sec */
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

/*===========================================================================
FUNCTION       DS707_EPZID_UPDATE_MAX_LIST_LEN

DESCRIPTION    This function updates the max_list_len and cleans up the
               list if the current list length is greater than the updated
               MAX list length.
  
DEPENDENCIES   None

RETURN VALUE   Time to expire in seconds

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_update_max_list_len
(
  byte new_max_list_len
)                                
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* This function should not be called if the list has only one element */
  if(new_max_list_len > DS707_EPZID_MAX_LEN)
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
              "Invalid new_max_list_len: = %d, allowed max_list_len: = %d",
              new_max_list_len, DS707_EPZID_MAX_LEN);
    ASSERT(0);
    return;
  }

  if (new_max_list_len < ds707_epzid_list_len)
  {
    ds707_epzid_shrink_epzid_list(new_max_list_len);
  }

  ds707_epzid_max_list_len = new_max_list_len;
  
} /* ds707_epzid_update_max_list_len() */

/*===========================================================================
FUNCTION       DS707_EPZID_SHRINK_EPZID_LIST

DESCRIPTION    This function shrinks the EPZID list to the the length 
               passed in as a parameter. All the entries past the specified
               list length are cleared.
  
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_shrink_epzid_list
(
 byte new_list_len       /* new length of the list after the shrink        */
)                                
{
  byte epzid_index;       /* Index                                        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*----------------------------------------------------------------------
   If the new list length is greater than the actual list length, the 
   operation is invalid. Bail out with an error message.
   This should never happen 
  ----------------------------------------------------------------------*/
  if(new_list_len >= ds707_epzid_list_len)
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, "List already smaller "
                    "than input list length, orig len %d: new len %d",
                    ds707_epzid_list_len, new_list_len);
    return;
  }

  for (epzid_index = new_list_len; epzid_index < DS707_EPZID_MAX_LEN; epzid_index++) 
  {
    ds707_epzid_list[epzid_index].pzid = INVALID_PZID;
    ds707_epzid_list[epzid_index].sid  = INVALID_SID;
    ds707_epzid_list[epzid_index].nid  = INVALID_NID;

#ifdef FEATURE_DS_EPZID_HYSTERESIS
    ds707_epzid_list[epzid_index].hyst_exp_time = 0;    
    ds707_epzid_list[epzid_index].ht_expired= FALSE;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
  }

  ds707_epzid_set_epzid_list_len (new_list_len);

} /* ds707_epzid_shrink_epzid_list() */
#ifdef FEATURE_IS707B_ADDENDUM
/*===========================================================================
FUNCTION       DS707_EPZID_GET_HAT_TIME_TO_EXPIRE_IN_SEC

DESCRIPTION    This function returns the HAT time to expire
  
DEPENDENCIES   None

RETURN VALUE   Time to expire in seconds

SIDE EFFECTS   None
===========================================================================*/
dword ds707_epzid_get_hat_time_to_expire_in_sec
(
  void
)                                
{
  dword time_to_expire = 0;
  dword uptime         = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  uptime = time_get_uptime_secs();
  if (ds707_epzid_get_hysteresis_hat_timer_is_running())
  {
    time_to_expire = ds707_epzid_hysteresis_get_hat_timer_in_sec()  - 
		                                      ( uptime - ds707_epzid_get_hat_start_time()) ;
  }
  DATA_IS707_MSG1(MSG_LEGACY_MED," Reference HAT value %d",
                  ds707_epzid_hysteresis_get_hat_timer_in_sec());
  return(time_to_expire);
} /* ds707_epzid_get_hat_time_to_expire_in_sec */

/*===========================================================================
FUNCTION      DS707_EPZIDI_MAKE_ENTRY_LAST

DESCRIPTION   This function checks if the epzid is in epzid list and makes it as last entry

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  Location in the list if already in the list, -1 otherwise

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzidi_make_entry_last
(
  int list_location
)
{
  ds707_epzid_type temp_epzid_ptr;
  uint8 i = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(list_location == 0 || list_location == 3)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Cant make the entry located at %d as last", 
                    list_location);
    return ;
  }
  if(ds707_epzid_list_len >= DS707_EPZID_MAX_LEN 
     || list_location >= DS707_EPZID_MAX_LEN)
  {
     DATA_IS707_MSG2(MSG_LEGACY_ERROR, "MAX Len Exceeded. "
                     "ds707_epzid_list_len = %d, list_location = %d", 
                     ds707_epzid_list_len, list_location);
     return ;
  }
  /*-------------------------------------------------------------------------
   First save the Epzid entry in the list that needs to be moved to bottom of the list. 
  -------------------------------------------------------------------------*/
  ds707_epzid_copy_epzid(&temp_epzid_ptr, &ds707_epzid_list[list_location]);
  /*-------------------------------------------------------------------------
   Go through the EPZID from 'list_location' and copy the entries left and finally copy the 
   saved temp_epzid to the last place of the list.
  -------------------------------------------------------------------------*/
  i = list_location;

  while ( i < ds707_epzid_list_len -1 )
  {
    ds707_epzid_copy_epzid (
                            &ds707_epzid_list[i], 
                            &ds707_epzid_list[i+1]
                            );
    i++; 
  }
	
  /*-------------------------------------------------------------------------
   Copy to the Last entry in the list with the epzid located at list_location
  -------------------------------------------------------------------------*/
  ds707_epzid_copy_epzid(&ds707_epzid_list[i], &temp_epzid_ptr); 

  i = ds707_epzid_list_len -1 ;
  ds707_epzid_print_epzid(i );

}/* ds707_epzidi_make_entry_last() */
#endif /* FEATURE_IS707B_ADDENDUM */

/*===========================================================================
FUNCTION      DS707_EPZID_IS_HT_EXPIRED

DESCRIPTION   This function checks if the epzid entry's HT is expired or not.

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_epzid_is_ht_expired( int list_location )
{
  if(list_location >= 0 && list_location < DS707_EPZID_MAX_LEN)
  {
    return(ds707_epzid_list[list_location].ht_expired);
  }
  return FALSE;
}
/*===========================================================================
FUNCTION      DS707_EPZID_SET_HT_EXPIRED

DESCRIPTION   This function sets or clears the epzid entry's HT is expired 
              flag.

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_set_ht_expired( int list_location,boolean flag )
{
  if(list_location >= 0 && list_location < DS707_EPZID_MAX_LEN)
  {
    ds707_epzid_list[list_location].ht_expired = flag;
  }
}

/*===========================================================================
FUNCTION      DS707_EPZID_SET_ACTIVE_EPZID_TYPE

DESCRIPTION   This function sets the value of ds707_active_epzid_type

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_set_active_epzid_type
(
  byte active_epzid_type
)
{
  ds707_active_epzid_type = active_epzid_type;
}

/*===========================================================================
FUNCTION      DS707_EPZID_GET_ACTIVE_EPZID_TYPE

DESCRIPTION   This function gets value of ds707_active_epzid_type

DEPENDENCIES  None

RETURN VALUE  Byte

SIDE EFFECTS  None
===========================================================================*/
byte ds707_epzid_get_active_epzid_type
(
  void
)
{
  return ds707_active_epzid_type;
}

#endif /* FEATURE_DATA_IS707 */

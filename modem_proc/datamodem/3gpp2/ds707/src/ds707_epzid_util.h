#ifndef DS707_EPZID_UTIL_H
#define DS707_EPZID_UTIL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S   E P Z I D  
                                  U T I L

GENERAL DESCRIPTION
  This file contains functions used to handle EPZID changes.

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2002-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_epzid.h_v   1.1   18 Nov 2002 21:33:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_epzid_util.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/14/10   sa      Adding debug messages before ASSERT.
28/12/09   ms      Changes to expose ds707_epzid_get_current_epzid API at
                   VU Level.
02/12/09   ss      Off target Lint Fixes
08/05/08   ms      IS707B addendum Feature support.
04/05/05   gr      Integrated the EPZID functionality into the main line
12/09/04   gr/sk   Initial version of utility funtions
===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_epzid.h"

/*===========================================================================
                                TYPEDEFS 
==========================================================================*/
/*-------------------------------------------------------------------------- 
Three bits are used to represent the EPZID change. 

        ------------------------
       |  |  |  |  | X | Y | Z |
       ------------------------
       X - PZID CHANGE
       Y - NID  CHANGE
       Z - SID  CHANGE
--------------------------------------------------------------------------*/ 
typedef enum
{
  EPZID_NO_CHANGE              = 0x0,
  EPZID_SID_CHANGE             = 0x1,
  EPZID_NID_CHANGE             = 0x2,
  EPZID_SID_NID_CHANGE         = 0x3,
  EPZID_PZID_CHANGE            = 0x4,
  EPZID_PZID_SID_CHANGE        = 0x5,
  EPZID_PZID_NID_CHANGE        = 0x6,
  EPZID_PZID_SID_NID_CHANGE    = 0x7
} ds707_epzid_change_type;

/*===========================================================================
                       EXTERN VARIABLES
===========================================================================*/
#define INVALID_PZID    0xFF    /* Byte size */
#define INVALID_SID     0xFFFF  /* Word size */
#define INVALID_NID     0xFFFF  /* Word size */

/* MACROS TO VERIFY PZID, SID, NID */
#define VERIFY_PZID (pzid) if(pzid == INVALID_PZID)\
                           {\
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid pzid");\
    ASSERT(0)\
                           }
#define VERIFY_SID (sid)   if(sid  == INVALID_SID)\
                           {\
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid sid");\
    ASSERT(0)\
                           }
#define VERIFY_NID(nid)    if(nid  == INVALID_NID)\
                           {\
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid nid");\
    ASSERT(0)\
                           }
/*===========================================================================
                                 EXTERNS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_EPZID_INIT_LIST

DESCRIPTION   This function initializes all the list element to invalid value

DEPENDENCIES  List is already created

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_init_list(void);


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
);

/*===========================================================================
FUNCTION      DS707_EPZID_ADD_EPZID

DESCRIPTION   This function adds a new epzid at location "0". If the list is 
              full, remove the last entry and make room at location "0"

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  TRUE if add succeeded and FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_epzid_add_epzid
(
  ds707_epzid_type *epzid_ptr
);

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
);
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
);

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
);

/*===========================================================================
FUNCTION      DS707_EPZID_GET_EPZID_FROM_LIST

DESCRIPTION   This function gets a copy of EPZID indicated by the index

DEPENDENCIES  curr_epzid pointer is initialized and a valid index 

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_get_epzid_from_list
(
  byte epzid_index, 
  ds707_epzid_type *curr_epzid
);

/*===========================================================================
FUNCTION      DS707_EPZID_IS_EPZID_IN_LIST

DESCRIPTION   This function checks if the epzid is in epzid list

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  location of EPZID if it exists in list, -1 otherwise

SIDE EFFECTS  None
===========================================================================*/
int ds707_epzid_is_epzid_in_list
(
  ds707_epzid_type *epzid_ptr
);

/*===========================================================================
FUNCTION      DS707_EPZID_PRINT_EPZID_LIST

DESCRIPTION   This function prints the entire epzid list

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_print_epzid_list(void);

/*===========================================================================
FUNCTION      DS707_EPZID_SET_CURRENT_EPZID

DESCRIPTION   This function sets the current EPZID to the input parameter

DEPENDENCIES  epzid pointer is initialized

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_set_current_epzid
(
  ds707_epzid_type *new_epzid_ptr
);

/*===========================================================================
FUNCTION      DS707_EPZID_GET_CURRENT_EPZID_FROM_LIST

DESCRIPTION   This function gets a copy of current epzid from the EPZID
              list

DEPENDENCIES  curr_epzid pointer is initialized

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_get_current_epzid_from_list
(
  ds707_epzid_type *curr_epzid
);

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
);                                

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
);                                
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
);                               
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
);             
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
);
/*===========================================================================
FUNCTION      DS707_EPZIDI_MAKE_ENTRY_LAST

DESCRIPTION   This function checks if the epzid is in epzid list and makes it as last entry

DEPENDENCIES  Valid list_location

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzidi_make_entry_last
(
  int list_location
);
#endif /* FEATURE_IS707B_ADDENDUM */

/*===========================================================================
FUNCTION      DS707_EPZID_IS_HT_EXPIRED

DESCRIPTION   This function checks if the epzid entry's HT is expired or not.

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_epzid_is_ht_expired
(
  int   list_location 
);

/*===========================================================================
FUNCTION      DS707_EPZID_SET_HT_EXPIRED

DESCRIPTION   This function set/re-sets the epzid entry's HT  expired flag.

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  void

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_set_ht_expired
(
  int list_location,
  boolean flag 
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_EPZID_UTIL_H */


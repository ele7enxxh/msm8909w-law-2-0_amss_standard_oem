/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsbc.c -- Broadcast Group

  The WMS module which implements the User API for SMS. This source file
  implements the BC group functions which process the client
  commands of the BC group.
  -------------

    Copyright (c) 1998-2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsbc.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)


#include "wmsbc.h"
#include "wmscl.h"
#include "wmsdbg.h"
#include "nv.h"
#include "err.h"
#include "wmsnv.h"
#include "string.h"

#include "wmsutils.h"

#ifdef FEATURE_CDSMS_BROADCAST
#include "wmscm.h"
#endif /* FEATURE_CDSMS_BROADCAST */
#include "wmsmsg.h"

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
#include "cai.h"
#include "caii.h"
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

#include "wmssim.h"
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
#include "wmsts.h"
#include "wmscfg.h"
#ifdef FEATURE_GWSMS_BROADCAST

#include "wmscm_gw.h"

#include "event.h"
#endif /* FEATURE_GWSMS_BROADCAST */
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

#include "wms_modem.h"
#include "stringl.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#ifndef ARRAY_SIZE
#define  ARRAY_SIZE(a) (sizeof((a))/sizeof((a)[0]))
#endif

#define WMS_LTE_DUP_DETECTION_LIST_SIZE 20
#ifdef FEATURE_SMS_PWS_OVER_LTE
wms_cell_info_s_type wms_msg_gw_cb_get_cell_info_by_appInfo(
  wms_prov_app_info_s_type *appInfo_ptr
);
#endif
#ifdef FEATURE_GWSMS_BROADCAST

boolean wms_msg_gw_cb_msg_is_cmas(
  wms_gw_cb_srv_id_type cb_srv_id
);
#endif



/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/


/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE

/* Define a critical section for the broadcast table.
 * This critical section protects the portions of the bc table configuration
 * which can be accessed from outside of the WMS command processing loop via
 * the internal modem APIs (defined in wms_modem.h).  These APIs are read-only
 * in nature, but directly access data which may be partially modified by WMS
 * at the time they are called.  The critical section is used to ensure that
 * the data read by the modem APIs is consistent and not modified while any
 * modem API's are active. See the definition of 'wms_bc_s_type' above.
 */
static rex_crit_sect_type           wms_bc_crit_sect;
static boolean                      wms_bc_crit_sect_initialized = FALSE;

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

/* Define a special outbound buffer type to isolate WMS client data
 * from internal WMS data.  This is done to protect from two specific
 * scenarios:
 *    1) Data tampering by clients causing other clients to receive bad data
 *    2) Clients calling free() on WMS buffers causing WMS to crash later
 *
 * This buffer will be used to hold data passed to the client during the callbacks.
 * Solution for 1) For each client callback called, the data will be re-copied
 *                 into the buffer to ensure each client receives un-tainted data.
 * Solution for 2) The buffer structure is set up so that there is never a valid
 *                 pointer for a client to free().  This will cause any client
 *                 attempting to free to cause an exception in the client's context.
 */
typedef struct wms_bc_outbound_event_s
{
  uint32                      unused_offset_do_not_remove;
  wms_bc_mm_event_info_s_type ev_info;
  union {                                                           // Use largest of:
    wms_bc_mm_service_info_s_type tbl_entries[WMS_BC_MM_TABLE_MAX]; // CDMA, size=128 (   bc_table[WMS_BC_TABLE_MAX])
    wms_bc_mm_srv_id_s_type       srv_entries[WMS_BC_MM_TABLE_MAX]; //   GW, size=190 (gw_cb_table[WMS_BC_MM_TABLE_MAX])
  } u;
} wms_bc_outbound_event_s_type;

/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/
boolean wms_bc_mm_make_table_event(
  wms_prov_app_info_s_type       *appInfo_ptr,
  wms_message_mode_e_type         message_mode,
  wms_bc_mm_table_s_type         *output
);

boolean wms_bc_mm_make_srv_ids_event(
  wms_prov_app_info_s_type       *appInfo_ptr,
  wms_message_mode_e_type         message_mode,
  wms_bc_mm_service_ids_s_type   *output
);

boolean wms_bc_mm_get_presidential_alert_entry_count_in_client_req(
  wms_message_mode_e_type         message_mode,
  wms_bc_mm_srv_id_type          *srv_id_arr
);

wms_cmd_err_e_type wms_bc_mm_delete_single_srv(
  wms_prov_app_info_s_type       *appInfo_ptr,
  wms_message_mode_e_type         message_mode,
  wms_bc_mm_srv_id_type          *srv_id_ptr
);

/*===========================================================================
FUNCTION wms_bc_s_ptr()                                            (INTERNAL)

DESCRIPTION
  Return the pointer to the static BC data.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the internal BC data.

SIDE EFFECTS
  None
===========================================================================*/
wms_bc_s_type * wms_bc_s_ptr(
  void
)
{
  static wms_bc_s_type  bc;

  return &bc;
} /* wms_bc_s_ptr() */

#ifdef FEATURE_SMS_PWS_OVER_LTE
/*===========================================================================
FUNCTION wms_duplist_copy()                                        (INTERNAL)

DESCRIPTION
  Copies a list's contents to a new list.

DEPENDENCIES
  New list must be initialized and allocated.

PARAMETERS
  wms_duplist_list_type    *ret_list [IN]: The list to copy into (sink)
  wms_duplist_list_type    *old_list [IN]: The list to copy (source)

RETURNS
  None

SIDE EFFECTS
  After copying, the new list will:
    * Have the same entries as the old list
    * Entry order will be maintained
    * Indexes will *not* be maintained

  After copying, the old list will:
    * Remain unchanged
===========================================================================*/
void wms_duplist_copy(
  wms_duplist_list_type *ret_list,
  wms_duplist_list_type *old_list
)
{
  MSG_HIGH_2( "In wms_duplist_copy(): 0x%08x 0x%08x",
              ret_list,
              old_list );

  if ( (NULL != ret_list) && (NULL != old_list) )
  {
    // Verify that both lists have entries and that all will fit into the destination
    if ( (NULL != ret_list->entries) &&
         (NULL != old_list->entries) &&
         (ret_list->size >= old_list->entry_count) )
    {
      int copy_parts = 2;
      int new_idx = 0;

      // Both lists have entries and destination is large enough: start copying

      MSG_HIGH_3( "Dest(size=%d), Src(size=%d, count=%d)",
                  ret_list->size,
                  old_list->size,
                  old_list->entry_count );

      do
      {
        int copy_start_idx;
        int copy_end_idx;
        int old_idx;

        if (old_list->start_idx < old_list->end_idx)
        {
          copy_start_idx = old_list->start_idx;
          copy_end_idx   = old_list->end_idx;
          copy_parts = 1;
        }
        else
        {
          if (2 == copy_parts)
          {
            // First chunk
            copy_start_idx = old_list->start_idx;
            copy_end_idx   = old_list->size;
          }
          else
          {
            // Second chunk
            copy_start_idx = 0;
            copy_end_idx   = old_list->end_idx;
          }
        }

        // Copy a chunk
        for ( old_idx = copy_start_idx; old_idx < copy_end_idx; old_idx++ )
        {
          // Check for and skip inactive entries
          if (0 != old_list->entries[old_idx].timestamp)
          {
            // Copy the entry
            ret_list->entries[new_idx++] = old_list->entries[old_idx];
          }
        }

        copy_parts--;

      } while (0 != copy_parts);

      // Update the destination list with the copy results
      ret_list->start_idx = 0;
      ret_list->end_idx = new_idx;
      ret_list->entry_count = new_idx;

      MSG_HIGH_2( "Copy result: Dest(size=%d, count=%d)",
                  ret_list->size,
                  ret_list->entry_count );
    }
    else
    {
      MSG_HIGH_2( "Copy error: won't fit or bad entry list: Dest size=%d, Src count=%d",
                  ret_list->size,
                  old_list->entry_count );
    }
  }
} /* wms_duplist_copy() */

/*===========================================================================
FUNCTION wms_duplist_init_entries()                                (INTERNAL)

DESCRIPTION
  Initializes a list:
    * Initializes the indexes (start, end) to zero
    * Initializes the entry_count to zero
    * Allocates and zeroes the entry list
    * Initializes the list size if entry list is allocated

DEPENDENCIES
  None

PARAMETERS
  wms_duplist_list_type    *list [IN]: The list to initialize
  int                       size [IN]: The number of entries in the list

RETURNS
  boolean
    TRUE:  Entry list allocation successful
    FALSE: Allocation was not successful, list is empty

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_duplist_init_entries(
  wms_duplist_list_type *list_to_init,
  int                    size
)
{
  list_to_init->start_idx = 0;
  list_to_init->end_idx = 0;
  list_to_init->entry_count = 0;

  // Free any previous list contents
  WMS_MEM_FREEIF(list_to_init->entries);

  if (0 < size)
  {
    list_to_init->entries = (wms_duplist_entry_type *)wms_mem_malloc((size_t)size * sizeof(wms_duplist_entry_type));

    if (NULL != list_to_init->entries)
    {
      memset(list_to_init->entries, 0, ((size_t)size * sizeof(wms_duplist_entry_type)));

      list_to_init->size = size;
      return TRUE;
    }
  }

  return FALSE;
} /* wms_duplist_init_entries() */

/*===========================================================================
FUNCTION wms_duplist_destroy()                                     (INTERNAL)

DESCRIPTION
  Destroys a duplist

DEPENDENCIES
  None

PARAMETERS
  wms_duplist_list_type    *list_to_destroy [IN]: The list to destroy

RETURNS
  None

SIDE EFFECTS
  List contents are deallocated
===========================================================================*/
void wms_duplist_destroy(
  wms_duplist_list_type *list_to_destroy
)
{
  if (NULL != list_to_destroy)
  {
    WMS_MEM_FREEIF(list_to_destroy->entries);
  }

  WMS_MEM_FREEIF(list_to_destroy);

  return;
} /* wms_duplist_destroy() */

/*===========================================================================
FUNCTION wms_duplist_create()                                      (INTERNAL)

DESCRIPTION
  Creates a duplist with 'size' entries, copying from the optionally
  provided list.

DEPENDENCIES
  None

PARAMETERS
  int                       size     [IN]: The size of the list
  wms_duplist_list_type    *old_list [IN]: The optional list to
                                           copy into the new list

RETURNS
  wms_duplist_list_type    : The new list

SIDE EFFECTS
  None
===========================================================================*/
wms_duplist_list_type *wms_duplist_create(
  int                    size,
  wms_duplist_list_type *old_list
)
{
  wms_duplist_list_type *ret_list = NULL;

  // Allocate the list body
  ret_list = wms_mem_malloc(sizeof(wms_duplist_list_type));

  if (NULL != ret_list)
  {
    // List body was allocated, initialize it

    memset(ret_list, 0, sizeof(wms_duplist_list_type));

    // If non-zero size, allocate the entry list
    if (0 < size)
    {
      int inited;

      // Allocate and initialize the entries
      inited = wms_duplist_init_entries(ret_list, size);

      if (inited)
      {
        // If given an optional input list, copy it now
        if (NULL != old_list)
        {
          wms_duplist_copy(ret_list, old_list);
        }
      }
      else
      {
        wms_mem_free(ret_list);
        ret_list = NULL;
      }
    }
  }

  return ret_list;
} /* wms_duplist_create() */

/*===========================================================================
FUNCTION wms_duplist_search()                                      (INTERNAL)

DESCRIPTION
  Searches the contents of a duplist for a list entry matching the
  contents of the 'entry' parameter.

  As the list is traversed, any expired entries are removed from the list.
  Expired entries are those where the entry timestamp < 'cur_time'.

  For each non-expired entry found in the list, the message_id of the list
  entry is compared to the message_id in the entry parameter. If a match is
  found, the 'compare_func()' is called with the current list entry and
  the entry parameter.

  When the 'compare_func()' returns TRUE (duplicate detected), the search
  is terminated and TRUE returned.

DEPENDENCIES
  None

PARAMETERS
  wms_duplist_list_type            *list_to_search [IN]: The list to dump
  wms_duplist_entry_type           *entry          [IN]: The entry to search for
  time_t                            cur_time       [IN]: Timestamp of the search
  wms_bc_compare_dup_func_type      compare_func   [IN]: Function to compare entries

RETURNS
  boolean
    TRUE:  Found (duplicate)
    FALSE: Not found

SIDE EFFECTS
  While searching, any expired entries found are removed from the list.
===========================================================================*/
boolean wms_duplist_search(
  wms_duplist_list_type            *list_to_search,
  wms_duplist_entry_type           *entry,
  time_t                            cur_time,
  wms_bc_compare_dup_func_type      compare_func
)
{
  boolean found = FALSE;
  int srch_parts = 2;  // Number of iterations needed to search the array
                       // Depends on index values (linear search or wrapped search)

  if ( (NULL == list_to_search) ||
       (NULL == list_to_search->entries) )
  {
    // no list or empty (unallocated) list
    MSG_HIGH_1("List is NULL(0x%x) or list is not allocated", list_to_search);
    return FALSE; // Invalid search, not found
  }

  if ( (0 == list_to_search->entry_count) ||
       (NULL == entry) ||
       (NULL == compare_func) )
  {
    // empty list (no entries), or nothing to search for
    MSG_HIGH_3( "List is empty(%d), entry is NULL(0x%x), or compare_func is NULL(0x%x)",
                list_to_search->entry_count,
                entry,
                compare_func );

    return FALSE;  // Not found
  }

  do
  {
    int srch_idx         = 0;  // Current search loop index
    int srch_start_idx   = 0;
    int srch_end_idx     = 0;

    // Determine the search boundaries

    if (list_to_search->start_idx < list_to_search->end_idx)
    {
      srch_start_idx = list_to_search->start_idx;
      srch_end_idx   = list_to_search->end_idx;
      srch_parts = 1;
    }
    else
    {
      if (2 == srch_parts)
      {
        // First chunk
        srch_start_idx = list_to_search->start_idx;
        srch_end_idx   = list_to_search->size;
      }
      else
      {
        // Second chunk
        srch_start_idx = 0;
        srch_end_idx   = list_to_search->end_idx;
      }
    }

    // Search a chunk for the serial

    for ( srch_idx = srch_start_idx; srch_idx < srch_end_idx; srch_idx++ )
    {
      // Check for and skip inactive entries
      // An inactive entry has timestamp == 0
      if (0 != list_to_search->entries[srch_idx].timestamp)
      {
        if (cur_time > list_to_search->entries[srch_idx].timestamp)
        {
          MSG_HIGH_2( "Expiring entry: 0x%04x @ 0x%08x",
                      list_to_search->entries[srch_idx].serial_no,
                      list_to_search->entries[srch_idx].timestamp );

          // This entry has expired, remove it from consideration
          //list_to_search->entries[srch_idx].timestamp = 0;
          list_to_search->entry_count--;

          // If the entry removed is the first one in the list,
          // move the list start index forward so the entry is not
          // part of the active list
          if (list_to_search->start_idx == srch_idx)
          {
            list_to_search->start_idx++;

            // Handle the case where the removed entry was the last
            // in the physical array and the start index wraps to
            // the start of the array
            if (list_to_search->start_idx >= list_to_search->size)
            {
              list_to_search->start_idx = 0;
            }
          }
        }
        else
        {
          // Entry has not expired, see if it is a match
          if (entry->message_id == list_to_search->entries[srch_idx].message_id)
          {
            // Message ID matches, do further checks
            MSG_HIGH_3( "Found entry with message_id 0x%04x: ser=%04x, index %d",
                        entry->message_id,
                        entry->serial_no,
                        srch_idx); //LOW

            found = compare_func( &(list_to_search->entries[srch_idx]), entry);

            if (found)
            {
              // This message is a duplicate, stop searching
              break; // out of for loop
            }
          }
        }
      }
    }

    srch_parts--;

  } while ( (0 != srch_parts) && (!found) );

  return found;
} /* wms_duplist_search() */

/*===========================================================================
FUNCTION wms_duplist_add_entry()                                   (INTERNAL)

DESCRIPTION
  Adds an entry to a duplist

DEPENDENCIES
  None

PARAMETERS
  wms_duplist_list_type       *list  [IN]: The list to add to
  wms_duplist_entry_type      *entry [IN]: The entry to add

RETURNS
  wms_duplist_ret_status_t: indicating the status of the add

SIDE EFFECTS
  None
===========================================================================*/
wms_duplist_ret_status_t wms_duplist_add_entry(
  wms_duplist_list_type       *add_list,
  wms_duplist_entry_type      *entry
)
{
  wms_duplist_ret_status_t ret_code = WMS_DUPLIST_SUCCESS;

  if ( (NULL == add_list) || (NULL == entry) )
  {
    return WMS_DUPLIST_BAD_PARAM;
  }

  if (add_list->entry_count < add_list->size)
  {
    // there is space, add to list somewhere

    if (   (add_list->start_idx <  add_list->end_idx) ||
         ( (add_list->start_idx == add_list->end_idx) && (0 == add_list->entry_count) ) )
    {
      // The list does not wrap past the end of the array.
      // The start index is below the end index unless the list is empty

      // 1) [...+++....]  size = 10, count=3, start=3, end=6
      //    [   s      ]  No wrap
      //    [      e   ]
      // or
      // 2) [++++++....]  size = 10, count=6, start=0, end=6
      //    [s         ]  No wrap
      //    [      e   ]
      // or
      // 3) [..........]  size = 10, count=0, start=2, end=2
      //    [  s       ]  No wrap
      //    [  e       ]
      // or
      // 4) [..........]  size = 10, count=0, start=0, end=0
      //    [s         ]  No wrap
      //    [e         ]
      // or
      // 5w)[...++++++.]  size = 10, count=6, start=3, end=9
      //    [   s      ]  Will wrap
      //    [         e]
      // or
      // 6w)[+++.+.+++.]  size = 10, count=7, start=0, end=9
      //    [s         ]  Will wrap
      //    [         e]
      // or
      // 7w)[+++++++++.]  size = 10, count=9, start=0, end=9
      //    [s         ]  Will wrap
      //    [         e]

      add_list->entries[(add_list->end_idx)++] = *entry;
      add_list->entry_count++;
      if (add_list->end_idx >= add_list->size)
      {
        add_list->end_idx = 0;
      }

      // 1) [...++++...]  size = 10, count=4, start=3, end=7
      //    [   s      ]  No wrap
      //    [       e  ]
      // or
      // 2) [+++++++...]  size = 10, count=7, start=0, end=7
      //    [s         ]  No wrap
      //    [       e  ]
      // or
      // 3) [..+.......]  size = 10, count=1, start=2, end=3
      //    [  s       ]  No wrap
      //    [   e      ]
      // or
      // 4) [+.........]  size = 10, count=1, start=0, end=1
      //    [s         ]  No wrap
      //    [ e        ]
      // or
      // 5w)[...+++++++]  size = 10, count=7, start=3, end=0
      //    [   s      ]  Wrap
      //    [e         ]
      // or
      // 6w)[+++.+.++++]  size = 10, count=8, start=0, end=0
      //    [s         ]  Wrap
      //    [e         ]  start == end, count < size: holey
      // or
      // 7w)[++++++++++]  size = 10, count=10, start=0, end=0
      //    [s         ]  Wrap
      //    [e         ]  start == end, count == size: full

      if (add_list->start_idx == add_list->end_idx)
      {
        if (add_list->entry_count == add_list->size)
        {
          ret_code = WMS_DUPLIST_SUCCESS_FULL; // Full  (entry added, needs extension)
        }
        else
        {
          ret_code = WMS_DUPLIST_SUCCESS_HOLEY; // Holey (entry added, needs repack)
        }
      }
    } // The list does not wrap around the end of the array, entry added
    else
    {
      // The list wraps around the end of the array

      // start_idx > end_idx
      // or (start_idx == end_idx) && entry_count != 0    (list full or holey)

      if (add_list->end_idx == add_list->start_idx)
      {
        // List wraps, list full

        // 1)[++++++++++]  size = 10, count=10, start=6, end=6
        //   [      s   ]
        //   [      e   ]  start == end, count == size: full
        // or
        // 2)[++++++++++]  size = 10, count=10, start=0, end=0
        //   [s         ]
        //   [e         ]  start == end, count == size: full
        // or
        // 3)[++..++++++]  size = 10, count=8, start=6, end=6
        //   [      s   ]
        //   [      e   ]  start == end, count < size: holey
        // or
        // 4)[++++.+.+++]  size = 10, count=8, start=0, end=0
        //   [s         ]
        //   [e         ]  start == end, count < size: holey

        if (add_list->entry_count == add_list->size)
        {
          ret_code = WMS_DUPLIST_FAILED_FULL; // Full  (entry not added, needs extension)
        }
        else
        {
          ret_code = WMS_DUPLIST_FAILED_HOLEY; // Holey (entry not added, needs repack)
        }

      } // List wraps, list full
      else
      {
        // ((list->end_idx + 1) <= list->start_idx)
        // Or, end_idx < start_idx

        // List wraps, entry can be added

        // Append
        // 1)[+++...++++]  size = 10, count=7, start=6, end=3
        //   [      s   ]
        //   [   e      ]
        // or
        // 2)[+..++.++++]  size = 10, count=7, start=6, end=5
        //   [      s   ]
        //   [     e    ]
        // or
        // 3)[+++++.++++]  size = 10, count=9, start=6, end=5
        //   [      s   ]
        //   [     e    ]

        add_list->entries[(add_list->end_idx)++] = *entry;
        add_list->entry_count++;

        // 1)[++++..++++]  size = 10, count=8, start=6, end=4
        //   [      s   ]
        //   [    e     ]
        // or
        // 2)[+..+++++++]  size = 10, count=8, start=6, end=6
        //   [      s   ]
        //   [      e   ]  start == end, count < size: holey
        // or
        // 3)[++++++++++]  size = 10, count=10, start=6, end=6
        //   [      s   ]
        //   [      e   ]  start == end, count == size: full

        if (add_list->start_idx == add_list->end_idx)
        {
          if (add_list->entry_count == add_list->size)
          {
            ret_code = WMS_DUPLIST_SUCCESS_FULL; // Full  (entry added, needs extension)
          }
          else
          {
            ret_code = WMS_DUPLIST_SUCCESS_HOLEY; // Holey (entry added, needs repack)
          }
        }

      } // list wraps, entry added

    } // list wraps
  }
  else
  {
    // Indicate NO_SPACE, entry not added
    ret_code = WMS_DUPLIST_FAILED_FULL; // Full (entry not added, needs extension)
  }

  MSG_HIGH_6( "add(ser=%04x, time=%08x), res(%d), idx(start=%d, end=%d), count=%d",
              entry->serial_no, entry->timestamp, ret_code,
              add_list->start_idx, add_list->end_idx, add_list->entry_count );

  return ret_code;
} /* wms_duplist_add_entry() */

/*===========================================================================
FUNCTION wms_duplist_add()                                         (INTERNAL)

DESCRIPTION
  Add an entry to a list, expanding the list if necessary.

  If the list being added to is full, the list will be expanded by 20 elements.

  TODO: Allow the expansion size to be specified.

DEPENDENCIES
  None

PARAMETERS
  wms_duplist_list_type      **list_to_add_to [IN]: The list to add to
  wms_duplist_entry_type      *entry_to_add   [IN]: The entry to add

RETURNS
  wms_duplist_ret_status_t

SIDE EFFECTS
  List may be resized if necessary.
===========================================================================*/
wms_duplist_ret_status_t wms_duplist_add(
  wms_duplist_list_type      **list_to_add_to,
  wms_duplist_entry_type      *entry_to_add
)
{
  wms_duplist_ret_status_t res;

  if ( (NULL == list_to_add_to) || (NULL == *list_to_add_to) || (NULL == entry_to_add) )
  {
    return WMS_DUPLIST_BAD_PARAM;
  }

  res = wms_duplist_add_entry(*list_to_add_to, entry_to_add);

  if ( (WMS_DUPLIST_FAILED_HOLEY == res) || (WMS_DUPLIST_FAILED_FULL == res) )
  {
    // Entry was not added, make bigger list

    wms_duplist_list_type   *new_list = NULL;

    // The list is full, make a new, bigger one from the old

    // Create new list from the old
    new_list = wms_duplist_create( ((*list_to_add_to)->size + WMS_LTE_DUP_DETECTION_LIST_SIZE), *list_to_add_to);

    // Destroy the old
    wms_duplist_destroy(*list_to_add_to);

    // Move new list into old pointer
    *list_to_add_to = new_list;
    new_list = NULL;

    res = wms_duplist_add_entry(*list_to_add_to, entry_to_add);
  }

  return res;
} /* wms_duplist_add() */

/*===========================================================================
FUNCTION wms_duplist_dump()                                        (INTERNAL)

DESCRIPTION
  Dumps the contents of a duplist

DEPENDENCIES
  None

PARAMETERS
  wms_duplist_list_type    *list [IN]: The list to dump

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_duplist_dump(
  wms_duplist_list_type *dump_list
)
{
  int idx;

  MSG_HIGH_4( "List: start_idx=%d, end_idx=%d, count=%d, size=%d",
              dump_list->start_idx,
              dump_list->end_idx,
              dump_list->entry_count,
              dump_list->size );

  for (idx = 0; idx < dump_list->size; idx++)
  {
    MSG_HIGH_5( "[%06d] %c%c: ser=0x%04x, time=0x%08x",
                idx,
                dump_list->start_idx == idx ? 's' : ' ',
                dump_list->end_idx == idx ? 'e' : ' ',
                dump_list->entries[idx].serial_no,
                dump_list->entries[idx].timestamp );
  }
} /* wms_duplist_dump() */

/*===========================================================================
FUNCTION wms_duplist_dump_id()                                     (INTERNAL)

DESCRIPTION
  Dumps the contents of a duplist given the duplist_id

DEPENDENCIES
  None

PARAMETERS
  wms_duplist_ident_t       list_id   [IN]: The list_id of the list to dump

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_duplist_dump_id(
  wms_duplist_ident_t           list_id
)
{
  wms_bc_s_type            *bc_s_ptr = wms_bc_s_ptr();
  wms_duplist_list_type    *list_to_use = NULL;

  if (WMS_LTE_DUPLIST_PRIMARY == list_id)
  {
    list_to_use = bc_s_ptr->primary_lte_duplist;
  }
  else if (WMS_LTE_DUPLIST_SECONDARY == list_id)
  {
    list_to_use = bc_s_ptr->secondary_lte_duplist;
  }
  else if (WMS_LTE_DUPLIST_CMAS == list_id)
  {
    list_to_use = bc_s_ptr->cmas_lte_duplist;
  }

  if (NULL != list_to_use)
  {
    wms_duplist_dump(list_to_use);
  }
} /* wms_duplist_dump_id() */

/*===========================================================================
FUNCTION wms_bc_cleanup()                                          (INTERNAL)

DESCRIPTION
  Release any resources created during startup.
  Used when the WMS task is stopped.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Destroys the duplicate detection lists which were created at startup.
===========================================================================*/
void wms_bc_cleanup(
  void
)
{
  wms_bc_s_type            *bc_s_ptr = wms_bc_s_ptr();

  // Destroy the duplicate detection lists
  wms_duplist_destroy(bc_s_ptr->primary_lte_duplist);
  wms_duplist_destroy(bc_s_ptr->secondary_lte_duplist);
  wms_duplist_destroy(bc_s_ptr->cmas_lte_duplist);

  return;
} /* wms_bc_cleanup() */
#endif /* FEATURE_SMS_PWS_OVER_LTE */

/*===========================================================================
FUNCTION wms_bc_pre_init()                                         (INTERNAL)

DESCRIPTION
  Initialize the BC items which should only be done once at startup.
  Functions wms_bc_init() and wms_bc_gw_cb_init() may be called more
  than once as the bc service is disabled and enabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes wms_bc_crit_sect and associated initialization flag
===========================================================================*/
void wms_bc_pre_init(
  void
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (!wms_bc_crit_sect_initialized)
  {
    wms_bc_crit_sect_initialized = TRUE;
    rex_init_crit_sect(&wms_bc_crit_sect);
  }
  else
  {
    MSG_HIGH_0("wms_bc_crit_sect has been initialized");
  }

#ifdef FEATURE_SMS_PWS_OVER_LTE
  {
    wms_bc_s_type               *bc_s_ptr = wms_bc_s_ptr();

    // Create initial duplicate detection lists
    bc_s_ptr->primary_lte_duplist   = wms_duplist_create(WMS_LTE_DUP_DETECTION_LIST_SIZE, NULL);
    bc_s_ptr->secondary_lte_duplist = wms_duplist_create(WMS_LTE_DUP_DETECTION_LIST_SIZE, NULL);
    bc_s_ptr->cmas_lte_duplist      = wms_duplist_create(WMS_LTE_DUP_DETECTION_LIST_SIZE, NULL);

    if ( (NULL != bc_s_ptr->primary_lte_duplist)    &&
         (NULL != bc_s_ptr->secondary_lte_duplist)  &&
         (NULL != bc_s_ptr->cmas_lte_duplist) )
    {
      MSG_HIGH_3( "LTE Duplicate detection lists have been created. Sizes: Prim=%d, Sec=%d, CMAS=%d",
                  bc_s_ptr->primary_lte_duplist->size,
                  bc_s_ptr->secondary_lte_duplist->size,
                  bc_s_ptr->cmas_lte_duplist->size );
    }
    else
    {
      MSG_HIGH_3( "Failed to create all LTE Duplicate detection lists. Prim=0x%x, Sec=0x%x, CMAS=0x%x",
                  bc_s_ptr->primary_lte_duplist,
                  bc_s_ptr->secondary_lte_duplist,
                  bc_s_ptr->cmas_lte_duplist );
    }
  }

#endif /* FEATURE_SMS_PWS_OVER_LTE */

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
} /* wms_bc_pre_init() */

/*===========================================================================
FUNCTION wms_get_1x_bc_config()                               (WMS_MODEM API)

DESCRIPTION
  This function retrieves a copy of the 1x bc config data. The entire
  set of bc configuration is not returned, only those data which are needed
  to perform a filter/match are returned.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if bc config data is returned, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_get_1x_bc_config(
  wms_1x_bc_config_s_type *bc_conf_ptr
)
{
#ifdef FEATURE_CDSMS_BROADCAST
  wms_prov_app_info_s_type    *appInfo_ptr = wms_get_appinfo_1x();

  if ( bc_conf_ptr != NULL )
  {
    rex_enter_crit_sect(&wms_bc_crit_sect); // MODEM_API: Ensure stable data is being read

    bc_conf_ptr->bc_pref       = appInfo_ptr->bc.cd.bc_pref;
    bc_conf_ptr->bc_enable     = appInfo_ptr->bc.cd.bc_enable;
    bc_conf_ptr->bc_table_size = appInfo_ptr->bc.cd.bc_table_size;

    (void)memscpy( (char *)bc_conf_ptr->bc_table,
                   sizeof(appInfo_ptr->bc.cd.bc_table),
                   (char *)appInfo_ptr->bc.cd.bc_table,
                   sizeof(appInfo_ptr->bc.cd.bc_table) );

    rex_leave_crit_sect(&wms_bc_crit_sect);

    return  TRUE;
  }
#endif
  return FALSE;
} /* wms_get_1x_bc_config() */

#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION wms_bc_enable_check()                                     (INTERNAL)

DESCRIPTION
  Check whether we need to enable or disable BC SMS

DEPENDENCIES
  None

RETURNS
  boolean: TRUE - enable; FALSE - disable

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_bc_enable_check(
  wms_prov_app_info_s_type    *appInfo_ptr
)
{
  boolean                      result      = FALSE;

  MSG_HIGH_1("BC CONF=%d",    appInfo_ptr->bc.cd.bc_config);
  MSG_HIGH_1("BC PREF=%d",    appInfo_ptr->bc.cd.bc_pref);
  MSG_HIGH_1("BC num_sel=%d", appInfo_ptr->bc.cd.bc_num_selected);

  if (WMS_BC_CONFIG_DISALLOW == appInfo_ptr->bc.cd.bc_config)
  {
    /* service provider disabled BC SMS
    */
    result = FALSE;
  }
  else if (WMS_BC_PREF_DEACTIVATE == appInfo_ptr->bc.cd.bc_pref)
  {
    /* user disabled BC SMS
    */
    result = FALSE;
  }
  else if ( (WMS_BC_CONFIG_ALLOW_ALL == appInfo_ptr->bc.cd.bc_config) &&
            (WMS_BC_PREF_ACTIVATE_ALL == appInfo_ptr->bc.cd.bc_pref) )
  {
    /* service provider enabled wild-card and the user activated it too,
       so there is no need to check the table entries
    */
    result = TRUE;
  }
  else if ( (WMS_BC_CONFIG_ALLOW_TABLE == appInfo_ptr->bc.cd.bc_config) ||
            (WMS_BC_PREF_ACTIVATE_TABLE == appInfo_ptr->bc.cd.bc_pref) )
  {
    result = (0 == appInfo_ptr->bc.cd.bc_num_selected) ? FALSE : TRUE;
  }

  return result;
} /* wms_bc_enable_check() */

/*===========================================================================
FUNCTION wms_bc_update_1x_bcsms_setting()                          (INTERNAL)

DESCRIPTION
  Enable/disable 1x broadcast sms

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Calls wms_cm_bc_enable() with the new enable state if the state
  is different from the current state.
===========================================================================*/
void wms_bc_update_1x_bcsms_setting(
  wms_prov_app_info_s_type    *appInfo_ptr
)
{
  boolean bc_enable_cur;
  boolean bc_enable_new;

  bc_enable_cur = appInfo_ptr->bc.cd.bc_enable;   // Save current state of enable
  bc_enable_new = wms_bc_enable_check(appInfo_ptr); // Get new enable state (outside of crit_sect!)

  // If bc_enable flag has changed, update config and notify CM
  if ( bc_enable_cur != bc_enable_new )
  {
    // Update config
    rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
    appInfo_ptr->bc.cd.bc_enable = bc_enable_new;            // Used by modem api
    rex_leave_crit_sect(&wms_bc_crit_sect);

    // Notify CM
    MSG_HIGH_1("BCSMS enable=%d", appInfo_ptr->bc.cd.bc_enable);
    wms_cm_bc_enable(appInfo_ptr->bc.cd.bc_enable);
  }

  return;
} /* wms_bc_update_1x_bcsms_setting() */

/*===========================================================================
FUNCTION wms_bc_search_table_entry()                               (INTERNAL)

DESCRIPTION
  Search the Broadcast Table Entry within the Broadcast Table

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_bc_search_table_entry(
  wms_prov_app_info_s_type              *appInfo_ptr,
  const wms_bc_service_id_s_type        *srv_id,
  boolean                               *found_free,
  boolean                               *found_dup,
  uint32                                *index,
  nv_sms_bc_service_table_type         **service_ptr,
  nv_sms_bc_service_table_options_type **opt_ptr
)
{
  uint32                       i;

  *service_ptr = NULL;
  *found_free = FALSE;
  *found_dup  = FALSE;

  /* search the table for a free entry
  */
  for (i = 0; i < appInfo_ptr->bc.cd.bc_table_size; i++)
  {
    if ((TRUE == appInfo_ptr->bc.cd.bc_table[i].active_service) &&
        ((uint16)srv_id->service == appInfo_ptr->bc.cd.bc_table[i].service) &&
        ((uint8)srv_id->language == appInfo_ptr->bc.cd.bc_table[i].language) )
    {
      *found_dup   = TRUE;
      *index       = i;
      *service_ptr = &appInfo_ptr->bc.cd.bc_table[i];
      *opt_ptr     = &appInfo_ptr->bc.cd.bc_table_options[i];
      break;
    }
    else if (FALSE == appInfo_ptr->bc.cd.bc_table[i].active_service)
    {
      *found_free  = TRUE;
      *index       = i;
      *service_ptr = &appInfo_ptr->bc.cd.bc_table[i];
      *opt_ptr     = &appInfo_ptr->bc.cd.bc_table_options[i];

      
    }
  }

    return;
} /* wms_bc_search_table_entry() */

/*===========================================================================
FUNCTION wms_bc_scpt_data()                                        (INTERNAL)

DESCRIPTION
  Send an internal command to WMS task to process the Broadcast SCPT data

DEPENDENCIES
  None

RETURNS
  wms_status_e_type

SIDE EFFECTS
  None
===========================================================================*/
wms_status_e_type wms_bc_scpt_data(
  const wms_tl_message_type     *tl_ptr,
  wms_msg_transport_type_e_type  transport_type
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_BC_SCPT_DATA;
    cmd_ptr->hdr.client_id   = (uint8)WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_1;
    cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

    cmd_ptr->cmd.bc_scpt_data.cdma_tl = * tl_ptr;
    cmd_ptr->cmd.bc_scpt_data.transport_type = transport_type;

    {
      wms_client_s_type  *client_ptr = wms_client_ptr_fetch(cmd_ptr->hdr.client_id);

      if (NULL != client_ptr)
      {
        cmd_ptr->cmd.bc_scpt_data.ch_pref = client_ptr->ch_pref;
        wms_put_cmd( cmd_ptr );
      }
      else
      {
        st = WMS_INVALID_CLIENT_ID_S;
      }

      WMS_CLIENT_PTR_RELEASEIF(client_ptr);
    }
  }
  else
  {
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;

} /* wms_bc_scpt_data() */

#endif /* FEATURE_CDSMS_BROADCAST */

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
/*===========================================================================
FUNCTION wms_bc_mm_filter_events()                                 (INTERNAL)

DESCRIPTION
  Decide whether this event can be sent to the client or not based on
  power management settings.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  OK to send event to client
  FALSE: Not OK to send event to client

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_bc_mm_filter_events(
  wms_client_id_type        client_id,
  wms_client_s_type        *client_ptr,
  wms_bc_mm_event_e_type    event
)
{
  boolean   ok_to_send_event = FALSE;

  MSG_HIGH_3( "BC_MM: client_id=%d, event=%d, notify_type = %d",
              client_id,
              event,
              client_ptr->bc_mm_notify_type );

  if( WMS_PM_NOTIFY_ALL == client_ptr->bc_mm_notify_type )
  {
    ok_to_send_event = TRUE;
  }
  else if( WMS_PM_NOTIFY_CUSTOM_FILTER == client_ptr->bc_mm_notify_type &&
           ( client_ptr->bc_mm_event_mask & ((uint32)1 << (int)event) ) )
  {
    ok_to_send_event = TRUE;
  }
  else if( WMS_PM_NOTIFY_DEFAULT_FILTER == client_ptr->bc_mm_notify_type )
  {
    switch( event )
    {
      case WMS_BC_MM_EVENT_PREF:
        ok_to_send_event = TRUE;
        break;

      default:
        break;
    }
  }
  else if ( WMS_PM_NOTIFY_NONE == client_ptr->cfg_notify_type )
  {
     ok_to_send_event = TRUE;
  }

  if( FALSE == ok_to_send_event )
  {
    MSG_HIGH_2( "Blocking bc_mm event %d for client %d",
                event,
                client_id );
  }

  return ok_to_send_event;

} /* wms_bc_mm_filter_events() */

/*===========================================================================
FUNCTION wms_bc_prep_and_copy_ob_bc_event()                        (INTERNAL)

DESCRIPTION
  Make a heap-based copy of the outbound event data provided to the function
  wms_bc_mm_event_notify() into a buffer to be sent to clients. The copy is
  made into a specific outbound data structure which allows the data sent to
  the client via callback to be completely disregarded after returning from
  the callback. The buffer is organized in such a fashion so that client
  tampering with the buffer does not impact WMS.

  If a buffer is not allocated yet, it will be allocated before the copy.
  If the buffer is already allocated, it will be zeroed.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  OK to send event to client
  FALSE: Not OK to send event to client

SIDE EFFECTS
  An outbound buffer may be allocated and returned to the caller. This
  buffer must be freed by the caller when appropriate.
===========================================================================*/
boolean wms_bc_prep_and_copy_ob_bc_event(
  wms_bc_mm_event_e_type         event,
  wms_bc_mm_event_info_s_type   *bc_mm_event_info_ptr,
  wms_bc_outbound_event_s_type **ob_evt
)
{
  if ( (NULL == ob_evt) || (NULL == bc_mm_event_info_ptr) )
  {
     return FALSE;
  }

  // Allocate a new buffer or zero the old
  if (NULL == *ob_evt)
  {
    *ob_evt = wms_mem_malloc(sizeof(**ob_evt));
  }

  if (NULL == *ob_evt)
  {
    MSG_HIGH_0("Malloc failed");
    return FALSE;
  }
  else {
    memset(*ob_evt, 0, sizeof(**ob_evt) );
  }

  // Copy event
  (void)memscpy( &((*ob_evt)->ev_info),
                 sizeof(*bc_mm_event_info_ptr),
                 bc_mm_event_info_ptr,
                 sizeof(*bc_mm_event_info_ptr));

  // Copy any extras (service id table, bc table)
  {
    switch(event)
    {
      case WMS_BC_MM_EVENT_TABLE:
      case WMS_BC_MM_EVENT_ADD_SRVS:
      case WMS_BC_MM_EVENT_SELECT_ALL_SRVS:
      case WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS:
        {
          if ( ((*ob_evt)->ev_info.u.bc_mm_table.size > 0) &&
               (NULL != (*ob_evt)->ev_info.u.bc_mm_table.entries) )
          {
            // Fix the table pointer in the copy to point to the union in the outbound structure
            (*ob_evt)->ev_info.u.bc_mm_table.entries = (*ob_evt)->u.tbl_entries;

            // Copy table to outbound structure
            (void)memscpy( (*ob_evt)->ev_info.u.bc_mm_table.entries,
                           sizeof(*((*ob_evt)->ev_info.u.bc_mm_table.entries)) * (*ob_evt)->ev_info.u.bc_mm_table.size,
                           bc_mm_event_info_ptr->u.bc_mm_table.entries,
                           sizeof(*((*ob_evt)->ev_info.u.bc_mm_table.entries)) * (*ob_evt)->ev_info.u.bc_mm_table.size);
          }
        }
        break;

      case WMS_BC_MM_EVENT_SRV_IDS:
      case WMS_BC_MM_EVENT_DELETE_SRVS:
        {
          if ( ((*ob_evt)->ev_info.u.bc_mm_srv_ids.size > 0) &&
               (NULL != (*ob_evt)->ev_info.u.bc_mm_srv_ids.entries) )
          {
            // Fix the table pointer in the copy to point to the union in the outbound structure
            (*ob_evt)->ev_info.u.bc_mm_srv_ids.entries = (*ob_evt)->u.srv_entries;

            // Copy srv_ids to outbound structure
            (void)memscpy( (*ob_evt)->ev_info.u.bc_mm_srv_ids.entries,
                           sizeof(*((*ob_evt)->ev_info.u.bc_mm_srv_ids.entries)) * (*ob_evt)->ev_info.u.bc_mm_srv_ids.size,
                           bc_mm_event_info_ptr->u.bc_mm_srv_ids.entries,
                           sizeof(*((*ob_evt)->ev_info.u.bc_mm_srv_ids.entries)) * (*ob_evt)->ev_info.u.bc_mm_srv_ids.size);
          }
        }
        break;
      default:
        break;
    } // switch(event)
  } // Copy extras

  return TRUE;
} /* wms_bc_prep_and_copy_ob_bc_event() */

/*===========================================================================
FUNCTION wms_bc_mm_event_notify()                                  (INTERNAL)

DESCRIPTION
  Notify all clients of a BC event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_bc_mm_event_notify(
  wms_bc_mm_event_e_type         event,
  wms_bc_mm_event_info_s_type   *bc_mm_event_info_ptr
)
{
  wms_client_s_type            *client_ptr;
  uint8                         i;
  wms_bc_outbound_event_s_type *ob_evt = NULL; // Outbound event data

  MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Notify: %d %s",
                 event,
                 wms_dbg_get_bc_mm_event_string((uint16)event));

  /* Assigning Event Information in bc_mm_event_info_ptr */
  bc_mm_event_info_ptr->event = event;

  /* Notify clients
  */
  for( i=0; i<(uint8)WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr_fetch( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->bc_mm_event_cb )
    {
      if( TRUE == wms_bc_mm_filter_events( (wms_client_id_type) i, client_ptr, event ))
      {
        // Make copy of outbound data FOR EACH CLIENT. The event data cannot be trusted
        // after a client has touched it. This way clients cannot corrupt the downstream
        // client notification data and any data, buffers, or pointers do not have to be
        // trusted or valid after the callback notification and the copy can be simply discarded.
        if ( wms_bc_prep_and_copy_ob_bc_event(event, bc_mm_event_info_ptr, &ob_evt) && (NULL != ob_evt) )
        {
          client_ptr->bc_mm_event_cb(event, &ob_evt->ev_info);
        }
      }
    }
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }

  WMS_MEM_FREEIF(ob_evt);

  return;
} /* wms_bc_mm_event_notify() */
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION wms_bc_enable_failure_proc()                              (INTERNAL)

DESCRIPTION
  Handle the BC Enable Failure and Notify Clients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Broadcast Event is sent to all clients

=========================================================================*/
void wms_bc_enable_failure_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_prov_app_info_s_type    *appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;
  wms_bc_mm_event_e_type       event;
  wms_bc_mm_event_info_s_type *bc_mm_event_ptr = NULL;

  MSG_HIGH_1( "WMS_BC_MM_ENABLE_FAILURE_E: CS_error=%d", cmd_ptr->cmd.bc_enable_failure.sms_cmd_err);

  bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

  if (NULL != bc_mm_event_ptr)
  {
    bc_mm_event_ptr->as_id             = cmd_ptr->hdr.as_id;
    bc_mm_event_ptr->message_mode      = WMS_MESSAGE_MODE_CDMA;
    bc_mm_event_ptr->u.bc_enable_error = WMS_CS_ERROR_S;

    event = appInfo_ptr->bc.cd.bc_enable ? WMS_BC_MM_EVENT_ENABLE_FAILURE :
                                           WMS_BC_MM_EVENT_DISABLE_FAILURE;

    wms_bc_mm_event_notify(event, bc_mm_event_ptr);
  }
  else
  {
    MSG_ERROR_0( "WMS_BC_MM_(EN|DIS)ABLE_FAILURE event not sent: memory allocation failed");
  }

  WMS_MEM_FREEIF(bc_mm_event_ptr);

  return;
} /* wms_bc_enable_failure_proc() */

/*===========================================================================
FUNCTION wms_bc_scpt_data_proc()                                   (INTERNAL)

DESCRIPTION
  Process internal SCPT data command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Broadcast Event is sent to all clients

=========================================================================*/
void wms_bc_scpt_data_proc(
  wms_cmd_type    *cmd_ptr
)
{
  uint32                                 tbl_index = 0;
  uint32                                 scpt_index;
  nv_item_type                           nvi;
  wms_bc_scpt_status_e_type              scpt_status     = WMS_BC_SCPT_STATUS_OK;
  boolean                                found_free      = FALSE;
  boolean                                found_dup       = FALSE;
  nv_sms_bc_service_table_type          *service_ptr     = NULL;
  nv_sms_bc_service_table_options_type  *opt_ptr         = NULL;
  wms_tl_message_type                   *tl_ptr          = &cmd_ptr->cmd.bc_scpt_data.cdma_tl;
  sys_modem_as_id_e_type                 as_id           = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type              *appInfo_ptr     = cmd_ptr->hdr.appInfo_ptr;


  MSG_HIGH_0("WMS_CMD_BC_SCPT_DATA");

  /* ----------------------------------------------------------- */
  /* ----------------------------------------------------------- */
  /* ------------- Now process the SCPT data ------------------- */
  /* ------------------------------------------------------------*/
  /* ----------------------------------------------------------- */

  for( scpt_index = 0;
       scpt_index < tl_ptr->cl_bd.scpt_data_ptr->num_entries;
       scpt_index ++ )
  {
    scpt_status = WMS_BC_SCPT_STATUS_OK;

    switch( tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].op_code )
    {
      /* --------------- */
      /* --- OP_ADD ---- */
      /* --------------- */
      case WMS_BC_SCPT_OP_ADD:
        if (appInfo_ptr->bc.cd.bc_num_active == WMS_BC_TABLE_MAX)
        {
          /* table is full
          */
          scpt_status = WMS_BC_SCPT_STATUS_SERVICE_MEMORY_FULL;
        }
        else
        {
          wms_bc_search_table_entry( appInfo_ptr,
                                     &tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].srv_id,
                                     &found_free,
                                     &found_dup,
                                     &tbl_index,
                                     &service_ptr,
                                     &opt_ptr );

          if( found_dup == TRUE )
          {
            scpt_status = WMS_BC_SCPT_STATUS_ALREADY_PROGRAMMED;
          }
          else
          {
            if( found_free == FALSE )
            {
              /* there is no free entry within the current table;
                 use the free entry just after the current table
              */
              tbl_index   = appInfo_ptr->bc.cd.bc_table_size; // Bounds checked implicitly by num_active < MAX
              service_ptr = &appInfo_ptr->bc.cd.bc_table[tbl_index];
              opt_ptr     = &appInfo_ptr->bc.cd.bc_table_options[tbl_index];

            }
            /* else, use the free entry found earlier */

            rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write

            /* update the entry and bc info
            */
            service_ptr->index          = (uint8)tbl_index;
            service_ptr->active_service = TRUE;
            service_ptr->service        = (uint16)tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].srv_id.service;
            service_ptr->language       = (uint8)tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].srv_id.language;
            service_ptr->selected       = TRUE;
            service_ptr->priority       = (uint8)WMS_PRIORITY_NORMAL;

            (void)memscpy( (char *) service_ptr->label,
                           WMS_BC_MM_SRV_LABEL_SIZE,
                           tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].label,
                           WMS_BC_MM_SRV_LABEL_SIZE );

            if (FALSE == found_free) {
              // Increase the table size if extending table (not using a free one)
              appInfo_ptr->bc.cd.bc_table_size++;
            }

            rex_leave_crit_sect(&wms_bc_crit_sect);

            opt_ptr->index          = service_ptr->index;     // Not used by modem api
            opt_ptr->label_encoding = (uint8)tl_ptr->cl_bd.scpt_data_ptr->encoding;     // Not used by modem api
            opt_ptr->bc_alert       = (uint8)tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].bc_alert; // Not used by modem api
            opt_ptr->max_messages   = tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].max_messages;     // Not used by modem api

            /* We have one more active entry
            */
            appInfo_ptr->bc.cd.bc_num_active++;    // Not used by modem api
            appInfo_ptr->bc.cd.bc_num_selected++;  // Not used by modem api

            /* write to NV
            */
            nvi.sms_bc_service_table = * service_ptr;
            MSG_HIGH_2( "Update service table entry %d with index = %d",
                        tbl_index,
                        service_ptr->index );
            if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, &nvi )
                != NV_DONE_S )
            {
              scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
            }
            else
            {
              nvi.sms_bc_service_table_options = * opt_ptr;
              if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_OPTIONS_I,
                                     &nvi ) != NV_DONE_S )
              {
                scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
              }
              else
              {
                nvi.sms_bc_service_table_size = appInfo_ptr->bc.cd.bc_table_size;
                if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_SIZE_I, &nvi )
                    != NV_DONE_S )
                {
                  scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
                }
                else
                {
                  wms_bc_mm_event_info_s_type           *bc_mm_event_ptr = NULL;

                  bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

                  if (NULL != bc_mm_event_ptr)
                  {
                    boolean  table_ok;

                    bc_mm_event_ptr->as_id        = as_id;
                    bc_mm_event_ptr->message_mode = WMS_MESSAGE_MODE_CDMA;

                    table_ok = wms_bc_mm_make_table_event( appInfo_ptr,
                                                           WMS_MESSAGE_MODE_CDMA,
                                                           &bc_mm_event_ptr->u.bc_mm_table );
                    if (table_ok)
                    {
                      wms_bc_update_1x_bcsms_setting(appInfo_ptr);

                      // SCPT data is Ok, send the event to client
                      wms_bc_mm_event_notify(WMS_BC_MM_EVENT_SCPT_ADD_SRVS, bc_mm_event_ptr);

                      WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_srv_ids.entries);
                    }
                  }
                  else
                  {
                    MSG_ERROR_0( "WMS_BC_MM_EVENT_SCPT_ADD_SRVS event not sent: memory allocation failed");
                  }

                  WMS_MEM_FREEIF(bc_mm_event_ptr);
                }
              }
            }
          }
        }
        break;

      /* --------------- */
      /* -- OP_DELETE -- */
      /* --------------- */
      case WMS_BC_SCPT_OP_DELETE:
      {
        wms_bc_mm_srv_id_type srv_id;
        srv_id.bc_srv_id = tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].srv_id;
        if (0 == wms_bc_mm_get_presidential_alert_entry_count_in_client_req(WMS_MESSAGE_MODE_CDMA, &srv_id))
        {
          wms_cmd_err_e_type cmd_err = wms_bc_mm_delete_single_srv( appInfo_ptr,
                                                                    WMS_MESSAGE_MODE_CDMA,
                                                                    &srv_id);

          if (WMS_CMD_ERR_BC_BAD_SRV_ID == cmd_err)
          {
            scpt_status =  WMS_BC_SCPT_STATUS_NOT_YET_PROGRAMMED;
          }
          else if (WMS_CMD_ERR_BC_NV_WRITE == cmd_err)
          {
            scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
          }
          else
          {
            wms_bc_mm_event_info_s_type           *bc_mm_event_ptr = NULL;

            bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

            if (NULL != bc_mm_event_ptr)
            {
              boolean  table_ok;

              bc_mm_event_ptr->as_id        = as_id;
              bc_mm_event_ptr->message_mode = WMS_MESSAGE_MODE_CDMA;

              table_ok = wms_bc_mm_make_srv_ids_event( appInfo_ptr,
                                                       WMS_MESSAGE_MODE_CDMA,
                                                       &bc_mm_event_ptr->u.bc_mm_srv_ids );
              if (table_ok)
              {
                wms_bc_update_1x_bcsms_setting(appInfo_ptr);

                // SCPT data is Ok, send the event to client
                wms_bc_mm_event_notify(WMS_BC_MM_EVENT_SCPT_DELETE_SRVS, bc_mm_event_ptr);

                WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_srv_ids.entries);
              }
            }
            else
            {
              MSG_ERROR_0( "WMS_BC_MM_EVENT_SCPT_DELETE_SRVS event not sent: memory allocation failed");
            }

            WMS_MEM_FREEIF(bc_mm_event_ptr);
          }
        }
      }
      break;

      /* ------------------ */
      /* -- OP_DELETE_ALL-- */
      /* ------------------ */
      case WMS_BC_SCPT_OP_DELETE_ALL:
        {
          uint32 i;
          wms_bc_mm_srv_id_type  srv_id;

          /* delete all entries from table
          */
          for( i=0; i < appInfo_ptr->bc.cd.bc_table_size; i++ )
          {
            service_ptr = &appInfo_ptr->bc.cd.bc_table[i];

            /* If this table entry is active, delete it
            */
            if( service_ptr->active_service == TRUE )
            {
              srv_id.bc_srv_id.service = (wms_service_e_type)service_ptr->service;
              /* If this table entry is active, delete it
              */
              if( 0 == wms_bc_mm_get_presidential_alert_entry_count_in_client_req(WMS_MESSAGE_MODE_CDMA, &srv_id) )
              {
                /* de-activate this entry
                */
                rex_enter_crit_sect(&wms_bc_crit_sect);  // Exclusive write
                service_ptr->active_service = FALSE; // Used by modem api
                appInfo_ptr->bc.cd.bc_num_active --;          // Not used by modem api

                if (TRUE == service_ptr->selected)
                {
                  appInfo_ptr->bc.cd.bc_num_selected --;      // Not used by modem api
                  service_ptr->selected = FALSE;     // Used by modem api
                }
                rex_leave_crit_sect(&wms_bc_crit_sect);

                /* write to NV
                */
                nvi.sms_bc_service_table = * service_ptr;
                MSG_HIGH_2( "Update service table entry %d with index = %d",
                            i,
                            service_ptr->index );
                if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, &nvi )
                    != NV_DONE_S )
                {
                  scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
                }
              }

            } // IF NULL Check
          }  // for

          if( scpt_status == WMS_BC_SCPT_STATUS_OK )
          {
            /* Delete all was successful
            */
            MSG_HIGH_0("BC Table Delete All OK");

            /* update local data from new NV item
            */
            wms_bc_update_1x_bcsms_setting(appInfo_ptr);

            /* Send the event to client
            */
            {
              wms_bc_mm_event_info_s_type           *bc_mm_event_ptr = NULL;

              bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

              if (NULL != bc_mm_event_ptr)
              {
                bc_mm_event_ptr->as_id        = as_id;
                bc_mm_event_ptr->message_mode = WMS_MESSAGE_MODE_CDMA;

                (void) wms_bc_mm_make_srv_ids_event( appInfo_ptr,
                                                     WMS_MESSAGE_MODE_CDMA,
                                                     &bc_mm_event_ptr->u.bc_mm_srv_ids);

                wms_bc_mm_event_notify(WMS_BC_MM_EVENT_SCPT_DELETE_ALL_SRVS, bc_mm_event_ptr);

                WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_srv_ids.entries);
              }
              else
              {
                MSG_ERROR_0( "WMS_BC_MM_EVENT_SCPT_DELETE_ALL_SRVS event not sent: memory allocation failed");
              }

              WMS_MEM_FREEIF(bc_mm_event_ptr);
            }
          }
        }
        break;


      default:
        /* Bad op code; skip this entry */
        scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
        break;

    } /* switch */

    /* update SCPT result
    */
    tl_ptr->cl_bd.scpt_result_ptr->entry[scpt_index].srv_id =
                            tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].srv_id;
    tl_ptr->cl_bd.scpt_result_ptr->entry[scpt_index].status = scpt_status;

  } /* for */


  /* ----------------------------------------------------------- */
  /* ----------------------------------------------------------- */
  /* ------------- Send SCPT result to network ----------------- */
  /* ------------------------------------------------------------*/
  /* ----------------------------------------------------------- */

  tl_ptr->cl_bd.mask = WMS_MASK_BD_MSG_ID | WMS_MASK_BD_SCPT_RESULT;
  tl_ptr->is_mo = TRUE;
  tl_ptr->cl_bd.message_id.type = WMS_BD_TYPE_SUBMIT;
  /* use the same message id number as in the incoming message */

  tl_ptr->cl_bd.scpt_result_ptr->num_entries = tl_ptr->cl_bd.scpt_data_ptr->num_entries;
  /* results for each operations were filled in already */

  /* Send the result to the network
  */
  wms_msg_send_scpt_result( cmd_ptr, tl_ptr );

  return;
} /* wms_bc_scpt_data_proc() */

/*===========================================================================
FUNCTION wms_bc_init()                                             (INTERNAL)

DESCRIPTION
  Do BC-related initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  BC data initialized.
===========================================================================*/
void wms_bc_init(
  void
)
{
  uint16                    i;
  nv_item_type              nvi;
  wms_bc_pref_e_type        local_bc_pref;
  wms_bc_config_e_type      local_bc_config;
  word                      local_bc_table_size;
  uint16                    local_bc_num_selected;
  wms_prov_app_info_s_type *appInfo_ptr = wms_get_appinfo_1x();

  MSG_HIGH_0("wms_bc_init()");

  /* --------------- read all BC SMS items from NV ---------------- */

  /* NV_SMS_BC_CONFIG_I:
  */
  (void) wms_nv_read_wait( NV_SMS_BC_CONFIG_I, &nvi );
  local_bc_config = (wms_bc_config_e_type) nvi.sms_bc_config;

  if (WMS_BC_CONFIG_DISALLOW == local_bc_config)
  {
    MSG_HIGH_0("BC SMS disabled in NV");

    rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
    appInfo_ptr->bc.cd.bc_config = WMS_BC_CONFIG_DISALLOW;
    appInfo_ptr->bc.cd.bc_pref = WMS_BC_PREF_DEACTIVATE;
    appInfo_ptr->bc.cd.bc_table_size = 0;
    rex_leave_crit_sect(&wms_bc_crit_sect);

    // Write bc_pref to NV
    nvi.sms_bc_user_pref = (uint8)WMS_BC_PREF_DEACTIVATE;
    (void)wms_nv_write_wait( NV_SMS_BC_USER_PREF_I, &nvi );

    // Write table size to NV
    nvi.sms_bc_service_table_size = 0;
    (void)wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_SIZE_I, &nvi );

    return;  /* SHORT-RETURN */
  }


  /* NV_SMS_BC_USER_PREF_I:
  */
  (void) wms_nv_read_wait( NV_SMS_BC_USER_PREF_I, &nvi );
  local_bc_pref = (wms_bc_pref_e_type) nvi.sms_bc_user_pref;

  /* ----------- Do some checking on the preference ----------- */
  if ( (WMS_BC_CONFIG_ALLOW_TABLE == local_bc_config) &&
       (WMS_BC_PREF_ACTIVATE_ALL == local_bc_pref) )
  {
    MSG_HIGH_0("Enforcing BC Pref to TableOnly");

    local_bc_pref = WMS_BC_PREF_ACTIVATE_TABLE;

    // Write user_pref to NV
    nvi.sms_bc_user_pref = (uint8)WMS_BC_PREF_ACTIVATE_TABLE;
    (void)wms_nv_write_wait( NV_SMS_BC_USER_PREF_I, &nvi );
  }

  /* NV_SMS_BC_SERVICE_TABLE_SIZE_I:
  */
  (void) wms_nv_read_wait( NV_SMS_BC_SERVICE_TABLE_SIZE_I, &nvi );
  local_bc_table_size = nvi.sms_bc_service_table_size;

  /* determine how many services are currently selected while traversing
     the whole table:
  */
  local_bc_num_selected = 0;

  // Temporarily make the table appear empty to those reading it during update
  rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
  appInfo_ptr->bc.cd.bc_table_size = 0;
  rex_leave_crit_sect(&wms_bc_crit_sect);

  // Read the table entries
  for ( i=0; i < local_bc_table_size && i < WMS_BC_TABLE_MAX; i++ )
  {
    nv_sms_bc_service_table_type  *pLocalTableEntry;

    // Read a table entry
    nvi.sms_bc_service_table.index = (uint8)i;
    (void) wms_nv_read_wait( NV_SMS_BC_SERVICE_TABLE_I, &nvi );

    pLocalTableEntry = &nvi.sms_bc_service_table;

    if ( (TRUE == pLocalTableEntry->active_service) &&
         (TRUE == pLocalTableEntry->selected) )
    {
      local_bc_num_selected++;
    }

    //rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
    // OK to write to bc_table entry outside of CS here because
    // table_size==0 at the moment and clients wont see this update
    appInfo_ptr->bc.cd.bc_table[i] = nvi.sms_bc_service_table;  // Used by modem api
    //rex_leave_crit_sect(&wms_bc_crit_sect);

  }

  rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
  appInfo_ptr->bc.cd.bc_pref         = local_bc_pref;          // Used by modem api
  // Re-enable client table traversal by updating actual table length
  appInfo_ptr->bc.cd.bc_table_size   = local_bc_table_size;    // Used by modem api
  rex_leave_crit_sect(&wms_bc_crit_sect);
  appInfo_ptr->bc.cd.bc_config       = local_bc_config;        // Not used by modem api
  appInfo_ptr->bc.cd.bc_num_selected = local_bc_num_selected;  // Not used by modem api

  return;
} /* wms_bc_init */

void wms_bc_online_proc(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  boolean                 bc_enable_new;

  wms_bc_init();

  /* ---- send BC enable command to lower layers ---- */
  bc_enable_new = wms_bc_enable_check(appInfo_ptr); // Get new enable state (outside of crit_sect!)

  rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
  appInfo_ptr->bc.cd.bc_enable = bc_enable_new;
  rex_leave_crit_sect(&wms_bc_crit_sect);

  MSG_HIGH_1("BCSMS enable=%d", appInfo_ptr->bc.cd.bc_enable);

  wms_cm_bc_enable( appInfo_ptr->bc.cd.bc_enable );

  return;
} /* wms_bc_online_proc() */

/*=========================================================================
FUNCTION wms_bc_addr_match()                                (WMS_MODEM API)

DESCRIPTION
  This function checks the BC_ADDR of a received broadcast page or
  broadcast message to see if it passes the filter.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - the mobile should process the received message
  FALSE - the mobile should discard the received message

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_bc_addr_match(
  const uint8 *bc_addr_ptr
)
{
  wms_prov_app_info_s_type    *appInfo_ptr = wms_get_appinfo_1x();
  boolean                      result      = FALSE;
  uint16                       i;
  wms_priority_e_type          priority;
  uint8                        msg_id;
  uint8                        zone_id;
  wms_service_e_type           service;
  wms_language_e_type          language;

  /* Decode the data first.  See TIA/EIA-637A 3.3.1.
       byte0: | priority(2) | msg_id(6) |
       byte1: | zone_id(8)              |
       byte2: | service (16)            |
       byte3: |                         |
       byte4: | language(8)             |
  */
  priority = (wms_priority_e_type) ( ( bc_addr_ptr[0] & 0xc0 ) >> 6 );
  msg_id   = ( bc_addr_ptr[0] & 0x3f );
  zone_id  = bc_addr_ptr[1];
  service  = (wms_service_e_type) ( ( bc_addr_ptr[2] << 8 ) | bc_addr_ptr[3] );
  language = (wms_language_e_type) bc_addr_ptr[4];

  MSG_HIGH_0("----BC_ADDR----");
  MSG_HIGH_1("BC srv=0x%lx",  service);
  MSG_HIGH_1("BC lan=%d",     language);
  MSG_HIGH_1("BC pri=%d",     priority);
  MSG_HIGH_1("BC msg_id=%d",  msg_id);
  MSG_HIGH_1("BC zone_id=%d", zone_id);

  /* --------- Now do the filtering --------- */

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return FALSE;
  }

  rex_enter_crit_sect(&wms_bc_crit_sect); // MODEM_API: Ensure stable data is being read

  if (FALSE == appInfo_ptr->bc.cd.bc_enable)
  {
    result = FALSE;
  }
  else if (WMS_BC_PREF_ACTIVATE_ALL == appInfo_ptr->bc.cd.bc_pref)
  {
    result = TRUE;
  }
  else
  {
    /* Now we need to check all table entries:
    */
    for ( i=0; i < appInfo_ptr->bc.cd.bc_table_size && i < WMS_BC_TABLE_MAX; i++ )
    {
      if ( (TRUE == appInfo_ptr->bc.cd.bc_table[i].active_service) &&
           (TRUE == appInfo_ptr->bc.cd.bc_table[i].selected) &&
           ((word) service == appInfo_ptr->bc.cd.bc_table[i].service) &&
           ((uint8) language == appInfo_ptr->bc.cd.bc_table[i].language) &&
           ((uint8) priority >= appInfo_ptr->bc.cd.bc_table[i].priority) )
      {
        result = TRUE;
        break;
      }
    }
  }

  rex_leave_crit_sect(&wms_bc_crit_sect);

  if (TRUE == result)
  {
    MSG_HIGH_0("bc_addr MATCHED");
  }
  else
  {
    MSG_HIGH_0("bc_addr NO match");
  }

  return result;
} /* wms_bc_addr_match */

#endif /* FEATURE_CDSMS_BROADCAST */

/*===========================================================================
 *===========================================================================
 *===========================================================================
 *===========================================================================

===========================================================================*
===========================================================================*
===========================================================================*
===========================================================================*/

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
/*===========================================================================
FUNCTION wms_bc_mm_get_active_table_size()                         (INTERNAL)

DESCRIPTION
  Get the Active Table size for the Broadcast Table based on the mode.

DEPENDENCIES
  None

RETURN VALUE
  Size of the Table

SIDE EFFECTS
  None
===========================================================================*/
uint32 wms_bc_mm_get_active_table_size(
  wms_prov_app_info_s_type  *appInfo_ptr,
  wms_message_mode_e_type    message_mode
)
{
  uint32           i;
  uint32           size = 0;

  if (message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    for ( i = 0; i < appInfo_ptr->bc.gw.gw_cb_table_size; i++ )
    {
      if (TRUE == appInfo_ptr->bc.gw.gw_cb_table[i].active_service)
      {
        size ++;
      }
    }
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    for ( i=0; i < appInfo_ptr->bc.cd.bc_table_size; i++ )
    {
      if (TRUE == appInfo_ptr->bc.cd.bc_table[i].active_service)
      {
        size ++;
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }

  return size;
} /* wms_bc_get_active_table_size() */

/*===========================================================================
FUNCTION wms_bc_mm_make_table_event()                              (INTERNAL)

DESCRIPTION
  Use the stored information to make a table event.

DEPENDENCIES
  None

RETURNS
  TRUE:  Successful
  FALSE: Failed

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_bc_mm_make_table_event(
  wms_prov_app_info_s_type  *appInfo_ptr,
  wms_message_mode_e_type    message_mode,
  wms_bc_mm_table_s_type    *output
)
{
  uint32           i;
  uint32           j;
  boolean          st = TRUE;

  if (NULL == output)
  {
    wms_check_for_null_ptr();
    return FALSE;
  }
  /* Allocate enough memory for the output */
  output->size = (uint16)wms_bc_mm_get_active_table_size( appInfo_ptr, message_mode );
  output->entries = wms_mem_malloc( output->size * sizeof(wms_bc_mm_service_info_s_type) );

  if( output->entries == NULL )
  {
    if (output->size > 0)
    {
      st = FALSE;
    }
  }
  else
  {
    if( message_mode == WMS_MESSAGE_MODE_GW )
    {
#ifdef FEATURE_GWSMS_BROADCAST
      for( i=0, j=0; i < appInfo_ptr->bc.gw.gw_cb_table_size; i++ )
      {
        if (TRUE == appInfo_ptr->bc.gw.gw_cb_table[i].active_service)
        {
          output->entries[j].srv_id.gw_cb_range.from  = appInfo_ptr->bc.gw.gw_cb_table[i].service_from;
          output->entries[j].srv_id.gw_cb_range.to    = appInfo_ptr->bc.gw.gw_cb_table[i].service_to;
          output->entries[j].selected                 = appInfo_ptr->bc.gw.gw_cb_table[i].selected;
          output->entries[j].priority                 = WMS_PRIORITY_NORMAL;
          (void)memscpy( output->entries[j].label,
                         WMS_BC_MM_SRV_LABEL_SIZE,
                         (char *) appInfo_ptr->bc.gw.gw_cb_table[i].label,
                         WMS_BC_MM_SRV_LABEL_SIZE );
          output->entries[j].label_encoding   = (wms_user_data_encoding_e_type) appInfo_ptr->bc.gw.gw_cb_table[i].label_encoding;
          output->entries[j].alert            = (wms_bc_alert_e_type) appInfo_ptr->bc.gw.gw_cb_table[i].bc_alert;
          output->entries[j].max_messages     = appInfo_ptr->bc.gw.gw_cb_table[i].max_messages;
          j++;
        }
      }
      output->size = (uint16)j;
#endif /* FEATURE_GWSMS_BROADCAST */
    }
    else
    {
#ifdef FEATURE_CDSMS_BROADCAST
      for( i=0, j=0; i < appInfo_ptr->bc.cd.bc_table_size; i++ )
      {
        if (TRUE == appInfo_ptr->bc.cd.bc_table[i].active_service)
        {
          output->entries[j].srv_id.bc_srv_id.service  = (wms_service_e_type) appInfo_ptr->bc.cd.bc_table[i].service;
          output->entries[j].srv_id.bc_srv_id.language = (wms_language_e_type) appInfo_ptr->bc.cd.bc_table[i].language;
          output->entries[j].selected                  = appInfo_ptr->bc.cd.bc_table[i].selected;
          output->entries[j].priority                  = (wms_priority_e_type) appInfo_ptr->bc.cd.bc_table[i].priority;
          (void)memscpy( output->entries[j].label,
                         WMS_BC_MM_SRV_LABEL_SIZE,
                         (char *) appInfo_ptr->bc.cd.bc_table[i].label,
                         WMS_BC_MM_SRV_LABEL_SIZE );

          {
            nv_sms_bc_service_table_options_type * opt_ptr = &appInfo_ptr->bc.cd.bc_table_options[i];

            output->entries[j].label_encoding = (wms_user_data_encoding_e_type) opt_ptr->label_encoding;
            output->entries[j].alert          = (wms_bc_alert_e_type) opt_ptr->bc_alert;
            output->entries[j].max_messages   = opt_ptr->max_messages;
          }
          j++;
        }
      }
      output->size = (uint16)j;
#endif /* FEATURE_CDSMS_BROADCAST */
    }
  }

  return st;
} /* wms_bc_mm_make_table_event() */

/*===========================================================================
FUNCTION wms_bc_mm_make_srv_ids_event()                            (INTERNAL)

DESCRIPTION
  Use the stored information to make a srv_ids event.

DEPENDENCIES
  None

RETURNS
  TRUE:  Sucessful
  FALSE: Failed

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_bc_mm_make_srv_ids_event(
  wms_prov_app_info_s_type       *appInfo_ptr,
  wms_message_mode_e_type         message_mode,
  wms_bc_mm_service_ids_s_type   *output
)
{
  uint32                    i           = 0;
  uint32                    j           = 0;
  boolean                   st          = TRUE;

  if ( (NULL == appInfo_ptr) || (NULL == output) )
  {
    MSG_ERROR_2( "Null pointer appInfo_ptr=0x%x, output=0x%x",
                 appInfo_ptr,
                 output );
    return FALSE;
  }

  /* Allocate enough memory for the output */
  output->size = (uint16)wms_bc_mm_get_active_table_size( appInfo_ptr, message_mode );
  output->entries = wms_mem_malloc( output->size * sizeof(wms_bc_mm_srv_id_s_type) );

  if (NULL == output->entries)
  {
    if (output->size > 0)
    {
      st = FALSE;
    }
  }
  else
  {
    if( message_mode == WMS_MESSAGE_MODE_GW )
    {
#ifdef FEATURE_GWSMS_BROADCAST
      for( i = 0, j = 0; i < appInfo_ptr->bc.gw.gw_cb_table_size; i++ )
      {
        if (TRUE == appInfo_ptr->bc.gw.gw_cb_table[i].active_service)
        {
          output->entries[j].gw_cb_range.from  = appInfo_ptr->bc.gw.gw_cb_table[i].service_from;
          output->entries[j].gw_cb_range.to    = appInfo_ptr->bc.gw.gw_cb_table[i].service_to;
          output->entries[j].selected          = appInfo_ptr->bc.gw.gw_cb_table[i].selected;
          j++;
        }
      }
      output->size = (uint16)j;
#endif /* FEATURE_GWSMS_BROADCAST */
    }
    else
    {
#ifdef FEATURE_CDSMS_BROADCAST
      for( i=0, j=0; i < appInfo_ptr->bc.cd.bc_table_size; i++ )
      {
        if (TRUE == appInfo_ptr->bc.cd.bc_table[i].active_service)
        {
          output->entries[j].bc_srv_id.service  =
                            (wms_service_e_type) appInfo_ptr->bc.cd.bc_table[i].service;
          output->entries[j].bc_srv_id.language =
                            (wms_language_e_type) appInfo_ptr->bc.cd.bc_table[i].language;
          output->entries[j].selected = appInfo_ptr->bc.cd.bc_table[i].selected;
          j++;
        }
      }
      output->size = (uint16)j;
#endif /* FEATURE_CDSMS_BROADCAST */
    }
  }
  return st;
} /* wms_bc_mm_make_srv_ids_event() */

#ifdef FEATURE_GWSMS_BROADCAST
/*===========================================================================
FUNCTION wms_bc_make_gw_cb_srv_info_event()                        (INTERNAL)

DESCRIPTION
  Build BC MM Service Info event information.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_bc_make_gw_cb_srv_info_event(
  nv_sms_gw_cb_service_table_type *input,
  wms_bc_mm_service_info_s_type   *output
)
{
  if( input==NULL || output == NULL )
  {
    MSG_ERROR_2( "Null pointer input=0x%x, output=0x%x",
                 input,
                 output );
    return;
  }

  output->srv_id.gw_cb_range.from = input->service_from;
  output->srv_id.gw_cb_range.to   = input->service_to;
  output->selected                = input->selected;
  output->priority                = WMS_PRIORITY_NORMAL;
  (void)memscpy( output->label,
                 WMS_BC_MM_SRV_LABEL_SIZE,
                 (char *) input->label,
                 WMS_BC_MM_SRV_LABEL_SIZE );
  output->label_encoding          = (wms_user_data_encoding_e_type) input->label_encoding;
  output->alert                   = (wms_bc_alert_e_type) input->bc_alert;
  output->max_messages            = input->max_messages;
} /* wms_bc_make_gw_cb_srv_info_event */

/*===========================================================================
FUNCTION wms_bc_gw_cb_enable_check()                               (INTERNAL)

DESCRIPTION
  Check whether we need to enable or disable BC SMS

DEPENDENCIES
  None

RETURNS
  boolean: TRUE - enable; FALSE - disable

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_bc_gw_cb_enable_check(
  wms_bc_config_e_type cb_config,
  wms_bc_pref_e_type   cb_pref,
  uint16               num_selected
)
{
  boolean              result = FALSE;

  MSG_HIGH_1("GW CB CONF=%d", cb_config);
  MSG_HIGH_1("GW CB PREF=%d", cb_pref);
  MSG_HIGH_1("GW CB num_sel=%d", num_selected);

  if ((WMS_BC_CONFIG_DISALLOW != cb_config) &&
      (WMS_BC_PREF_DEACTIVATE != cb_pref) &&
      (0 != num_selected))
  {
    result = TRUE;
  }

  return result;
} /* wms_bc_gw_cb_enable_check() */

/*===========================================================================
FUNCTION wms_cb_search_gw_cb_table_entry_in_range()                (INTERNAL)

DESCRIPTION
  Search the GW Broadcast Table for entry overlapping the ID

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_cb_search_gw_cb_table_entry_in_range(
  wms_prov_app_info_s_type         *appInfo_ptr,
  wms_gw_cb_srv_range_s_type       *range_ptr,
  uint32                           *index,
  nv_sms_gw_cb_service_table_type **service_ptr
)
{
  int32                     i;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return FALSE;
  }

  if ( range_ptr == NULL ||index == NULL || service_ptr == NULL )
  {
    MSG_ERROR_3( "Null Pointer  range_ptr=0x%x, index=0x%x, service_ptr=0x%x",
                 range_ptr,
                 index,
                 service_ptr );
    return FALSE;
  }

  if (0 == appInfo_ptr->bc.gw.gw_cb_table_size)
  {
    return FALSE;
  }

  /* search the table for a entry that has range overlaps the ID. The loop starts
     with the last entry so that we can find the first available free entry
     in the table.
  */
  for( i = appInfo_ptr->bc.gw.gw_cb_table_size - 1; i >= 0 ; i-- )
  {
    if ((TRUE            == appInfo_ptr->bc.gw.gw_cb_table[i].active_service) &&
        (range_ptr->from <  appInfo_ptr->bc.gw.gw_cb_table[i].service_from) &&
        (range_ptr->to   >  appInfo_ptr->bc.gw.gw_cb_table[i].service_to))
    {
      *index = (uint32)i;
      *service_ptr = &appInfo_ptr->bc.gw.gw_cb_table[i];
      return TRUE;
    }
  }

  return FALSE;
} /* wms_cb_search_gw_cb_table_entry_in_range() */

/*===========================================================================
FUNCTION wms_bc_search_gw_cb_table_entry()                         (INTERNAL)

DESCRIPTION
  Search the GW Broadcast Table for a particular Service Entry.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_bc_search_gw_cb_table_entry(
  wms_prov_app_info_s_type   *appInfo_ptr,
  wms_gw_cb_srv_range_s_type *range_ptr,      /* IN */
  boolean                    *found_free,       /* OUT */
  boolean                    *found_dup,        /* OUT */
  uint32                     *index,            /* OUT */
  nv_sms_gw_cb_service_table_type **service_ptr /* OUT */
)
{
  int32                     i;
  int32                     free_entry_count = 0;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  if (NULL != found_free)
  {
    *found_free = FALSE;
  }

  if (NULL != found_dup)
  {
    *found_dup = FALSE;
  }

  if( (NULL == range_ptr) || (NULL == found_free) ||
      (NULL == found_dup) || (NULL == index) ||
      (NULL == service_ptr) )
  {
    MSG_ERROR_0("NULL Pointer");
    return;
  }

  if (0 == appInfo_ptr->bc.gw.gw_cb_table_size)
  {
    return;
  }

  /* search the table for a duplicate entry or a free entry. The loop starts
     with the last entry so that we can find the first available free entry
     in the table.
  */
  for( i = appInfo_ptr->bc.gw.gw_cb_table_size - 1; i >= 0 ; i-- )
  {
    if ((TRUE            == appInfo_ptr->bc.gw.gw_cb_table[i].active_service) &&
        (range_ptr->from == appInfo_ptr->bc.gw.gw_cb_table[i].service_from) &&
        (range_ptr->to   == appInfo_ptr->bc.gw.gw_cb_table[i].service_to))
    {
      *found_dup = TRUE;
      *index = (uint32)i;
      *service_ptr = &appInfo_ptr->bc.gw.gw_cb_table[i];
      break;
    }
    else if (FALSE == appInfo_ptr->bc.gw.gw_cb_table[i].active_service)
    {
      *found_free = TRUE;
      *index = (uint32)i;
      *service_ptr = &appInfo_ptr->bc.gw.gw_cb_table[i];
      free_entry_count++;
    }
  }
  MSG_HIGH_2( "BC: found free entry %d out of %d ",free_entry_count,
                                               appInfo_ptr->bc.gw.gw_cb_table_size );

  return;
} /* wms_bc_search_gw_cb_table_entry() */

/*===========================================================================
FUNCTION wms_bc_gw_cb_enable_to_nas()                              (INTERNAL)

DESCRIPTION
  Send the GW CB Enable to the NAS Layer to Enable Cell Broadcast

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_bc_gw_cb_enable_to_nas(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  wms_evt_cb_sms_search_req_s_type  evt;
  wms_cfg_s_type                   *lcfg_s_ptr = wms_cfg_s_ptr();

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  MSG_HIGH_2( "Sending to NAS: GW CB enable=%d, as_id=%d",
              appInfo_ptr->bc.gw.gw_cb_enable,
              appInfo_ptr->asid );

  if (FALSE == appInfo_ptr->bc.gw.gw_cb_enable)
  {
    /* Disable */
    if (WMS_CMD_ERR_NONE != wms_cm_gw_cb_disable( appInfo_ptr->asid )) 
    {
      MSG_ERROR_0("wms_cm_gw_cb_disable() failed");
      return;
    }
    evt.num_selected = 0;
  }
  else if (TRUE == lcfg_s_ptr->lpm)
  {
    MSG_ERROR_0( "Do not send search list to NAS/ LTE RRC while in LPM" );
    return;
  }
  else
  {
    boolean reading_advised = FALSE;
    boolean reading_optional = FALSE;
    uint32 num_selected = 0;

#ifdef FEATURE_UMTS_BMC
    reading_advised = appInfo_ptr->bc.gw.reading_advised;
    reading_optional = appInfo_ptr->bc.gw.reading_optional;
#endif /* FEATURE_UMTS_BMC */

    if (WMS_CMD_ERR_NONE != wms_cm_gw_cb_search_list( appInfo_ptr->asid,
                                                      appInfo_ptr->bc.gw.sim_cbmid_list,
                                                      appInfo_ptr->bc.gw.sim_cbmid_list_size,
                                                      appInfo_ptr->bc.gw.gw_cb_table,
                                                      appInfo_ptr->bc.gw.gw_cb_table_size,
                                                      reading_advised,
                                                      reading_optional,
                                                      &num_selected )) 
    {
      MSG_ERROR_0( "wms_cm_gw_cb_search_list() failed" );
      return;
    }
    else
    {
      evt.num_selected = (uint8)num_selected;
    }
  }
  event_report_payload (EVENT_WMS_SEARCH_REQUEST, sizeof(evt), (void *) &evt);

  return;
} /* wms_bc_gw_cb_enable_to_nas() */

/*===========================================================================
FUNCTION wms_bc_build_sim_cbmi_data()                              (INTERNAL)

DESCRIPTION
  Build the CBMI Data to be written to the SIM Card.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_bc_build_sim_cbmi_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  uint8                    *sim_cbmi_data
)
{
  uint32           i;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  memset( sim_cbmi_data, 0xFF, appInfo_ptr->bc.gw.sim_cbmi_rec_len );

  for( i = 0 ; i < appInfo_ptr->bc.gw.sim_cbmi_list_size; i ++ )
  {
    sim_cbmi_data[i*2]   = appInfo_ptr->bc.gw.sim_cbmi_list[i] >> 8;
    sim_cbmi_data[i*2+1] = appInfo_ptr->bc.gw.sim_cbmi_list[i] & 0xFF;
  }
} /* wms_bc_build_sim_cbmi_data */

/*===========================================================================
FUNCTION wms_bc_build_sim_cbmir_data()                             (INTERNAL)

DESCRIPTION
  Build the CBMI Data to be written to the SIM Card.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_bc_build_sim_cbmir_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  uint8                    *sim_cbmir_data
)
{
  uint32           i;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  memset( sim_cbmir_data, 0xFF, appInfo_ptr->bc.gw.sim_cbmir_rec_len );

  for( i = 0 ; i < appInfo_ptr->bc.gw.sim_cbmir_list_size; i ++ )
  {
    sim_cbmir_data[i*4]   = appInfo_ptr->bc.gw.sim_cbmir_list[i].from >> 8;
    sim_cbmir_data[i*4+1] = appInfo_ptr->bc.gw.sim_cbmir_list[i].from & 0xFF;
    sim_cbmir_data[i*4+2] = appInfo_ptr->bc.gw.sim_cbmir_list[i].to >> 8;
    sim_cbmir_data[i*4+3] = appInfo_ptr->bc.gw.sim_cbmir_list[i].to & 0xFF;
  }

  return;
} /* wms_bc_build_sim_cbmir_data */

/*===========================================================================
FUNCTION wms_bc_search_gw_cb_sim_list()                            (INTERNAL)

DESCRIPTION
  Search the Service ID in the SIM CBMI and CBMIR Entries.

DEPENDENCIES
  None

RETURNS
  True / False

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_bc_search_gw_cb_sim_list(
  wms_prov_app_info_s_type *appInfo_ptr,
  wms_gw_cb_srv_id_type     from,
  wms_gw_cb_srv_id_type     to
)
{
  uint32           i;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return FALSE;
  }

  for( i = 0 ; i < appInfo_ptr->bc.gw.sim_cbmi_list_size; i ++ )
  {
    if( appInfo_ptr->bc.gw.sim_cbmi_list[i] == from &&
        appInfo_ptr->bc.gw.sim_cbmi_list[i] == to )
    {
      return TRUE;
    }
  }

  for( i = 0 ; i < appInfo_ptr->bc.gw.sim_cbmir_list_size; i ++ )
  {
    if( appInfo_ptr->bc.gw.sim_cbmir_list[i].from == from &&
        appInfo_ptr->bc.gw.sim_cbmir_list[i].to   == to )
    {
      return TRUE;
    }
  }

  return FALSE;
} /* wms_bc_search_gw_cb_sim_list() */

/*===========================================================================
FUNCTION wms_bc_update_gw_cb_table_to_sim()                        (INTERNAL)

DESCRIPTION
  Update the GW CB Table to the SIM Card.

DEPENDENCIES
  None

RETURNS
  True / False

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_bc_update_gw_cb_table_to_sim(
  sys_modem_as_id_e_type  as_id
)
{
  uint32                      i;
  uint32                      cbmi_max;
  uint32                      cbmir_max;
  boolean                     st          = TRUE;
  wms_prov_app_info_s_type   *appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);
  wms_gw_bc_info_s_type      *bc_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return FALSE;
  }

  bc_ptr = &appInfo_ptr->bc.gw;

  cbmi_max  = MIN(bc_ptr->sim_cbmi_rec_len / 2, WMS_SIM_CBMI_LIST_MAX);
  /* Each Record occupies 2 bytes for Message ID */

  cbmir_max = MIN(bc_ptr->sim_cbmir_rec_len / 4, WMS_SIM_CBMIR_LIST_MAX);
  /* Each Record occupies 2 bytes for Message ID To
     and 2 bytes for Message ID from */

  bc_ptr->sim_cbmi_list_size  = 0;
  bc_ptr->sim_cbmir_list_size = 0;


  MSG_HIGH_1("Maximum Number of CBMI Records = %d", cbmi_max);
  MSG_HIGH_1("Maximum Number of CBMIR Records = %d", cbmir_max);

  /* Traverse the whole service table */
  for (i = 0; i < bc_ptr->gw_cb_table_size && i < WMS_BC_MM_TABLE_MAX; i++ )
  {
    if ((TRUE == bc_ptr->gw_cb_table[i].active_service) &&
        (TRUE == bc_ptr->gw_cb_table[i].selected) &&
        (TRUE == bc_ptr->in_sim[i]))
    {
      /* Add this entry to CBMI only if from==to and CBMI is not full */
      if( ( bc_ptr->gw_cb_table[i].service_from == bc_ptr->gw_cb_table[i].service_to ) &&
          ( bc_ptr->sim_cbmi_list_size < cbmi_max ) )
      {
        bc_ptr->sim_cbmi_list[bc_ptr->sim_cbmi_list_size] = bc_ptr->gw_cb_table[i].service_from;
        bc_ptr->sim_cbmi_list_size++;
      }
      else if( bc_ptr->sim_cbmir_list_size < cbmir_max )
      {
        bc_ptr->sim_cbmir_list[bc_ptr->sim_cbmir_list_size].from = bc_ptr->gw_cb_table[i].service_from;
        bc_ptr->sim_cbmir_list[bc_ptr->sim_cbmir_list_size].to   = bc_ptr->gw_cb_table[i].service_to;
        bc_ptr->sim_cbmir_list_size++;
      }
      else
      {
        MSG_HIGH_2( "Cannot Add Service from %d to %d to SIM",
                    bc_ptr->gw_cb_table[i].service_from,
                    bc_ptr->gw_cb_table[i].service_to );
        st = FALSE;
      }
    }
  } /* for loop */

  if (st == FALSE)
  {
    MSG_HIGH_0("Number of Selected Entries exceeds CBMI/CBMIR records");
    st = TRUE; /* Resetting Status to TRUE */
  }

  {
    uint8                        sim_data[255] = {0};

    /* Update data to the SIM - CBMI */
    if (bc_ptr->sim_cbmi_rec_len != 0)
    {
      wms_bc_build_sim_cbmi_data( appInfo_ptr, sim_data );
      if (wms_sim_write_cbmi( appInfo_ptr, sim_data ) == FALSE)
      {
        MSG_ERROR_0("CBMI Write Failed");
        st = FALSE;
      }
    }

    /* Update data to the SIM - CBMIR */
    if (bc_ptr->sim_cbmir_rec_len != 0)
    {
      wms_bc_build_sim_cbmir_data( appInfo_ptr, sim_data );
      if (wms_sim_write_cbmir( appInfo_ptr, sim_data ) == FALSE)
      {
        MSG_ERROR_0("CBMIR Write Failed");
        st = FALSE;
      }
    }
  }

  return st;
} /* wms_bc_update_gw_cb_table_to_sim() */

#endif /* FEATURE_GWSMS_BROADCAST */

/* ================================================================= */
/* ================= BC MM Command Processing ====================== */
/* ================================================================= */

/*===========================================================================
FUNCTION wms_bc_mm_get_config_proc()                               (INTERNAL)

DESCRIPTION
  This function is used to retrieve the configuration for
  Multimode Broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_get_config_proc(
  wms_cmd_type  *cmd_ptr
)
{
  sys_modem_as_id_e_type       as_id           = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type    *appInfo_ptr     = cmd_ptr->hdr.appInfo_ptr;
  wms_bc_mm_event_info_s_type *bc_mm_event_ptr = NULL;
  wms_cmd_err_e_type           cmd_err         = WMS_CMD_ERR_NONE;

  MSG_HIGH_2( "WMS_CMD_BC_MM_GET_CONFIG: asid %d, message_mode %d",
              as_id,
              cmd_ptr->cmd.bc_mm_get_config.message_mode );

  do
  {
    if (!wms_check_appinfo_is_valid(appInfo_ptr))
    {
      cmd_err = WMS_CMD_ERR_NULL_PTR;
      break;
    }

    bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

    if (NULL == bc_mm_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
      break;
    }
  } while(0);

  // Indicate the command status to the client
  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    bc_mm_event_ptr->as_id        = as_id;
    bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_get_config.message_mode;

    if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_get_config.message_mode)
    {
#ifdef FEATURE_GWSMS_BROADCAST
      // GW: copy config data
      bc_mm_event_ptr->u.bc_config = appInfo_ptr->bc.gw.gw_cb_config;
#endif /* FEATURE_GWSMS_BROADCAST */
    }
    else
    {
#ifdef FEATURE_CDSMS_BROADCAST
      // CDMA: copy config data
      bc_mm_event_ptr->u.bc_config = appInfo_ptr->bc.cd.bc_config;
#endif /* FEATURE_CDSMS_BROADCAST */
    }

    // Send event to the client
    wms_bc_mm_event_notify( WMS_BC_MM_EVENT_CONFIG, bc_mm_event_ptr );
  }
  else
  {
    MSG_ERROR_1( "WMS_BC_MM_EVENT_CONFIG event not sent: error %d", cmd_err);
  }

  WMS_MEM_FREEIF(bc_mm_event_ptr);

  return;
} /* wms_bc_mm_get_config_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_get_pref_proc()                                 (INTERNAL)

DESCRIPTION
  This function is used to retrieve the user preference for
  Multimode Broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_get_pref_proc(
  wms_cmd_type  *cmd_ptr
)
{
  sys_modem_as_id_e_type       as_id           = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type    *appInfo_ptr     = cmd_ptr->hdr.appInfo_ptr;
  wms_bc_mm_event_info_s_type *bc_mm_event_ptr = NULL;
  wms_cmd_err_e_type           cmd_err         = WMS_CMD_ERR_NONE;

  MSG_HIGH_0("WMS_CMD_BC_MM_GET_PREF");

  do
  {
    if (NULL == appInfo_ptr)
    {
      cmd_err = WMS_CMD_ERR_NULL_PTR;
      break;
    }

    bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

    if (NULL == bc_mm_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
      break;
    }
  } while(0);

  // Indicate the command status to the client
  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    bc_mm_event_ptr->as_id        = as_id;
    bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_get_pref.message_mode;
    bc_mm_event_ptr->user_data    = cmd_ptr->hdr.user_data;

    if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_get_pref.message_mode)
    {
#ifdef FEATURE_GWSMS_BROADCAST
      bc_mm_event_ptr->u.bc_pref = appInfo_ptr->bc.gw.gw_cb_pref;
#endif /* FEATURE_GWSMS_BROADCAST */
    }
    else
    {
#ifdef FEATURE_CDSMS_BROADCAST
      bc_mm_event_ptr->u.bc_pref = appInfo_ptr->bc.cd.bc_pref;
#endif /* FEATURE_CDSMS_BROADCAST */
    }

    wms_bc_mm_event_notify( WMS_BC_MM_EVENT_PREF, bc_mm_event_ptr );
  }
  else
  {
    MSG_ERROR_1( "WMS_BC_MM_EVENT_PREF event not sent: error %d", cmd_err);
  }

  WMS_MEM_FREEIF(bc_mm_event_ptr);

  return;
} /* wms_bc_mm_get_pref_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_get_presidential_alert_entry_count_in_client_req()                            (INTERNAL)

DESCRIPTION
  This function is used to determine how many presidential entries are present in
  the srv_id_arr of the client request.

DEPENDENCIES
  None

RETURN VALUE
 uint8

SIDE EFFECTS
  None

=========================================================================*/
uint8 wms_bc_mm_get_presidential_alert_entry_count_in_client_req(
  wms_message_mode_e_type         message_mode,
  wms_bc_mm_srv_id_type*          srv_id_arr
)
{
  wms_cfg_s_type  *cfg_s_ptr = wms_cfg_s_ptr();
  uint8            pres_entry_count = 0; 

  if (NULL == srv_id_arr)
  {
    MSG_ERROR_0("NULL srv_id_arr");     
  }
  else if ( !cfg_s_ptr->presidential_bc_alert_is_disabled )
  {
    if (WMS_MESSAGE_MODE_CDMA == message_mode)
    {
      if (WMS_SRV_CMAS_PRESIDENTIAL_ALERT == srv_id_arr[0].bc_srv_id.service)
      {
        pres_entry_count = 1;
      }
    }
    else if (WMS_MESSAGE_MODE_GW == message_mode)
    {
      if ((srv_id_arr[0].gw_cb_range.from <= WMS_GW_CB_SRV_ID_CMAS_MSGID1) &&
          (srv_id_arr[0].gw_cb_range.to >= WMS_GW_CB_SRV_ID_CMAS_MSGID14))
      {
        MSG_HIGH_0("Two Presidential Alert entries present in the service id range");
      
        // Maintain the two presidential entries in the srv_id_arr.
        // These will be later on processed.
        srv_id_arr[0].gw_cb_range.from = WMS_GW_CB_SRV_ID_CMAS_MSGID1;
        srv_id_arr[0].gw_cb_range.to   = WMS_GW_CB_SRV_ID_CMAS_MSGID1;

        srv_id_arr[1].gw_cb_range.from   = WMS_GW_CB_SRV_ID_CMAS_MSGID14;
        srv_id_arr[1].gw_cb_range.to	 = WMS_GW_CB_SRV_ID_CMAS_MSGID14;
        pres_entry_count = 2;

      }
      else if ((srv_id_arr[0].gw_cb_range.from <= WMS_GW_CB_SRV_ID_CMAS_MSGID1) &&
               (srv_id_arr[0].gw_cb_range.to >= WMS_GW_CB_SRV_ID_CMAS_MSGID1))
      {
        // Maintain the only presidential entry present in srv_id_arr.
        srv_id_arr[0].gw_cb_range.from = WMS_GW_CB_SRV_ID_CMAS_MSGID1;
        srv_id_arr[0].gw_cb_range.to   = WMS_GW_CB_SRV_ID_CMAS_MSGID1;
        pres_entry_count = 1;

      }
      else if ((srv_id_arr[0].gw_cb_range.from <= WMS_GW_CB_SRV_ID_CMAS_MSGID14) &&
               (srv_id_arr[0].gw_cb_range.to >= WMS_GW_CB_SRV_ID_CMAS_MSGID14))
      {
        // Maintain the only presidential entry present in srv_id_arr.
        srv_id_arr[0].gw_cb_range.from = WMS_GW_CB_SRV_ID_CMAS_MSGID14;
        srv_id_arr[0].gw_cb_range.to   = WMS_GW_CB_SRV_ID_CMAS_MSGID14;
        pres_entry_count = 1;
      } 
    }
  }

  return pres_entry_count;
} /* wms_bc_mm_get_presidential_alert_entry_count_in_client_req() */

/*===========================================================================
FUNCTION wms_bc_mm_table_deactivate_check()                        (INTERNAL)

DESCRIPTION
  This function is used to determine if the broadcast SMS service table can
  be deactivated for the given mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - ok to deactivate
  FALSE - do not deactivate

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_bc_mm_table_deactivate_check(
  sys_modem_as_id_e_type          as_id,
  wms_message_mode_e_type         message_mode
)
{
   uint16                    i;
   wms_bc_mm_srv_id_type     srv_id[2];
   boolean                   ok_to_deact = TRUE;
   wms_prov_app_info_s_type *appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);

   if (!wms_check_appinfo_is_valid(appInfo_ptr))
   {
     return FALSE;
   }

   if (WMS_MESSAGE_MODE_GW == message_mode)
   {
#ifdef FEATURE_GWSMS_BROADCAST

     for(i = 0; i < appInfo_ptr->bc.gw.gw_cb_table_size && i < WMS_BC_MM_TABLE_MAX; i++)
     {
       srv_id[0].gw_cb_range.from = appInfo_ptr->bc.gw.gw_cb_table[i].service_from;
       srv_id[0].gw_cb_range.to   = appInfo_ptr->bc.gw.gw_cb_table[i].service_to;
       if (0 < wms_bc_mm_get_presidential_alert_entry_count_in_client_req(WMS_MESSAGE_MODE_GW, srv_id))
       {
          ok_to_deact = FALSE;
          break;
       }
     }
#endif
   }
   else if (WMS_MESSAGE_MODE_CDMA == message_mode)
   {
#ifdef FEATURE_CDSMS_BROADCAST
     (void)as_id;  // not used

      for(i = 0; i < appInfo_ptr->bc.cd.bc_table_size && i < WMS_BC_TABLE_MAX; i++)
      {
        srv_id[0].bc_srv_id.service = (wms_service_e_type)appInfo_ptr->bc.cd.bc_table[i].service;
        if (0 < wms_bc_mm_get_presidential_alert_entry_count_in_client_req(WMS_MESSAGE_MODE_CDMA, srv_id))
        {
           ok_to_deact = FALSE;
           break;
        }
      }
#endif
   }

   return ok_to_deact;
} /* wms_bc_mm_table_deactivate_check() */

/*===========================================================================
FUNCTION wms_bc_mm_set_pref_proc()                                 (INTERNAL)

DESCRIPTION
  This function is used to set the user preference for
  Multimode Broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_set_pref_proc(
  wms_cmd_type  *cmd_ptr
)
{
  nv_item_type                 nvi;
  sys_modem_as_id_e_type       as_id           = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type    *appInfo_ptr     = cmd_ptr->hdr.appInfo_ptr;
  wms_bc_mm_event_info_s_type *bc_mm_event_ptr = NULL;
  wms_cmd_err_e_type           cmd_err         = WMS_CMD_ERR_NONE;

  MSG_HIGH_0("WMS_CMD_BC_MM_SET_PREF");

  do
  {
    if (NULL == appInfo_ptr)
    {
      cmd_err = WMS_CMD_ERR_NULL_PTR;
      break;
    }

    if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_set_pref.message_mode)
    {
#ifdef FEATURE_GWSMS_BROADCAST
      // If the broadcast config is set to DISALLOW, then the broadcast
      // user pref cannot be set to anything other than DEACTIVATE
      if ( (WMS_BC_CONFIG_DISALLOW == appInfo_ptr->bc.gw.gw_cb_config) &&
           (WMS_BC_PREF_DEACTIVATE != cmd_ptr->cmd.bc_mm_set_pref.bc_pref) )
      {
        MSG_HIGH_0("BC Pref: CANNOT_ACTIVATE");
        cmd_err = WMS_CMD_ERR_BC_CANNOT_ACTIVATE;
        break;
      }

      // Check for broadcast user pref out of bounds
      if (cmd_ptr->cmd.bc_mm_set_pref.bc_pref >= WMS_BC_PREF_MAX )
      {
        MSG_HIGH_0("BC Pref: bad pref");
        cmd_err = WMS_CMD_ERR_BC_BAD_PREF;
        break;
      }

      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }

      // Check for no change
      if (cmd_ptr->cmd.bc_mm_set_pref.bc_pref == appInfo_ptr->bc.gw.gw_cb_pref )
      {
        // No preference change
        cmd_err = WMS_CMD_ERR_NONE;
        break;
      }

      // If deactivating, make sure the table can be deactivated.
      // Some entries like the Presidential Warning System (PWS) can't be
      // disabled by the user.
      if (WMS_BC_PREF_DEACTIVATE == cmd_ptr->cmd.bc_mm_set_pref.bc_pref)
      {
        if ( !wms_bc_mm_table_deactivate_check(as_id, WMS_MESSAGE_MODE_GW) )
        {
          MSG_ERROR_0("BC Pref: Cannot deactivate");
          cmd_err = WMS_CMD_ERR_BC_DEACTIVATE_NOT_ALLOWED;
          break;
        }
      }

      // The command parameters are OK, now update NV
      {
        nv_stat_enum_type write_status;

        nvi.sms_gw_cb_user_pref = (byte)cmd_ptr->cmd.bc_mm_set_pref.bc_pref;

        write_status = wms_nv_write_wait_per_subs(as_id, NV_SMS_GW_CB_USER_PREF_I, &nvi);

        if (NV_DONE_S != write_status)
        {
          MSG_ERROR_0("BC PREF NV write failed");
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
          break;
        }
      }

#endif /* FEATURE_GWSMS_BROADCAST */
    }
    else // message mode CDMA
    {
#ifdef FEATURE_CDSMS_BROADCAST
      // If the broadcast config is set to DISALLOW, then the broadcast
      // user pref cannot be set to anything other than DEACTIVATE
      if ( (WMS_BC_CONFIG_DISALLOW == appInfo_ptr->bc.cd.bc_config) &&
           (WMS_BC_PREF_DEACTIVATE != cmd_ptr->cmd.bc_mm_set_pref.bc_pref) )
      {
        MSG_HIGH_0("BC Pref: CANNOT_ACTIVATE");
        cmd_err = WMS_CMD_ERR_BC_CANNOT_ACTIVATE;
        break;
      }

      // If the broadcast config is set to ALLOW_TABLE, then the broadcast
      // user pref cannot be set to anything higher than ALLOW_TABLE
      if ( (WMS_BC_CONFIG_ALLOW_TABLE == appInfo_ptr->bc.cd.bc_config) &&
           (WMS_BC_PREF_ACTIVATE_ALL == cmd_ptr->cmd.bc_mm_set_pref.bc_pref) )
      {
        MSG_HIGH_0("BC Pref: CANNOT_ACTIVATE_ALL");
        cmd_err = WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL;
        break;
      }

      // Check for broadcast user pref out of bounds
      if (cmd_ptr->cmd.bc_mm_set_pref.bc_pref >= WMS_BC_PREF_MAX)
      {
        MSG_HIGH_0("BC Pref: bad pref");
        cmd_err = WMS_CMD_ERR_BC_BAD_PREF;
        break;
      }

      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }

      // Check for no change
      if (cmd_ptr->cmd.bc_mm_set_pref.bc_pref == appInfo_ptr->bc.cd.bc_pref)
      {
        // No preference change
        cmd_err = WMS_CMD_ERR_NONE;
        break;
      }

      // If deactivating, make sure the table can be deactivated.
      // Some entries like the Presidential Warning System (PWS) can't be
      // disabled by the user.
      if (WMS_BC_PREF_DEACTIVATE == cmd_ptr->cmd.bc_mm_set_pref.bc_pref)
      {
        if (!wms_bc_mm_table_deactivate_check( as_id, WMS_MESSAGE_MODE_CDMA))
        {
          MSG_ERROR_0("BC Pref: Cannot deactivate");
          cmd_err = WMS_CMD_ERR_BC_DEACTIVATE_NOT_ALLOWED;
          break;
        }
      }

      // The command parameters are OK, now update NV
      {
        nv_stat_enum_type write_status;

        nvi.sms_bc_user_pref = (byte)cmd_ptr->cmd.bc_mm_set_pref.bc_pref;

        write_status = wms_nv_write_wait( NV_SMS_BC_USER_PREF_I, &nvi );

        if (NV_DONE_S != write_status)
        {
          MSG_ERROR_0("BC PREF NV write failed");
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
          break;
        }
      }
#endif /* FEATURE_CDSMS_BROADCAST */
    }

  } while(0);

  // Indicate the command status to the client
  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    // Prep the event
    bc_mm_event_ptr->as_id        = as_id;
    bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_set_pref.message_mode;
    bc_mm_event_ptr->u.bc_pref    = cmd_ptr->cmd.bc_mm_set_pref.bc_pref;
    bc_mm_event_ptr->user_data    = cmd_ptr->hdr.user_data;

    // Send event to client
    wms_bc_mm_event_notify( WMS_BC_MM_EVENT_PREF, bc_mm_event_ptr );

    if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_set_pref.message_mode)
    {
#ifdef FEATURE_GWSMS_BROADCAST
      // GW: update local state
      boolean gw_cb_enable_cur;
      boolean gw_cb_enable_new;

      // Update local data
      gw_cb_enable_cur = appInfo_ptr->bc.gw.gw_cb_enable;      // Save current state of enable

      gw_cb_enable_new = wms_bc_gw_cb_enable_check( appInfo_ptr->bc.gw.gw_cb_config,
                                                    cmd_ptr->cmd.bc_mm_set_pref.bc_pref,
                                                    appInfo_ptr->bc.gw.gw_cb_num_selected );

      rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
      appInfo_ptr->bc.gw.gw_cb_pref = cmd_ptr->cmd.bc_mm_set_pref.bc_pref; // Used by modem api
      appInfo_ptr->bc.gw.gw_cb_enable = gw_cb_enable_new;                  // Used by modem api
      rex_leave_crit_sect(&wms_bc_crit_sect);

      // Notify NAS only if gw_cb_enable flag has changed
      if (gw_cb_enable_cur != gw_cb_enable_new )
      {
        MSG_HIGH_1("GW CB enable=%d", gw_cb_enable_new);
        wms_bc_gw_cb_enable_to_nas(appInfo_ptr);
      }
#endif /* FEATURE_GWSMS_BROADCAST */
    }
    else
    {
#ifdef FEATURE_CDSMS_BROADCAST
      // CDMA: update local state
      boolean bc_enable_cur;
      boolean bc_enable_new;

      // Update local data
      bc_enable_cur = appInfo_ptr->bc.cd.bc_enable;   // Save current state of enable
      bc_enable_new = wms_bc_enable_check(appInfo_ptr);    // Get new enable state (outside of crit_sect!)

      rex_enter_crit_sect(&wms_bc_crit_sect);   // Exclusive write
      appInfo_ptr->bc.cd.bc_pref = cmd_ptr->cmd.bc_mm_set_pref.bc_pref;  // Used by modem api
      appInfo_ptr->bc.cd.bc_enable = bc_enable_new;                      // Used by modem api
      rex_leave_crit_sect(&wms_bc_crit_sect);

      // Notify CM only if bc_enable flag has changed
      if( bc_enable_cur != bc_enable_new )
      {
        MSG_HIGH_1("BCSMS enable=%d", appInfo_ptr->bc.cd.bc_enable);
        wms_cm_bc_enable( appInfo_ptr->bc.cd.bc_enable );
      }
#endif /* FEATURE_CDSMS_BROADCAST */
    }
  }
  else
  {
    MSG_ERROR_1( "WMS_BC_MM_EVENT_PREF event not sent: error %d", cmd_err);
  }

  WMS_MEM_FREEIF(bc_mm_event_ptr);

  return;
} /* wms_bc_mm_set_pref_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_get_table_proc()                                (INTERNAL)

DESCRIPTION
  This function is used to retrieve the service table for broadcast SMS.

  NOTE: To prevent retrieving a large service table, the function
  wms_bc_mm_get_all_service_ids() can be called to retrieve all service
  IDs, where each ID is much smaller than a complete table entry, and then
  the function wms_bc_mm_get_service_info() can be called for each service ID
  in order to retrieve the table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_get_table_proc(
  wms_cmd_type  *cmd_ptr
)
{
  wms_bc_mm_event_info_s_type    *bc_mm_event_ptr = NULL;
  sys_modem_as_id_e_type          as_id           = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type       *appInfo_ptr     = cmd_ptr->hdr.appInfo_ptr;
  wms_cmd_err_e_type              cmd_err         = WMS_CMD_ERR_NONE;

  MSG_HIGH_2( "WMS_CMD_BC_MM_GET_TABLE: asid %d, message_mode %d",
              as_id,
              cmd_ptr->cmd.bc_mm_get_table.message_mode );

  do
  {
    boolean  table_ok;

    bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

    if (NULL == bc_mm_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
      break;
    }

    table_ok = wms_bc_mm_make_table_event( appInfo_ptr,
                                           cmd_ptr->cmd.bc_mm_get_table.message_mode,
                                           &bc_mm_event_ptr->u.bc_mm_table );
    if (!table_ok)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
      break;
    }

  } while(0);

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    bc_mm_event_ptr->as_id        = as_id;
    bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_get_table.message_mode;
    bc_mm_event_ptr->user_data    = cmd_ptr->hdr.user_data;

    wms_bc_mm_event_notify( WMS_BC_MM_EVENT_TABLE, bc_mm_event_ptr );
  }
  else
  {
    MSG_ERROR_1( "WMS_BC_MM_EVENT_TABLE event not sent: error %d", cmd_err);
  }

  if (NULL != bc_mm_event_ptr)
  {
    WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_table.entries);
    wms_mem_free(bc_mm_event_ptr);
  }

  return;
} /* wms_bc_mm_get_table_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_select_srv_proc()                               (INTERNAL)

DESCRIPTION
  This function is used to select a broadcast SMS service

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_select_srv_proc(
  wms_cmd_type  *cmd_ptr
)
{
  sys_modem_as_id_e_type    as_id       = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type *appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;
  nv_item_type              lnvi;

  MSG_HIGH_0("WMS_CMD_BC_SELECT_SRV");

  if (NULL == appInfo_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
    return;
  }

  memset( &lnvi, 0, sizeof(lnvi));

  if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_select_srv.message_mode)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    wms_bc_mm_event_info_s_type       *bc_mm_event_ptr   = NULL;
    boolean                            found_free        = FALSE;
    boolean                            found_dup         = FALSE;
    nv_sms_gw_cb_service_table_type   *gw_cb_service_ptr = NULL;
    wms_cmd_err_e_type                 cmd_err           = WMS_CMD_ERR_NONE;
    uint32                             service_idx;

    do
    {
      boolean         must_update_nv  = FALSE;

      /* find the entry matching service */
      wms_bc_search_gw_cb_table_entry( appInfo_ptr,
                                       &cmd_ptr->cmd.bc_mm_select_srv.srv_id.gw_cb_range,
                                       &found_free,
                                       &found_dup,
                                       &service_idx,
                                       &gw_cb_service_ptr );

      if (!found_dup)
      {
        MSG_HIGH_0("GW CB SEL: entry not found");
        cmd_err = WMS_CMD_ERR_BC_BAD_SRV_ID;
        break;
      }

      // Allocate the event buffer
      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }

      // If selection has changed, NV must be updated
      if (cmd_ptr->cmd.bc_mm_select_srv.selected != gw_cb_service_ptr->selected)
      {
        must_update_nv = TRUE;
      }

      // Copy the current service information to an NV item
      lnvi.sms_gw_cb_service_table = *gw_cb_service_ptr;

      // Change the NV item to reflect the select/deselect
      lnvi.sms_gw_cb_service_table.selected = cmd_ptr->cmd.bc_mm_select_srv.selected;

      // Update NV and SIM if selection has changed
      if (must_update_nv)
      {
        // Write to NV if needed */
        if (TRUE == appInfo_ptr->bc.gw.in_nv[service_idx])
        {
          nv_stat_enum_type    write_status;

          write_status = wms_nv_write_wait_per_subs( as_id, NV_SMS_GW_CB_SERVICE_TABLE_I, &lnvi );

          if (NV_DONE_S != write_status)
          {
            MSG_ERROR_1("GW CB Table NV write failed: %d", write_status);
            cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
            break;
          }
        }

        // Write to sim if needed
        if (TRUE == appInfo_ptr->bc.gw.in_sim[service_idx])
        {
          if (FALSE == wms_bc_update_gw_cb_table_to_sim(as_id))
          {
            MSG_ERROR_0("GW CB Table SIM write failed");
            cmd_err = WMS_CMD_ERR_BC_SIM_WRITE;
            break;
          }
        }
      }
    } while(0);

    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      /* update num_selected */
      if ( (TRUE  == cmd_ptr->cmd.bc_mm_select_srv.selected) &&
           (FALSE == gw_cb_service_ptr->selected) )
      {
        appInfo_ptr->bc.gw.gw_cb_num_selected++;  // Not used by modem api
      }
      else if ((FALSE == cmd_ptr->cmd.bc_mm_select_srv.selected) &&
               (TRUE == gw_cb_service_ptr->selected) )
      {
        appInfo_ptr->bc.gw.gw_cb_num_selected--;  // Not used by modem api
      }

      {
        boolean gw_cb_enable_new = wms_bc_gw_cb_enable_check( appInfo_ptr->bc.gw.gw_cb_config,
                                                              appInfo_ptr->bc.gw.gw_cb_pref,
                                                              appInfo_ptr->bc.gw.gw_cb_num_selected );

        rex_enter_crit_sect(&wms_bc_crit_sect);  // Exclusive write
        *gw_cb_service_ptr               = lnvi.sms_gw_cb_service_table; // Used by modem api
        appInfo_ptr->bc.gw.gw_cb_enable = gw_cb_enable_new;             // Used by modem api
        rex_leave_crit_sect(&wms_bc_crit_sect);
      }

      MSG_HIGH_1("GW CB enable=%d", appInfo_ptr->bc.gw.gw_cb_enable);
      wms_bc_gw_cb_enable_to_nas(appInfo_ptr);

      // Prep the event
      bc_mm_event_ptr->as_id        = as_id;
      bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_select_srv.message_mode;

      wms_bc_make_gw_cb_srv_info_event( gw_cb_service_ptr,
                                        &bc_mm_event_ptr->u.bc_mm_srv_info );

      // Send event to client
      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SRV_UPDATED, bc_mm_event_ptr);
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_SRV_UPDATED event not sent: error %d", cmd_err);
    }

    WMS_MEM_FREEIF(bc_mm_event_ptr);
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    wms_bc_mm_event_info_s_type       *bc_mm_event_ptr   = NULL;
    boolean                            found             = FALSE;
    nv_sms_bc_service_table_type      *service_ptr       = NULL;
    wms_cmd_err_e_type                 cmd_err           = WMS_CMD_ERR_NONE;
    uint16                             service_idx;

    do
    {
      boolean         must_update_nv  = FALSE;

      // Find the entry matching service and language */
      for (service_idx = 0; service_idx < appInfo_ptr->bc.cd.bc_table_size; service_idx++)
      {
        service_ptr = &appInfo_ptr->bc.cd.bc_table[service_idx];

        if ( (TRUE == service_ptr->active_service) &&
             ((word)cmd_ptr->cmd.bc_mm_select_srv.srv_id.bc_srv_id.service == service_ptr->service) &&
             ((byte)cmd_ptr->cmd.bc_mm_select_srv.srv_id.bc_srv_id.language == service_ptr->language) )
        {
          found = TRUE;
          MSG_HIGH_1("select_srv_proc: found entry at index %d", service_idx);
          break; // Out of for()
        }
      }

      if (!found)
      {
        MSG_HIGH_0("select_srv_proc: entry not found");
        cmd_err = WMS_CMD_ERR_BC_BAD_SRV_ID;
        break; // Out of do/while
      }

      // Allocate the event buffer
      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }

      // If selection has changed, NV must be updated
      if (cmd_ptr->cmd.bc_mm_select_srv.selected != service_ptr->selected)
      {
        must_update_nv = TRUE;
      }

      // Copy the current service information to an nv item
      lnvi.sms_bc_service_table = *service_ptr;

      // Change the NV item to reflect the select/deselect
      lnvi.sms_bc_service_table.selected = cmd_ptr->cmd.bc_mm_select_srv.selected;

      // Update the NV item if required
      if (must_update_nv)
      {
        nv_stat_enum_type  write_status;

        // Write to NV
        MSG_HIGH_2( "Update service table entry %d with index = %d",
                    service_idx,
                    service_ptr->index );

        write_status = wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, &lnvi );

        if (NV_DONE_S != write_status)
        {
          MSG_ERROR_0("BC Table NV write failed");
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
          break;
        }
      }
    } while(0);

    // Send command status
    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      boolean bc_enable_cur;
      boolean bc_enable_new;

      // Update the broadcast table num_selected
      if ((TRUE == cmd_ptr->cmd.bc_mm_select_srv.selected) &&
          (FALSE == service_ptr->selected) )
      {
        appInfo_ptr->bc.cd.bc_num_selected++; // Not used by modem api
      }
      else if ((FALSE == cmd_ptr->cmd.bc_mm_select_srv.selected) &&
               (TRUE == service_ptr->selected) )
      {
        appInfo_ptr->bc.cd.bc_num_selected--; // Not used by modem api
      }

      bc_enable_cur = appInfo_ptr->bc.cd.bc_enable;   // Save current state of enable
      bc_enable_new = wms_bc_enable_check(appInfo_ptr);    // Get new enable state (outside of crit_sect!)

      rex_enter_crit_sect(&wms_bc_crit_sect);               // Exclusive write
      *service_ptr           = lnvi.sms_bc_service_table;   // Used by modem api
      appInfo_ptr->bc.cd.bc_enable = bc_enable_new;               // Used by modem api
      rex_leave_crit_sect(&wms_bc_crit_sect);

      // Notify CM only if bc_enable flag has changed
      if( bc_enable_cur != bc_enable_new )
      {
        MSG_HIGH_1("BCSMS enable=%d", appInfo_ptr->bc.cd.bc_enable);
        wms_cm_bc_enable( appInfo_ptr->bc.cd.bc_enable );
      }

      // Prep the event
      {
        nv_sms_bc_service_table_options_type  *opt_ptr;

        bc_mm_event_ptr->as_id                                      = cmd_ptr->hdr.as_id;
        bc_mm_event_ptr->message_mode                               = cmd_ptr->cmd.bc_mm_select_srv.message_mode;
        bc_mm_event_ptr->u.bc_mm_srv_info.srv_id.bc_srv_id.service  = (wms_service_e_type)service_ptr->service;
        bc_mm_event_ptr->u.bc_mm_srv_info.srv_id.bc_srv_id.language = (wms_language_e_type)service_ptr->language;
        bc_mm_event_ptr->u.bc_mm_srv_info.selected                  = service_ptr->selected;
        bc_mm_event_ptr->u.bc_mm_srv_info.priority                  = (wms_priority_e_type)service_ptr->priority;
        (void)memscpy( bc_mm_event_ptr->u.bc_mm_srv_info.label,
                       WMS_BC_MM_SRV_LABEL_SIZE,
                       (char *) service_ptr->label,
                       WMS_BC_MM_SRV_LABEL_SIZE );

        opt_ptr = &appInfo_ptr->bc.cd.bc_table_options[service_idx];
        bc_mm_event_ptr->u.bc_mm_srv_info.label_encoding = (wms_user_data_encoding_e_type)opt_ptr->label_encoding;
        bc_mm_event_ptr->u.bc_mm_srv_info.alert          = (wms_bc_alert_e_type)opt_ptr->bc_alert;
        bc_mm_event_ptr->u.bc_mm_srv_info.max_messages   = opt_ptr->max_messages;
      }

      // Send event to client
      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SRV_UPDATED, bc_mm_event_ptr );
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_SRV_UPDATED event not sent: error %d", cmd_err);
    }

    WMS_MEM_FREEIF(bc_mm_event_ptr);
#endif /* FEATURE_CDSMS_BROADCAST */
  }

  return;
} /* wms_bc_mm_select_srv_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_get_all_srv_ids_proc()                          (INTERNAL)

DESCRIPTION
  This function is used to retrieve all the service IDs in the broadcast
  SMS service table. After the retrieval of the service IDs, the function
  wms_bc_get_service_info() can be called to retrieve the complete
  table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_get_all_srv_ids_proc(
  wms_cmd_type  *cmd_ptr
)
{
  wms_bc_mm_event_info_s_type    *bc_mm_event_ptr = NULL;
  sys_modem_as_id_e_type          as_id           = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type       *appInfo_ptr     = cmd_ptr->hdr.appInfo_ptr;
  wms_cmd_err_e_type              cmd_err         = WMS_CMD_ERR_NONE;

  MSG_HIGH_0("WMS_CMD_BC_MM_GET_ALL_SRV_IDS");

  if (NULL == appInfo_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
    return;
  }

  do
  {
    boolean table_ok;

    bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

    if (NULL == bc_mm_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
      break;
    }

    table_ok = wms_bc_mm_make_srv_ids_event( appInfo_ptr,
                                             cmd_ptr->cmd.bc_mm_get_all_srv_ids.message_mode,
                                             &bc_mm_event_ptr->u.bc_mm_srv_ids );
    if (!table_ok)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
      break;
    }
  } while (0);

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    bc_mm_event_ptr->as_id        = as_id;
    bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_get_all_srv_ids.message_mode;
    bc_mm_event_ptr->user_data    = cmd_ptr->hdr.user_data;

    wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SRV_IDS, bc_mm_event_ptr );
  }
  else
  {
    MSG_ERROR_1( "WMS_BC_MM_EVENT_SRV_IDS event not sent: error %d", cmd_err);
  }

  if (NULL != bc_mm_event_ptr)
  {
    WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_table.entries);
    wms_mem_free(bc_mm_event_ptr);
  }

  return;
} /* wms_bc_mm_get_all_srv_ids_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_get_srv_info_proc()                             (INTERNAL)

DESCRIPTION
  This function is used to retrieve a table entry in the broadcast SMS
  service table using a service ID.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_get_srv_info_proc(
  wms_cmd_type  *cmd_ptr
)
{
  sys_modem_as_id_e_type          as_id           = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type       *appInfo_ptr     = cmd_ptr->hdr.appInfo_ptr;

  MSG_HIGH_0("WMS_CMD_BC_MM_GET_SRV_INFO");

  if (NULL == appInfo_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
    return;
  }

  if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_get_srv_info.message_mode)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    wms_bc_mm_event_info_s_type       *bc_mm_event_ptr   = NULL;
    nv_sms_gw_cb_service_table_type   *gw_cb_service_ptr = NULL;
    wms_cmd_err_e_type                 cmd_err           = WMS_CMD_ERR_NONE;

    do
    {
      boolean                            found_dup;
      boolean                            found_free;
      uint32                             service_idx;

      /* find the entry matching service */
      wms_bc_search_gw_cb_table_entry( appInfo_ptr,
                                       &cmd_ptr->cmd.bc_mm_get_srv_info.srv_id.gw_cb_range,
                                       &found_free,
                                       &found_dup,
                                       &service_idx,
                                       &gw_cb_service_ptr );

      if (!found_dup)
      {
        MSG_HIGH_0("BC MM GET_SRV_INFO: entry not found");
        cmd_err = WMS_CMD_ERR_BC_BAD_SRV_ID;
        break;
      }

      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }
    } while(0);

    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      // Prep the event
      bc_mm_event_ptr->as_id        = as_id;
      bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_get_srv_info.message_mode;

      wms_bc_make_gw_cb_srv_info_event( gw_cb_service_ptr,
                                        &bc_mm_event_ptr->u.bc_mm_srv_info );

      // Send event to clients
      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SRV_INFO, bc_mm_event_ptr );
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_SRV_INFO event not sent: error %d", cmd_err);
    }

    WMS_MEM_FREEIF(bc_mm_event_ptr);
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    wms_bc_mm_event_info_s_type   *bc_mm_event_ptr   = NULL;
    nv_sms_bc_service_table_type  *service_ptr       = NULL;
    wms_cmd_err_e_type             cmd_err           = WMS_CMD_ERR_NONE;
    uint32                         service_idx;

    do
    {
      boolean                        found             = FALSE;

      // Find the entry matching service and language
      for (service_idx = 0; service_idx < appInfo_ptr->bc.cd.bc_table_size; service_idx++)
      {
        service_ptr = &appInfo_ptr->bc.cd.bc_table[service_idx];

        if ( (TRUE == service_ptr->active_service) &&
             ((word)cmd_ptr->cmd.bc_mm_get_srv_info.srv_id.bc_srv_id.service == service_ptr->service) &&
             ((byte)cmd_ptr->cmd.bc_mm_get_srv_info.srv_id.bc_srv_id.language == service_ptr->language) )
        {
          found = TRUE;
          MSG_HIGH_1("BC GET_SRV_INFO: found entry at index %d", service_idx);
          break;
        }
      }

      if (!found)
      {
        MSG_HIGH_0("BC GET_SRV_INFO: entry not found");
        cmd_err = WMS_CMD_ERR_BC_BAD_SRV_ID;
        break;
      }

      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }
    } while(0);

    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      // Prep the event
      {
        nv_sms_bc_service_table_options_type  *opt_ptr;

        bc_mm_event_ptr->as_id                                      = cmd_ptr->hdr.as_id;
        bc_mm_event_ptr->message_mode                               = cmd_ptr->cmd.bc_mm_get_srv_info.message_mode;
        bc_mm_event_ptr->u.bc_mm_srv_info.srv_id.bc_srv_id.service  = (wms_service_e_type)service_ptr->service;
        bc_mm_event_ptr->u.bc_mm_srv_info.srv_id.bc_srv_id.language = (wms_language_e_type)service_ptr->language;
        bc_mm_event_ptr->u.bc_mm_srv_info.selected                  = service_ptr->selected;
        bc_mm_event_ptr->u.bc_mm_srv_info.priority                  = (wms_priority_e_type)service_ptr->priority;
        (void)memscpy( bc_mm_event_ptr->u.bc_mm_srv_info.label,
                       WMS_BC_MM_SRV_LABEL_SIZE,
                       (char *) service_ptr->label,
                       WMS_BC_MM_SRV_LABEL_SIZE );

        opt_ptr = &appInfo_ptr->bc.cd.bc_table_options[service_idx];
        bc_mm_event_ptr->u.bc_mm_srv_info.label_encoding = (wms_user_data_encoding_e_type)opt_ptr->label_encoding;
        bc_mm_event_ptr->u.bc_mm_srv_info.alert          = (wms_bc_alert_e_type)opt_ptr->bc_alert;
        bc_mm_event_ptr->u.bc_mm_srv_info.max_messages   = opt_ptr->max_messages;
      }

      // Send event to client
      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SRV_INFO, bc_mm_event_ptr );
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_SRV_INFO event not sent: error %d", cmd_err);
    }

    WMS_MEM_FREEIF(bc_mm_event_ptr);
#endif /* FEATURE_CDSMS_BROADCAST */
  }

  return;
} /* wms_bc_mm_get_srv_info_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_add_single_srv()                                (INTERNAL)

DESCRIPTION
  This function is used to add a single entry

DEPENDENCIES
  None

RETURN VALUE
  WMS Command Error

SIDE EFFECTS
  None

=========================================================================*/
wms_cmd_err_e_type wms_bc_mm_add_single_srv(
  wms_prov_app_info_s_type          *appInfo_ptr,
  wms_message_mode_e_type            message_mode,
  wms_bc_mm_service_info_s_type     *srv_info_ptr,
  boolean                            write_to_nv /* if TRUE, add to nv as well */
)
{
  uint32                     tbl_index   = 0;
  nv_item_type               nvi;
  boolean                    found_free  = FALSE;
  boolean                    found_dup   = FALSE;
  wms_cmd_err_e_type         cmd_err     = WMS_CMD_ERR_NONE;

  if (NULL == appInfo_ptr)
  {
    return WMS_CMD_ERR_NULL_PTR;
  }

  if (message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    nv_sms_gw_cb_service_table_type   *gw_cb_service_ptr = NULL;

    if( srv_info_ptr == NULL )
    {
      cmd_err = WMS_CMD_ERR_NULL_PTR;
    }
    else if( appInfo_ptr->bc.gw.gw_cb_num_active >= WMS_BC_MM_TABLE_MAX )
    {
      /* table is full
      */
      cmd_err = WMS_CMD_ERR_BC_TABLE_FULL;
    }
    else
    {
      wms_bc_search_gw_cb_table_entry( appInfo_ptr,
                                       &srv_info_ptr->srv_id.gw_cb_range,
                                       &found_free,
                                       &found_dup,
                                       &tbl_index,
                                       &gw_cb_service_ptr );
      if( found_dup == FALSE)
      {
        found_dup = wms_cb_search_gw_cb_table_entry_in_range( appInfo_ptr,
                                                              &srv_info_ptr->srv_id.gw_cb_range,
                                                              &tbl_index,
                                                              &gw_cb_service_ptr );
      }
      if( found_dup == TRUE )
      {
        MSG_HIGH_1("BC MM ADD_SRV: found dup entry %d", tbl_index);
        /* Flag this item to be in sim, fields other than service id may need update */
        appInfo_ptr->bc.gw.in_sim[tbl_index] = TRUE;
      }
      else
      {
        if( found_free == FALSE )
        {
          /* there is no free entry within the current table;
            use the free entry just after the current table
          */

          tbl_index = appInfo_ptr->bc.gw.gw_cb_table_size;
          gw_cb_service_ptr = &appInfo_ptr->bc.gw.gw_cb_table[tbl_index];

        }
        /* else, use the free entry found earlier */
      }
    }

    if (( cmd_err == WMS_CMD_ERR_NONE ) && (gw_cb_service_ptr != NULL))
    {
      rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write

      /* update the entry and bc info
      */
      /* 'tbl_index' is initialized already if cmd_err==WMS_CMD_ERR_NONE */
      gw_cb_service_ptr->index          =  (byte)tbl_index;                    // Not used by modem api
      gw_cb_service_ptr->active_service = TRUE;                                // Used by modem api

      /* Dont overwrite presidential alerts if the new range to be added has the "selected" field disabled.
         For example, existing table has entry 4383 - 4383 (selected)
         And entry to be added is 4382-4392 (unselected).
         In this case, entry 4383-4383 should be retained. */
      if ( (TRUE == found_dup) && 
           ( ( (gw_cb_service_ptr->service_from == WMS_GW_CB_SRV_ID_CMAS_MSGID1) &&
               (gw_cb_service_ptr->service_to == WMS_GW_CB_SRV_ID_CMAS_MSGID1) ) ||
             ( (gw_cb_service_ptr->service_from == WMS_GW_CB_SRV_ID_CMAS_MSGID14) &&
               (gw_cb_service_ptr->service_to == WMS_GW_CB_SRV_ID_CMAS_MSGID14) ) ) &&
           (gw_cb_service_ptr->selected && !srv_info_ptr->selected) )
            
              
      {
         MSG_HIGH_0("Presidential alert is not overwritten");
      }
      else
      {
         gw_cb_service_ptr->service_from = srv_info_ptr->srv_id.gw_cb_range.from; // Used by modem api
         gw_cb_service_ptr->service_to = srv_info_ptr->srv_id.gw_cb_range.to;     // Used by modem api
         gw_cb_service_ptr->selected = srv_info_ptr->selected;                    // Used by modem api
  
         (void)memscpy( (char *) gw_cb_service_ptr->label,                        // Not used by modem api
                     WMS_BC_MM_SRV_LABEL_SIZE,
                     srv_info_ptr->label,
                     WMS_BC_MM_SRV_LABEL_SIZE );
         gw_cb_service_ptr->label_encoding = (byte)srv_info_ptr->label_encoding;  // Not used by modem api
         gw_cb_service_ptr->bc_alert = (byte)srv_info_ptr->alert;                 // Not used by modem api
         gw_cb_service_ptr->max_messages = srv_info_ptr->max_messages;            // Not used by modem api
      }

      if( found_dup != TRUE )
      {
        /* We have one more active entry
        */
        appInfo_ptr->bc.gw.gw_cb_num_active ++;                                         // Not used by modem api

        /* We may have one more selected entry
        */
        if (TRUE == gw_cb_service_ptr->selected)
        {
          appInfo_ptr->bc.gw.gw_cb_num_selected ++;                                     // Not used by modem api
        }

        if( found_free == FALSE )
        {
          // Increase the table size if extending table (not using a free one)
          appInfo_ptr->bc.gw.gw_cb_table_size ++;                                      // Used by modem api
        }
      }

      rex_leave_crit_sect(&wms_bc_crit_sect);

      /* write to NV if needed
      */
      if (TRUE == write_to_nv)
      {
        // Write the table entry to NV
        nvi.sms_gw_cb_service_table = * gw_cb_service_ptr;
        if( wms_nv_write_wait_per_subs( appInfo_ptr->asid, NV_SMS_GW_CB_SERVICE_TABLE_I, &nvi ) != NV_DONE_S )
        {
          MSG_ERROR_0("GW CB Table NV write failed");
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
        }

        // Write the new table size back to NV if needed
        nvi.sms_gw_cb_service_table_size = appInfo_ptr->bc.gw.gw_cb_table_size;
        if( wms_nv_write_wait_per_subs( appInfo_ptr->asid, NV_SMS_GW_CB_SERVICE_TABLE_SIZE_I, &nvi ) != NV_DONE_S )
        {
          MSG_ERROR_0("BC Table Size NV write failed");
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
        }

        appInfo_ptr->bc.gw.in_nv[tbl_index] = TRUE;   // Not used by modem api
      }
      else
      {
        appInfo_ptr->bc.gw.in_nv[tbl_index] = FALSE;  // Not used by modem api
      }
      /* Flag this item to be in sim */
      appInfo_ptr->bc.gw.in_sim[tbl_index] = TRUE;    // Not used by modem api

    }
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST

    if( srv_info_ptr == NULL )
    {
      cmd_err = WMS_CMD_ERR_NULL_PTR;
    }
    else if( appInfo_ptr->bc.cd.bc_num_active == WMS_BC_TABLE_MAX )
    {
      /* table is full
      */
      cmd_err = WMS_CMD_ERR_BC_TABLE_FULL;
    }
    else
    {
      nv_sms_bc_service_table_type   *service_ptr = NULL;
      nv_sms_bc_service_table_options_type   *opt_ptr = NULL;

      wms_bc_search_table_entry( appInfo_ptr,
                                 &srv_info_ptr->srv_id.bc_srv_id,
                                 &found_free,
                                 &found_dup,
                                 &tbl_index,
                                 &service_ptr,
                                 &opt_ptr );

      if( found_dup == TRUE )
      {
        MSG_HIGH_1("BC ADD_SRV: found dup entry %d", tbl_index);
      }
      else
      {
        if( found_free == FALSE )
        {
          /* there is no free entry within the current table;
            use the free entry just after the current table
          */
          tbl_index = appInfo_ptr->bc.cd.bc_table_size; // Bounds are implicitly checked by num_active < MAX
          service_ptr = &appInfo_ptr->bc.cd.bc_table[tbl_index];                       // Used by modem api
          opt_ptr = &appInfo_ptr->bc.cd.bc_table_options[tbl_index];                   // Not used by modem api
        }
        /* else, use the free entry found earlier */

        rex_enter_crit_sect(&wms_bc_crit_sect);  // Exclusive write
        /* update the entry and bc info */
        service_ptr->index = (uint8)tbl_index;                                 // Not used by modem api
        service_ptr->active_service = TRUE;                                    // Used by modem api
        service_ptr->service = (word)srv_info_ptr->srv_id.bc_srv_id.service;   // Used by modem api
        service_ptr->language = (byte)srv_info_ptr->srv_id.bc_srv_id.language; // Used by modem api
        service_ptr->selected = srv_info_ptr->selected;                        // Used by modem api
        service_ptr->priority = (byte)srv_info_ptr->priority;                  // Used by modem api
        (void)memscpy( (char *) service_ptr->label,                            // Not used by modem api
                       WMS_BC_MM_SRV_LABEL_SIZE,
                       srv_info_ptr->label,
                       WMS_BC_MM_SRV_LABEL_SIZE );

        if (FALSE == found_free) {
          // Increase the table size if extending table (not using a free one)
          appInfo_ptr->bc.cd.bc_table_size ++;                                          // Used by modem api
        }
        rex_leave_crit_sect(&wms_bc_crit_sect);

        opt_ptr->index          = service_ptr->index;
        opt_ptr->label_encoding = (byte)srv_info_ptr->label_encoding;
        opt_ptr->bc_alert       = (byte)srv_info_ptr->alert;
        opt_ptr->max_messages   = srv_info_ptr->max_messages;

        /* We have one more active entry
        */
        appInfo_ptr->bc.cd.bc_num_active ++;  // Not used by modem api

        /* We may have one more selected entry
        */
        if( service_ptr->selected == TRUE)
        {
          appInfo_ptr->bc.cd.bc_num_selected ++;  // Not used by modem api
        }

        /* write to NV
        */
        nvi.sms_bc_service_table = * service_ptr;
        MSG_HIGH_2( "Update service table entry %d with index = %d",
                    tbl_index,
                    service_ptr->index );
        if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, &nvi )
            != NV_DONE_S )
        {
          MSG_ERROR_0("BC Table NV write failed");
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
        }
        else
        {
          nvi.sms_bc_service_table_options = * opt_ptr;
          if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_OPTIONS_I,
                                &nvi ) != NV_DONE_S )
          {
            MSG_ERROR_0("BC Table NV write failed");
            cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
          }
          else
          {
            /* Write the new table size back to NV */
            nvi.sms_bc_service_table_size = appInfo_ptr->bc.cd.bc_table_size;
            if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_SIZE_I, &nvi )
              != NV_DONE_S )
            {
              MSG_ERROR_0("BC Table Size NV write failed");
              cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
            }
          }
        }
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  return cmd_err;
} /* wms_bc_mm_add_single_srv() */

/*===========================================================================
FUNCTION wms_bc_mm_add_srv_proc()                                  (INTERNAL)

DESCRIPTION
  This function is used to add entries to the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_add_srv_proc(
  wms_cmd_type  *cmd_ptr
)
{
  wms_cmd_err_e_type        cmd_err     = WMS_CMD_ERR_NONE;
  uint32                    i;
  sys_modem_as_id_e_type    as_id       = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type *appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  MSG_HIGH_1("WMS_CMD_BC_MM_ADD_SRV: msg mode = %d", cmd_ptr->cmd.bc_mm_add_srv.message_mode);

  if (NULL == appInfo_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
    return;
  }

  if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_add_srv.message_mode)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    boolean write_to_nv = FALSE;

    for (i = 0; i < cmd_ptr->cmd.bc_mm_add_srv.num_entries; i++)
    {
      /* Write to NV if cbmi or cbmir do not exist */
      if (cmd_ptr->cmd.bc_mm_add_srv.entries[i].srv_id.gw_cb_range.from ==
          cmd_ptr->cmd.bc_mm_add_srv.entries[i].srv_id.gw_cb_range.to)
      {
         if (appInfo_ptr->bc.gw.sim_cbmi_rec_len < 2)
         {
            write_to_nv = TRUE;
         }
      }
      else
      {
         if (appInfo_ptr->bc.gw.sim_cbmir_rec_len < 4)
         {
            write_to_nv = TRUE;
         }
      }

      cmd_err = wms_bc_mm_add_single_srv( appInfo_ptr,
                                          WMS_MESSAGE_MODE_GW,
                                          &cmd_ptr->cmd.bc_mm_add_srv.entries[i],
                                          write_to_nv);

      if (cmd_err != WMS_CMD_ERR_NONE )
      {
        /* Something goes wrong, send command error and stop processing. */
        wms_client_cmd_status( cmd_ptr, cmd_err );
        break;
      }
    } /* for() */

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      if( wms_bc_update_gw_cb_table_to_sim(as_id) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_BC_SIM_WRITE;
        wms_client_cmd_status( cmd_ptr, cmd_err );
      }
    }

    /* Send table event to clients */
    if ( (WMS_CMD_ERR_NONE == cmd_err) ||
         (WMS_CMD_ERR_BC_SIM_WRITE == cmd_err) )
    {
      wms_bc_mm_event_info_s_type *bc_mm_event_ptr = NULL;
      boolean                      table_ok;

      cmd_err = WMS_CMD_ERR_NONE;

      do
      {
        bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

        if (NULL == bc_mm_event_ptr)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
          break;
        }

        table_ok = wms_bc_mm_make_table_event( appInfo_ptr,
                                               WMS_MESSAGE_MODE_GW,
                                               &bc_mm_event_ptr->u.bc_mm_table );
        if (!table_ok)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        }
      } while(0);

      wms_client_cmd_status( cmd_ptr, cmd_err );

      if (WMS_CMD_ERR_NONE == cmd_err)
      {
        boolean gw_cb_enable_new;

        // Check to see if we need to enable BC to lower layers
        gw_cb_enable_new = wms_bc_gw_cb_enable_check( appInfo_ptr->bc.gw.gw_cb_config,
                                                      appInfo_ptr->bc.gw.gw_cb_pref,
                                                      appInfo_ptr->bc.gw.gw_cb_num_selected );

        rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
        appInfo_ptr->bc.gw.gw_cb_enable = gw_cb_enable_new;  // Used by modem api
        rex_leave_crit_sect(&wms_bc_crit_sect);

        MSG_HIGH_1("GW CB enable=%d", appInfo_ptr->bc.gw.gw_cb_enable);
        wms_bc_gw_cb_enable_to_nas(appInfo_ptr);

        // Prep event
        bc_mm_event_ptr->as_id        = as_id;
        bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_add_srv.message_mode;
        bc_mm_event_ptr->user_data    = cmd_ptr->hdr.user_data;

        // Send event to clients
        wms_bc_mm_event_notify(WMS_BC_MM_EVENT_ADD_SRVS, bc_mm_event_ptr );
      }
      else
      {
        MSG_ERROR_1( "WMS_BC_MM_EVENT_ADD_SRVS event not sent: error %d", cmd_err);
      }

      if (NULL != bc_mm_event_ptr)
      {
        WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_table.entries);
        wms_mem_free(bc_mm_event_ptr);
      }
    }
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    for (i = 0; i < cmd_ptr->cmd.bc_mm_add_srv.num_entries; i++)
    {
      cmd_err = wms_bc_mm_add_single_srv( appInfo_ptr,
                                          WMS_MESSAGE_MODE_CDMA,
                                          &cmd_ptr->cmd.bc_mm_add_srv.entries[i],
                                          TRUE);

      if (cmd_err != WMS_CMD_ERR_NONE)
      {
        /* Something goes wrong, send command error and stop processing. */
        wms_client_cmd_status( cmd_ptr, cmd_err );
        break;
      }
    } /* for() */

    /* Send table event to clients */
    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      wms_bc_mm_event_info_s_type *bc_mm_event_ptr = NULL;
      boolean                      table_ok;

      do
      {
        bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

        if (NULL == bc_mm_event_ptr)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
          break;
        }

        table_ok = wms_bc_mm_make_table_event( appInfo_ptr,
                                               WMS_MESSAGE_MODE_CDMA,
                                               &bc_mm_event_ptr->u.bc_mm_table );
        if (!table_ok)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        }
      } while(0);

      wms_client_cmd_status( cmd_ptr, cmd_err );

      if (WMS_CMD_ERR_NONE == cmd_err)
      {
        /* Check to see if we need to enable BC to lower layers
        */
        wms_bc_update_1x_bcsms_setting(appInfo_ptr);

        // Prep the event
        bc_mm_event_ptr->as_id        = as_id;
        bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_add_srv.message_mode;
        bc_mm_event_ptr->user_data    = cmd_ptr->hdr.user_data;

        // Send event to clients
        wms_bc_mm_event_notify(WMS_BC_MM_EVENT_ADD_SRVS, bc_mm_event_ptr );
      }
      else
      {
        MSG_ERROR_1( "WMS_BC_MM_EVENT_ADD_SRVS event not sent: error %d", cmd_err);
      }

      if (NULL != bc_mm_event_ptr)
      {
        WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_table.entries);
        wms_mem_free(bc_mm_event_ptr);
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }

  WMS_MEM_FREEIF( cmd_ptr->cmd.bc_mm_add_srv.entries );

  return;
} /* wms_bc_mm_add_srv_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_check_presidential_alert_entry_count_in_table()            (INTERNAL)

DESCRIPTION
  This function returns the number of entries of a specific presidential
  message ID in the service table.

RETURN VALUE
  Number of entries of a specific presidential message ID in the service table.

SIDE EFFECTS
  None

=========================================================================*/
int wms_bc_mm_check_presidential_alert_entry_count_in_table(
  wms_prov_app_info_s_type       *appInfo_ptr,
  wms_message_mode_e_type         message_mode,
  wms_bc_mm_srv_id_type          srv_id_arr_item
)
{
  uint16  i;
  int     pres_alert_entry_count = 0;

  for (i = 0; i < MIN(appInfo_ptr->bc.gw.gw_cb_table_size, WMS_BC_MM_TABLE_MAX); i++)
  {
    if ((srv_id_arr_item.gw_cb_range.from <= WMS_GW_CB_SRV_ID_CMAS_MSGID1) &&
        (srv_id_arr_item.gw_cb_range.to >= WMS_GW_CB_SRV_ID_CMAS_MSGID1))
    {
      if ((appInfo_ptr->bc.gw.gw_cb_table[i].active_service == TRUE) &&
          (appInfo_ptr->bc.gw.gw_cb_table[i].service_from <= WMS_GW_CB_SRV_ID_CMAS_MSGID1) &&
          (appInfo_ptr->bc.gw.gw_cb_table[i].service_to >= WMS_GW_CB_SRV_ID_CMAS_MSGID1))
      {
        pres_alert_entry_count++;
      }
    }
  
    if ((srv_id_arr_item.gw_cb_range.from <= WMS_GW_CB_SRV_ID_CMAS_MSGID14) &&
        (srv_id_arr_item.gw_cb_range.to   >= WMS_GW_CB_SRV_ID_CMAS_MSGID14))
    {
      if ((appInfo_ptr->bc.gw.gw_cb_table[i].active_service == TRUE) &&
          (appInfo_ptr->bc.gw.gw_cb_table[i].service_from <= WMS_GW_CB_SRV_ID_CMAS_MSGID14) &&
          (appInfo_ptr->bc.gw.gw_cb_table[i].service_to >= WMS_GW_CB_SRV_ID_CMAS_MSGID14))
      {
        pres_alert_entry_count++;
      }
    }
  }

  MSG_HIGH_1("Number of presidential alert entries in the service table is %d", pres_alert_entry_count);

  return pres_alert_entry_count;
} /* wms_bc_mm_check_presidential_alert_entry_count_in_table() */

/*===========================================================================
FUNCTION wms_bc_mm_is_delete_of_pres_entry_possible()            (INTERNAL)

DESCRIPTION
  This function checks whether the presidential entry can be deleted
  because of the presence of same presidential entry  in the service
  table more than once.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_bc_mm_is_delete_of_pres_entry_possible(
  wms_prov_app_info_s_type       *appInfo_ptr,
  wms_message_mode_e_type         message_mode,
  wms_bc_mm_srv_id_type*           srv_id_arr,
  uint8                            pres_entry_count_in_client_req,
  nv_sms_gw_cb_service_table_type *gw_cb_service_ptr
)
{
  uint16                    i;
  int                       deletable_entry_count = 0;
  int                       delete_index;
  boolean                   ok_to_delete = FALSE;

    if (!wms_check_appinfo_is_valid(appInfo_ptr))
    {
      return ok_to_delete;
    }

  if (WMS_MESSAGE_MODE_GW == message_mode)
  {
#ifdef FEATURE_GWSMS_BROADCAST

    for (i = 0; i < pres_entry_count_in_client_req; i++)
    {
      int  pres_alert_entry_count_in_table = 0;

      pres_alert_entry_count_in_table = wms_bc_mm_check_presidential_alert_entry_count_in_table(appInfo_ptr, WMS_MESSAGE_MODE_GW, srv_id_arr[i]);

      if (pres_alert_entry_count_in_table > 1)
      {
        // If the same presidential entry is present more than once, this entry is deletable.
        deletable_entry_count ++;
        delete_index = i;		  
      }
    }

/*************************************************************************************************************************
                                CASES : - 
 Case#  pres_entry_count_in_client_req    deleteable_entry_count    comments 
  a      1                                  0                         There is one presidential entry in the input
                                                                      range and that can't be deleted.
                                                                      Overwrite the table entry with presidential entry
  b      1                                  1                         There is one presidential entry in the input
                                                                      range and that can be deleted. ok_to_delete = TRUE.
  c      2                                  0                         There are two presidential entries in the input
                                                                      range and both can't be deleted.
  d      2                                  1                         There are two presidential entries in the input
                                                                      range and only one of them can be deleted.
  e      2                                  2                         There are two presidential entries in the input
                                                                      range and both can be deleted.
*************************************************************************************************************************/

    if (pres_entry_count_in_client_req == deletable_entry_count)
    {
      // cases b and e : the table entry will be deleted.
      ok_to_delete = TRUE;
    }
    else if (1 == pres_entry_count_in_client_req && 0 == deletable_entry_count)
    {
      // case a : table entry will be overwritten with the presidential ID.
      rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
      gw_cb_service_ptr->service_from = srv_id_arr[0].gw_cb_range.from;
      gw_cb_service_ptr->service_to   = srv_id_arr[0].gw_cb_range.to;
      rex_leave_crit_sect(&wms_bc_crit_sect);
    }
    else if (2 == pres_entry_count_in_client_req && 1 == deletable_entry_count)
    {
      // case d : table entry will be overwritten with the non deletable entry.
      rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write					
      gw_cb_service_ptr->service_from   = srv_id_arr[1 - delete_index].gw_cb_range.from;
      gw_cb_service_ptr->service_to	= srv_id_arr[1 - delete_index].gw_cb_range.to;
      rex_leave_crit_sect(&wms_bc_crit_sect);
    }
    else if (2 == pres_entry_count_in_client_req && 0 == deletable_entry_count)
    {
      // case c : table entry will be split into two presidential message ID entries.
      wms_bc_mm_service_info_s_type srv_info;

      rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
      gw_cb_service_ptr->service_from = srv_id_arr[0].gw_cb_range.from;
      gw_cb_service_ptr->service_to = srv_id_arr[0].gw_cb_range.to;
      rex_leave_crit_sect(&wms_bc_crit_sect);

      srv_info.alert = gw_cb_service_ptr->bc_alert;
      memscpy(srv_info.label, NV_SMS_BC_SRV_LABEL_SIZE, gw_cb_service_ptr->label, NV_SMS_BC_SRV_LABEL_SIZE);
      srv_info.label_encoding = gw_cb_service_ptr->label_encoding;
      srv_info.max_messages   = gw_cb_service_ptr->max_messages;
      srv_info.selected       = gw_cb_service_ptr->selected;
      //srv_info.priority ?

      srv_info.srv_id.gw_cb_range.from = srv_id_arr[1].gw_cb_range.from;
      srv_info.srv_id.gw_cb_range.to   = srv_id_arr[1].gw_cb_range.to;

      wms_bc_mm_add_single_srv(appInfo_ptr, WMS_MESSAGE_MODE_GW, &srv_info, FALSE);
    }
#endif /* FEATURE_GWSMS_BROADCAST */
  }

 
  return ok_to_delete;
} /* wms_bc_mm_is_delete_of_pres_entry_possible() */


/*===========================================================================
FUNCTION wms_bc_mm_delete_single_srv()                             (INTERNAL)

DESCRIPTION
  This function is used to delete a single entry

DEPENDENCIES
  None

RETURN VALUE
  WMS Command Error

SIDE EFFECTS
  None

=========================================================================*/
wms_cmd_err_e_type wms_bc_mm_delete_single_srv(
  wms_prov_app_info_s_type      *appInfo_ptr,
  wms_message_mode_e_type        message_mode,
  wms_bc_mm_srv_id_type         *srv_id_ptr
)
{
  wms_cmd_err_e_type        cmd_err     = WMS_CMD_ERR_NONE;
  uint32                    tbl_index;
  nv_item_type              nvi;

  if (NULL == appInfo_ptr)
  {
    return WMS_CMD_ERR_NULL_PTR;
  }

  if (message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    boolean              found_free = FALSE;
    boolean              found_dup  = FALSE;
    nv_sms_gw_cb_service_table_type   *gw_cb_service_ptr = NULL;

    if( srv_id_ptr == NULL )
    {
      cmd_err = WMS_CMD_ERR_NULL_PTR;
    }
    else
    {
      wms_bc_search_gw_cb_table_entry( appInfo_ptr,
                                       &srv_id_ptr->gw_cb_range,
                                       &found_free,
                                       &found_dup,
                                       &tbl_index,
                                       &gw_cb_service_ptr );

      if( found_dup == FALSE )
      {
        MSG_HIGH_0("BC MM DELETE_SRV: entry not found");
        cmd_err = WMS_CMD_ERR_BC_BAD_SRV_ID;
      }
      else
      {
        wms_bc_mm_srv_id_type  srv_id[2];
        uint8                  pres_entry_count_in_req;
        boolean                ok_to_delete = TRUE;

        gw_cb_service_ptr = &appInfo_ptr->bc.gw.gw_cb_table[tbl_index];
        srv_id[0].gw_cb_range.from = gw_cb_service_ptr->service_from;
        srv_id[0].gw_cb_range.to   = gw_cb_service_ptr->service_to;

        /* Get the count of how many presidential entries are present in the srv id range to be deleted. */
        pres_entry_count_in_req = wms_bc_mm_get_presidential_alert_entry_count_in_client_req(WMS_MESSAGE_MODE_GW, srv_id);

        if( pres_entry_count_in_req > 0 )
        {
          // It is OK to delete the specified entry that includes presidential alert message id
	  // if we have other same presidential alert entries in the service table.
          ok_to_delete = wms_bc_mm_is_delete_of_pres_entry_possible( appInfo_ptr, WMS_MESSAGE_MODE_GW, srv_id, pres_entry_count_in_req, gw_cb_service_ptr);

        }
        if( ok_to_delete )
        {
          rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write

          /* de-activate this entry
          */
          gw_cb_service_ptr->active_service = FALSE;  // Used by modem api

          /* we have one less active entry
          */
          appInfo_ptr->bc.gw.gw_cb_num_active --;              // Not used by modem api

          /* we may have one less selected entry
          */
          if (TRUE == gw_cb_service_ptr->selected)
          {
            gw_cb_service_ptr->selected = FALSE;      // Used by modem api
            appInfo_ptr->bc.gw.gw_cb_num_selected --;          // Not used by modem api
          }

          rex_leave_crit_sect(&wms_bc_crit_sect);
        }
        if (appInfo_ptr->bc.gw.in_nv[tbl_index] == TRUE)
        {
          /* write to NV
          */
          nvi.sms_gw_cb_service_table = * gw_cb_service_ptr;
          if( wms_nv_write_wait_per_subs( appInfo_ptr->asid, NV_SMS_GW_CB_SERVICE_TABLE_I, &nvi ) != NV_DONE_S )
          {
            MSG_ERROR_0("GW CB Table NV write failed");
            cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
          }
          // Write the new table size back to NV.
          nvi.sms_gw_cb_service_table_size = appInfo_ptr->bc.gw.gw_cb_table_size;
          if( wms_nv_write_wait_per_subs( appInfo_ptr->asid, NV_SMS_GW_CB_SERVICE_TABLE_SIZE_I, &nvi ) != NV_DONE_S )
          {
            MSG_ERROR_0("BC Table Size NV write failed");
            cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
          }
        }
      }
    }
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    boolean found = FALSE;
    nv_sms_bc_service_table_type   *service_ptr = NULL;

    /* search the table for a free entry
    */
    for (tbl_index = 0; tbl_index < appInfo_ptr->bc.cd.bc_table_size; tbl_index++ )
    {
      service_ptr = &appInfo_ptr->bc.cd.bc_table[tbl_index];

      if( service_ptr->active_service == TRUE &&
          service_ptr->service == (word)srv_id_ptr->bc_srv_id.service &&
          service_ptr->language == (byte)srv_id_ptr->bc_srv_id.language )
      {
        found = TRUE;
        break;
      }
    }

    if( found == FALSE )
    {
      MSG_HIGH_0("BC DELETE_SRV: entry not found");
      cmd_err = WMS_CMD_ERR_BC_BAD_SRV_ID;
    }
    else
    {
      if (NULL != service_ptr)
      {
        /* de-activate this entry
        */
        rex_enter_crit_sect(&wms_bc_crit_sect);  // Exclusive write
        service_ptr->active_service = FALSE;   // Used by modem api

        /* we have one less active entry */
        appInfo_ptr->bc.cd.bc_num_active --;            // Not used by modem api

        /* we may have one less selected entry */
        if (TRUE == service_ptr->selected)
        {
          service_ptr->selected = FALSE;       // Used by modem api
          appInfo_ptr->bc.cd.bc_num_selected --;        // Not used by modem api
        }
        rex_leave_crit_sect(&wms_bc_crit_sect);

        /* write to NV
        */
        nvi.sms_bc_service_table = * service_ptr;
        MSG_HIGH_2( "Update service table entry %d with index = %d",
                    tbl_index,
                    service_ptr->index );
        if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, &nvi )
            != NV_DONE_S )
        {
          MSG_ERROR_0("BC Table NV write failed");
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
        }
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }

  return cmd_err;
} /* wms_bc_mm_delete_single_srv() */

/*===========================================================================
FUNCTION wms_bc_mm_delete_srv_proc()                               (INTERNAL)

DESCRIPTION
  This function is used to delete entries from the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_delete_srv_proc(
  wms_cmd_type  *cmd_ptr
)
{
  uint32                    i;
  wms_cmd_err_e_type        cmd_err     = WMS_CMD_ERR_NONE;
  sys_modem_as_id_e_type    as_id       = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type *appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  MSG_HIGH_1("WMS_CMD_BC_MM_DELETE_SRV msg mode = %d", cmd_ptr->cmd.bc_mm_delete_srv.message_mode);

  if (NULL == appInfo_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
    return;
  }

  if( cmd_ptr->cmd.bc_mm_delete_srv.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
    for (i = 0; i < cmd_ptr->cmd.bc_mm_delete_srv.num_entries; i++)
    {
      cmd_err = wms_bc_mm_delete_single_srv( appInfo_ptr,
                                             WMS_MESSAGE_MODE_GW,
                                             &cmd_ptr->cmd.bc_mm_delete_srv.srv_ids[i]);

      if( cmd_err != WMS_CMD_ERR_NONE )
      {
        /* Something goes wrong, send command error and stop processing. */
        wms_client_cmd_status( cmd_ptr, cmd_err );
        break;
      }
    } /* for() */

    if( cmd_err == WMS_CMD_ERR_NONE )
    {
      if( wms_bc_update_gw_cb_table_to_sim(as_id) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_BC_SIM_WRITE;
        wms_client_cmd_status( cmd_ptr, cmd_err );
      }
    }

    /* Send event to clients */
    if ( (WMS_CMD_ERR_NONE == cmd_err) ||
         (WMS_CMD_ERR_BC_SIM_WRITE == cmd_err) )
    {
      wms_bc_mm_event_info_s_type *bc_mm_event_ptr = NULL;

      cmd_err = WMS_CMD_ERR_NONE;

      do
      {
        boolean                      table_ok;

        bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

        if (NULL == bc_mm_event_ptr)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
          break;
        }

        table_ok = wms_bc_mm_make_srv_ids_event( appInfo_ptr,
                                                 WMS_MESSAGE_MODE_GW,
                                                 &bc_mm_event_ptr->u.bc_mm_srv_ids );
        if (!table_ok)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        }
      } while(0);

      wms_client_cmd_status( cmd_ptr, cmd_err );

      if (WMS_CMD_ERR_NONE == cmd_err)
      {
        boolean gw_cb_enable_new;

        /* Check to see if we need to enable BC to lower layers
        */
        gw_cb_enable_new = wms_bc_gw_cb_enable_check( appInfo_ptr->bc.gw.gw_cb_config,
                                                      appInfo_ptr->bc.gw.gw_cb_pref,
                                                      appInfo_ptr->bc.gw.gw_cb_num_selected );

        rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
        appInfo_ptr->bc.gw.gw_cb_enable = gw_cb_enable_new;                 // Used by modem api
        rex_leave_crit_sect(&wms_bc_crit_sect);

        MSG_HIGH_1("GW CB enable=%d", appInfo_ptr->bc.gw.gw_cb_enable);
        wms_bc_gw_cb_enable_to_nas(appInfo_ptr);

        // Prep event
        bc_mm_event_ptr->as_id        = as_id;
        bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_delete_srv.message_mode;
        bc_mm_event_ptr->user_data    = cmd_ptr->hdr.user_data;

        // Send event to clients
        wms_bc_mm_event_notify( WMS_BC_MM_EVENT_DELETE_SRVS, bc_mm_event_ptr );
      }
      else
      {
        MSG_ERROR_1( "WMS_BC_MM_EVENT_DELETE_SRVS event not sent: error %d", cmd_err);
      }

      if (NULL != bc_mm_event_ptr)
      {
        WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_table.entries);
        wms_mem_free(bc_mm_event_ptr);
      }
    }
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    for (i = 0; i < cmd_ptr->cmd.bc_mm_delete_srv.num_entries; i++)
    {
      if (0 == wms_bc_mm_get_presidential_alert_entry_count_in_client_req(WMS_MESSAGE_MODE_CDMA,
                                                                          &cmd_ptr->cmd.bc_mm_delete_srv.srv_ids[i]))
      {
        cmd_err = wms_bc_mm_delete_single_srv( appInfo_ptr,
                                               WMS_MESSAGE_MODE_CDMA,
                                               &cmd_ptr->cmd.bc_mm_delete_srv.srv_ids[i]);
      }

      if (cmd_err != WMS_CMD_ERR_NONE)
      {
        /* Something goes wrong, send command error and stop processing. */
        wms_client_cmd_status( cmd_ptr, cmd_err );
        break;
      }
    } /* for() */

    /* Send event to clients */
    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      wms_bc_mm_event_info_s_type *bc_mm_event_ptr = NULL;

      do
      {
        boolean                      table_ok;

        bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

        if (NULL == bc_mm_event_ptr)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
          break;
        }

        table_ok = wms_bc_mm_make_srv_ids_event( appInfo_ptr,
                                                 WMS_MESSAGE_MODE_CDMA,
                                                 &bc_mm_event_ptr->u.bc_mm_srv_ids );
        if (!table_ok)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        }
      } while(0);

      wms_client_cmd_status( cmd_ptr, cmd_err );

      if (WMS_CMD_ERR_NONE == cmd_err)
      {
        // Check to see if we need to enable BC to lower layers
        wms_bc_update_1x_bcsms_setting(appInfo_ptr);

        // Prep event
        bc_mm_event_ptr->as_id        = as_id;
        bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_delete_srv.message_mode;
        bc_mm_event_ptr->user_data    = cmd_ptr->hdr.user_data;

        wms_bc_mm_event_notify( WMS_BC_MM_EVENT_DELETE_SRVS, bc_mm_event_ptr );
      }
      else
      {
        MSG_ERROR_1( "WMS_BC_MM_EVENT_DELETE_SRVS event not sent: error %d", cmd_err);
      }

      if (NULL != bc_mm_event_ptr)
      {
        WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_table.entries);
        wms_mem_free(bc_mm_event_ptr);
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }

  WMS_MEM_FREEIF( cmd_ptr->cmd.bc_mm_delete_srv.srv_ids );

  return;
} /* wms_bc_mm_delete_srv_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_change_srv_info_proc()                          (INTERNAL)

DESCRIPTION
  This function is used to change an entry in the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_change_srv_info_proc(
  wms_cmd_type  *cmd_ptr
)
{
  wms_prov_app_info_s_type *appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  MSG_HIGH_0("WMS_CMD_BC_MM_CHANGE_SRV_INFO");

  if (NULL == appInfo_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
    return;
  }

  if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_change_srv_info.message_mode)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    wms_bc_mm_event_info_s_type       *bc_mm_event_ptr   = NULL;
    wms_cmd_err_e_type                 cmd_err           = WMS_CMD_ERR_NONE;
    nv_sms_gw_cb_service_table_type   *gw_cb_service_ptr = NULL;
    sys_modem_as_id_e_type             as_id             = cmd_ptr->hdr.as_id;
    uint32                             tbl_index         = 0;

    do
    {
      nv_item_type                       lnvi;
      wms_bc_mm_srv_id_type              srv_id[2];

      srv_id[0] = cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.srv_id;

      // Check if the entry can be changed
      if (0 < wms_bc_mm_get_presidential_alert_entry_count_in_client_req(WMS_MESSAGE_MODE_GW,
                                                                         srv_id))
      {
        MSG_HIGH_0( "BC MM Change SRV INFO: change not allowed");
        cmd_err = WMS_CMD_ERR_BC_BAD_SRV_ID;
        break;
      }

      // Find the entry in the current table
      {
        boolean      found_dup = FALSE;
        boolean      found_free = FALSE;

        // Find the entry matching service and language */
        wms_bc_search_gw_cb_table_entry( appInfo_ptr,
                                         &cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.srv_id.gw_cb_range,
                                         &found_free,
                                         &found_dup,
                                         &tbl_index,
                                         &gw_cb_service_ptr );

        if (!found_dup)
        {
          MSG_HIGH_0("BC MM CHange SRV INFO: entry not found");
          cmd_err = WMS_CMD_ERR_BC_BAD_SRV_ID;
          break;
        }
      }

      // Update the table's num_selected count
      if ((TRUE == gw_cb_service_ptr->selected) &&
          (FALSE == cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.selected))
      {
        appInfo_ptr->bc.gw.gw_cb_num_selected--; // Not used by modem api
      }
      else if ((FALSE == gw_cb_service_ptr->selected) &&
               (TRUE == cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.selected))
      {
        appInfo_ptr->bc.gw.gw_cb_num_selected++; // Not used by modem api
      }

      // Set up the NV item to write
      memset(&lnvi, 0, sizeof(lnvi));

      lnvi.sms_gw_cb_service_table.index          = (byte)tbl_index;
      lnvi.sms_gw_cb_service_table.active_service = TRUE;
      lnvi.sms_gw_cb_service_table.service_from   = cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.srv_id.gw_cb_range.from;
      lnvi.sms_gw_cb_service_table.service_to     = cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.srv_id.gw_cb_range.to;
      lnvi.sms_gw_cb_service_table.selected       = cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.selected;
      (void)memscpy( (char *)lnvi.sms_gw_cb_service_table.label,
                     WMS_BC_MM_SRV_LABEL_SIZE,
                     cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.label,
                     WMS_BC_MM_SRV_LABEL_SIZE );
      lnvi.sms_gw_cb_service_table.label_encoding = (byte)cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.label_encoding;
      lnvi.sms_gw_cb_service_table.bc_alert       = (byte)cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.alert;
      lnvi.sms_gw_cb_service_table.max_messages   = cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.max_messages;

      // Update internal state
      {
        boolean gw_cb_enable_new = wms_bc_gw_cb_enable_check( appInfo_ptr->bc.gw.gw_cb_config,
                                                              appInfo_ptr->bc.gw.gw_cb_pref,
                                                              appInfo_ptr->bc.gw.gw_cb_num_selected );

        rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
        // Update local table cache
        *gw_cb_service_ptr = lnvi.sms_gw_cb_service_table;
        // Check to see if we need to enable BC to lower layers
        appInfo_ptr->bc.gw.gw_cb_enable = gw_cb_enable_new;
        rex_leave_crit_sect(&wms_bc_crit_sect);
      }

      // Update NV and SIM
      // Write to NV if needed
      if (TRUE == appInfo_ptr->bc.gw.in_nv[tbl_index])
      {
        nv_stat_enum_type    write_status;

        write_status = wms_nv_write_wait_per_subs( as_id, NV_SMS_GW_CB_SERVICE_TABLE_I, &lnvi );

        if (NV_DONE_S != write_status)
        {
          MSG_ERROR_1("GW CB Table NV write failed: %d", write_status);
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
          break;
        }
      }

      // Write to sim if needed
      if (TRUE == appInfo_ptr->bc.gw.in_sim[tbl_index])
      {
        if (FALSE == wms_bc_update_gw_cb_table_to_sim(as_id))
        {
          cmd_err = WMS_CMD_ERR_BC_SIM_WRITE;
          break;
        }
      }

      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }

    } while(0);

    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      MSG_HIGH_1("GW CB enable=%d", appInfo_ptr->bc.gw.gw_cb_enable);

      wms_bc_gw_cb_enable_to_nas(appInfo_ptr);

      // Prep the event
      bc_mm_event_ptr->as_id        = cmd_ptr->hdr.as_id;
      bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_change_srv_info.message_mode;

      wms_bc_make_gw_cb_srv_info_event( gw_cb_service_ptr,
                                        &bc_mm_event_ptr->u.bc_mm_srv_info );

      // command is Ok, send the event to client
      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SRV_UPDATED, bc_mm_event_ptr);
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_SRV_UPDATED event not sent: error %d", cmd_err);
    }

    WMS_MEM_FREEIF(bc_mm_event_ptr);

#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    wms_bc_mm_event_info_s_type       *bc_mm_event_ptr = NULL;
    wms_cmd_err_e_type                 cmd_err         = WMS_CMD_ERR_NONE;
    nv_sms_bc_service_table_type      *service_ptr     = NULL;
    uint32                             tbl_index       = 0;

    do
    {
      nv_item_type                    lnvi;

      // Check if the entry can be changed
      if (0 < wms_bc_mm_get_presidential_alert_entry_count_in_client_req( WMS_MESSAGE_MODE_CDMA,
                                                                          &cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.srv_id))
      {
        MSG_HIGH_0( "BC MM Change SRV INFO: change not allowed");
        cmd_err = WMS_CMD_ERR_BC_BAD_SRV_ID;
        break;
      }

      // Find the service table entry
      {
        boolean                         found = FALSE;

        // Find the entry matching service and language
        for (tbl_index = 0; tbl_index < appInfo_ptr->bc.cd.bc_table_size; tbl_index++ )
        {
          service_ptr = &appInfo_ptr->bc.cd.bc_table[tbl_index];  // Used by modem api

          if ( (TRUE == service_ptr->active_service) &&
               ((word)cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.srv_id.bc_srv_id.service == service_ptr->service) &&
               ((byte)cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.srv_id.bc_srv_id.language == service_ptr->language) )
          {
            found = TRUE;
            MSG_HIGH_1("BC SEL: found entry %d", tbl_index);
            break;
          }
        }

        if (!found)
        {
          MSG_HIGH_0("BC MM CHange SRV INFO: entry not found");
          cmd_err = WMS_CMD_ERR_BC_BAD_SRV_ID;
          break;
        }
      }

      // Update the service entry label
      (void)memscpy( (char *)service_ptr->label,  // Not used by modem api
                     WMS_BC_MM_SRV_LABEL_SIZE,
                     cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.label,
                     WMS_BC_MM_SRV_LABEL_SIZE );

      // Set up the NV item to write
      memset(&lnvi, 0, sizeof(lnvi));

      lnvi.sms_bc_service_table = *service_ptr;

      MSG_HIGH_2( "Update service table entry %d with index = %d",
                  tbl_index,
                  service_ptr->index );

      // Write the NV item
      {
        nv_stat_enum_type    write_status;

        write_status = wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, &lnvi );

        if (NV_DONE_S != write_status)
        {
          MSG_ERROR_1("BC Table NV write failed: %d", write_status);
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
          break;
        }
      }

      // Allocate the event structure
      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }

    } while(0);

    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      nv_sms_bc_service_table_options_type  *opt_ptr;

      // Prep the event
      // Prep event
      bc_mm_event_ptr->as_id                                      = cmd_ptr->hdr.as_id;
      bc_mm_event_ptr->message_mode                               = cmd_ptr->cmd.bc_mm_change_srv_info.message_mode;

      bc_mm_event_ptr->u.bc_mm_srv_info.srv_id.bc_srv_id.service  = (wms_service_e_type) service_ptr->service;
      bc_mm_event_ptr->u.bc_mm_srv_info.srv_id.bc_srv_id.language = (wms_language_e_type) service_ptr->language;
      bc_mm_event_ptr->u.bc_mm_srv_info.selected                  = service_ptr->selected;
      bc_mm_event_ptr->u.bc_mm_srv_info.priority                  = (wms_priority_e_type) service_ptr->priority;
      (void)memscpy( bc_mm_event_ptr->u.bc_mm_srv_info.label,
                     WMS_BC_MM_SRV_LABEL_SIZE,
                     (char *) service_ptr->label,
                     WMS_BC_MM_SRV_LABEL_SIZE );

      opt_ptr = &appInfo_ptr->bc.cd.bc_table_options[tbl_index];

      bc_mm_event_ptr->u.bc_mm_srv_info.label_encoding = (wms_user_data_encoding_e_type) opt_ptr->label_encoding;
      bc_mm_event_ptr->u.bc_mm_srv_info.alert          = (wms_bc_alert_e_type) opt_ptr->bc_alert;
      bc_mm_event_ptr->u.bc_mm_srv_info.max_messages   = opt_ptr->max_messages;

      // Send the event to clients
      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SRV_UPDATED, bc_mm_event_ptr );
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_SRV_UPDATED event not sent: error %d", cmd_err);
    }

    WMS_MEM_FREEIF(bc_mm_event_ptr);

#endif /* FEATURE_CDSMS_BROADCAST */
  }

  return;
} /* wms_bc_mm_change_srv_info() */

/*===========================================================================
FUNCTION wms_bc_mm_delete_all_services_proc()                      (INTERNAL)

DESCRIPTION
  This function is used to delete all entries from the broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_delete_all_services_proc(
  wms_cmd_type  *cmd_ptr
)
{
  uint32                    tbl_index;
  nv_item_type              nvi;
  wms_cmd_err_e_type        cmd_err     = WMS_CMD_ERR_NONE;
  wms_bc_mm_srv_id_type     srv_id[2];
  wms_prov_app_info_s_type *appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  MSG_HIGH_3( "wms_cmd_bc_mm_delete_all_services_proc(client %d, asid %d, msg mode = %d)",
              cmd_ptr->hdr.client_id,
              cmd_ptr->hdr.as_id,
              cmd_ptr->cmd.bc_mm_delete_all_srvs.message_mode );

  if (NULL == appInfo_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
    return;
  }

  if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_delete_all_srvs.message_mode)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    wms_bc_mm_event_info_s_type       *bc_mm_event_ptr      = NULL;
    nv_sms_gw_cb_service_table_type   *gw_cb_service_ptr    = NULL;
    sys_modem_as_id_e_type             as_id                = cmd_ptr->hdr.as_id;

    do
    {
      // Delete all entries from table
      for (tbl_index = 0; tbl_index < appInfo_ptr->bc.gw.gw_cb_table_size; tbl_index++)
      {
        boolean                            ok_to_delete  = TRUE;
        int                                pres_entry_count_in_req;
        gw_cb_service_ptr  = &appInfo_ptr->bc.gw.gw_cb_table[tbl_index];

        srv_id[0].gw_cb_range.from = gw_cb_service_ptr->service_from;
        srv_id[0].gw_cb_range.to   = gw_cb_service_ptr->service_to;

        if (gw_cb_service_ptr->active_service)
        {
          // Check to see if there is an 'unsettable' service ID in this entry
          // An unsettable item is the Presidential Alert (0x1000 CDMA, 4370, 4383 GW)
          pres_entry_count_in_req = wms_bc_mm_get_presidential_alert_entry_count_in_client_req(WMS_MESSAGE_MODE_GW, srv_id);
          
          if( pres_entry_count_in_req > 0 )
          {
            // It is OK to delete the specified entry that includes presidential alert message id
            // if we have other presidential alert entries in the service table
            ok_to_delete = wms_bc_mm_is_delete_of_pres_entry_possible( appInfo_ptr, WMS_MESSAGE_MODE_GW, srv_id, pres_entry_count_in_req, gw_cb_service_ptr);
          }


          if (ok_to_delete)
          {
            // This entry can be deleted.
            // The entry is set to 'inactive' and 'unselected'

            rex_enter_crit_sect(&wms_bc_crit_sect);      // Exclusive write

            gw_cb_service_ptr->active_service = FALSE;   // Used by modem api
            appInfo_ptr->bc.gw.gw_cb_num_active--;      // Not used by modem api

            if (TRUE == gw_cb_service_ptr->selected)
            {
              appInfo_ptr->bc.gw.gw_cb_num_selected--;  // Not used by modem api
              gw_cb_service_ptr->selected = FALSE;       // Used by modem api
            }

            rex_leave_crit_sect(&wms_bc_crit_sect);
          }

          if (TRUE == appInfo_ptr->bc.gw.in_nv[tbl_index])
          {
            nv_stat_enum_type  write_status;

            // Set up NV item to write
            nvi.sms_gw_cb_service_table = *gw_cb_service_ptr;

            write_status = wms_nv_write_wait_per_subs( as_id, NV_SMS_GW_CB_SERVICE_TABLE_I, &nvi );

            if (NV_DONE_S != write_status)
            {
              MSG_ERROR_1("BC Table NV write failed: error %d", write_status);
              cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
              // A failure to write the NV does not stop the loop
            }
            
            nvi.sms_gw_cb_service_table_size = appInfo_ptr->bc.gw.gw_cb_table_size;
            if( wms_nv_write_wait_per_subs( appInfo_ptr->asid, NV_SMS_GW_CB_SERVICE_TABLE_SIZE_I, &nvi ) != NV_DONE_S )
            {
              MSG_ERROR_0("BC Table Size NV write failed");
              cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
            }
          }
        } // if (active service)
      } // for (each service)

      if (WMS_CMD_ERR_NONE != cmd_err)
      {
        break;
      }

      if (!wms_bc_update_gw_cb_table_to_sim(as_id))
      {
        // Couldn't update sim
        cmd_err = WMS_CMD_ERR_BC_SIM_WRITE;
        break;
      }

      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }

    } while(0);

    /* Send command status
    */
    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      boolean gw_cb_enable_new;

      // Delete all was successful
      MSG_HIGH_0("GW CB Table Delete All OK");

      gw_cb_enable_new = wms_bc_gw_cb_enable_check( appInfo_ptr->bc.gw.gw_cb_config,
                                                    appInfo_ptr->bc.gw.gw_cb_pref,
                                                    appInfo_ptr->bc.gw.gw_cb_num_selected );

      rex_enter_crit_sect(&wms_bc_crit_sect);                     // Exclusive write
      appInfo_ptr->bc.gw.gw_cb_enable = gw_cb_enable_new;        // Used by modem api
      rex_leave_crit_sect(&wms_bc_crit_sect);

      MSG_HIGH_1("GW CB enable=%d", appInfo_ptr->bc.gw.gw_cb_enable);
      wms_bc_gw_cb_enable_to_nas(appInfo_ptr);

      // Prep the event
      bc_mm_event_ptr->as_id        = cmd_ptr->hdr.as_id;
      bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_delete_all_srvs.message_mode;

      (void)wms_bc_mm_make_srv_ids_event( appInfo_ptr,
                                          WMS_MESSAGE_MODE_GW,
                                          &bc_mm_event_ptr->u.bc_mm_srv_ids);

      // Send the event to client
      wms_bc_mm_event_notify(WMS_BC_MM_EVENT_DELETE_ALL_SRVS, bc_mm_event_ptr);

      WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_srv_ids.entries);
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_DELETE_ALL_SRVS event not sent: error %d", cmd_err);
    }

    WMS_MEM_FREEIF(bc_mm_event_ptr);

#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    wms_bc_mm_event_info_s_type *bc_mm_event_ptr = NULL;

    do
    {
      nv_sms_bc_service_table_type   *service_ptr = NULL;

      // Delete all entries from table
      for (tbl_index = 0; tbl_index < appInfo_ptr->bc.cd.bc_table_size; tbl_index++)
      {
        service_ptr = &appInfo_ptr->bc.cd.bc_table[tbl_index];

        srv_id[0].bc_srv_id.service = (wms_service_e_type)service_ptr->service;

        // If this table entry is active, delete it
        if ((TRUE == service_ptr->active_service) &&
            (0 == wms_bc_mm_get_presidential_alert_entry_count_in_client_req(WMS_MESSAGE_MODE_CDMA, srv_id)))
        {
          // De-activate this entry
          rex_enter_crit_sect(&wms_bc_crit_sect);  // Exclusive write
          service_ptr->active_service = FALSE;     // Used by modem api
          appInfo_ptr->bc.cd.bc_num_active --;           // Not used by modem api

          if (service_ptr->selected)
          {
            appInfo_ptr->bc.cd.bc_num_selected --;       // Not used by modem api
            service_ptr->selected = FALSE;         // Used by modem api
          }
          rex_leave_crit_sect(&wms_bc_crit_sect);

          {
            nv_stat_enum_type  write_status;

            // Set up NV item to write
            nvi.sms_bc_service_table = *service_ptr;

            // Write to NV
            write_status = wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, &nvi );

            if (NV_DONE_S != write_status)
            {
              MSG_ERROR_1("BC Table NV write failed: error %d", write_status);
              cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
              // A failure to write the NV does not stop the loop
            }
          }
        } // active service
      } // for each service entry

      if (WMS_CMD_ERR_NONE != cmd_err)
      {
        break;
      }

      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }

    } while (0);


    // Send command status
    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      // Delete all was successful
      MSG_HIGH_0("BC Table Delete All OK");

      // Update CM with the current broadcast state
      wms_bc_update_1x_bcsms_setting(appInfo_ptr);

      bc_mm_event_ptr->as_id        = cmd_ptr->hdr.as_id;
      bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_delete_all_srvs.message_mode;

      // Send the event to client
      (void) wms_bc_mm_make_srv_ids_event( appInfo_ptr,
                                           WMS_MESSAGE_MODE_CDMA,
                                           &bc_mm_event_ptr->u.bc_mm_srv_ids);

      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_DELETE_ALL_SRVS, bc_mm_event_ptr );
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_DELETE_ALL_SRVS event not sent: error %d", cmd_err);
    }

    if (NULL != bc_mm_event_ptr)
    {
      WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_table.entries);
      wms_mem_free(bc_mm_event_ptr);
    }

#endif /* FEATURE_CDSMS_BROADCAST */
  }

  return;
} /* wms_bc_mm_delete_all_services_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_select_all_services_proc()                      (INTERNAL)

DESCRIPTION
  This function is used to select/deselect all entries from the
  broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
void wms_bc_mm_select_all_services_proc(
  wms_cmd_type  *cmd_ptr
)
{
  uint32                    tbl_index;
  nv_item_type              nvi;
  wms_cmd_err_e_type        cmd_err     = WMS_CMD_ERR_NONE;
  sys_modem_as_id_e_type    as_id       = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type *appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (NULL == appInfo_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
    return;
  }

  MSG_HIGH_1( "WMS_CMD_BC_MM_SELECT_ALL_SERVICES: msg mode = %d",
              cmd_ptr->cmd.bc_mm_select_all_srvs.message_mode );

  if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_select_all_srvs.message_mode)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    wms_bc_mm_event_info_s_type       *bc_mm_event_ptr = NULL;

    do
    {
      nv_sms_gw_cb_service_table_type   *gw_cb_service_ptr = NULL;

      // Select all entries from table
      for (tbl_index = 0; tbl_index < appInfo_ptr->bc.gw.gw_cb_table_size; tbl_index++)
      {
        gw_cb_service_ptr = &appInfo_ptr->bc.gw.gw_cb_table[tbl_index];

        if (gw_cb_service_ptr->active_service)
        {
          rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write

          if ((gw_cb_service_ptr->selected) &&
              (!cmd_ptr->cmd.bc_mm_select_all_srvs.selected))
          {
            // Currently selected becoming not selected
            appInfo_ptr->bc.gw.gw_cb_num_selected --;      // Not used by modem api
            gw_cb_service_ptr->selected = FALSE;            // Used by modem api
          }
          else if ((!gw_cb_service_ptr->selected) &&
                   (cmd_ptr->cmd.bc_mm_select_all_srvs.selected))
          {
            // Currently not selected becoming selected
            appInfo_ptr->bc.gw.gw_cb_num_selected ++;      // Not used by modem api
            gw_cb_service_ptr->selected = TRUE;             // Used by modem api
          }

          rex_leave_crit_sect(&wms_bc_crit_sect);

          // Write to NV
          {
            nv_stat_enum_type write_status;

            nvi.sms_gw_cb_service_table = *gw_cb_service_ptr;

            write_status = wms_nv_write_wait_per_subs(as_id, NV_SMS_GW_CB_SERVICE_TABLE_I, &nvi);

            if (NV_DONE_S != write_status)
            {
              MSG_ERROR_0("GW CB Table NV write failed");
              cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
              // A failure to write the NV does not stop the loop
            }
          }
        } // active service
      } // for each service entry

      if (WMS_CMD_ERR_NONE != cmd_err)
      {
        break;
      }

      // Update SIM
      if (!wms_bc_update_gw_cb_table_to_sim(as_id))
      {
        cmd_err = WMS_CMD_ERR_BC_SIM_WRITE;
      }

      // Allocate the event and populate the service table
      {
        boolean                      table_ok;

        bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

        if (NULL == bc_mm_event_ptr)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
          break;
        }

        table_ok = wms_bc_mm_make_table_event( appInfo_ptr,
                                               WMS_MESSAGE_MODE_GW,
                                               &bc_mm_event_ptr->u.bc_mm_table );
        if (!table_ok)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
          break;
        }
      }

    } while(0);

    // Send command status
    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_BC_SIM_WRITE == cmd_err)
    {
      // Ignore SIM write failure and send event to client anyway
      cmd_err = WMS_CMD_ERR_NONE;
    }

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
        boolean gw_cb_enable_new;

        gw_cb_enable_new = wms_bc_gw_cb_enable_check( appInfo_ptr->bc.gw.gw_cb_config,
                                                      appInfo_ptr->bc.gw.gw_cb_pref,
                                                      appInfo_ptr->bc.gw.gw_cb_num_selected );

        rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
        appInfo_ptr->bc.gw.gw_cb_enable = gw_cb_enable_new;   // Used by modem api
        rex_leave_crit_sect(&wms_bc_crit_sect);

        MSG_HIGH_1("GW CB enable=%d", appInfo_ptr->bc.gw.gw_cb_enable);
        wms_bc_gw_cb_enable_to_nas(appInfo_ptr);

        // Prep the event (service table is already done)
        bc_mm_event_ptr->as_id        = cmd_ptr->hdr.as_id;
        bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_select_all_srvs.message_mode;

        // Send the event to client
        wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SELECT_ALL_SRVS, bc_mm_event_ptr);
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_SELECT_ALL_SRVS event not sent: error %d", cmd_err);
    }

    if (NULL != bc_mm_event_ptr)
    {
      WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_table.entries);
      wms_mem_free(bc_mm_event_ptr);
    }

#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    wms_bc_mm_event_info_s_type       *bc_mm_event_ptr = NULL;

    do
    {
      nv_sms_bc_service_table_type   *service_ptr = NULL;

      // Update all entries in the table
      for (tbl_index = 0; tbl_index < appInfo_ptr->bc.cd.bc_table_size; tbl_index++)
      {
        service_ptr = &appInfo_ptr->bc.cd.bc_table[tbl_index];

        if (service_ptr->active_service)
        {
          rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write

          if ((service_ptr->selected) &&
              (!cmd_ptr->cmd.bc_mm_select_all_srvs.selected))
          {
            // Currently not selected becoming selected
            appInfo_ptr->bc.cd.bc_num_selected --;     // Not used by modem api
            service_ptr->selected = FALSE;       // Used by modem api
          }
          else if ((!service_ptr->selected) &&
                   ( cmd_ptr->cmd.bc_mm_select_all_srvs.selected))
          {
            // Currently selected becoming not selected
            appInfo_ptr->bc.cd.bc_num_selected ++;     // Not used by modem api
            service_ptr->selected = TRUE;        // Used by modem api
          }

          rex_leave_crit_sect(&wms_bc_crit_sect);

          // Write to NV
          {
            nv_stat_enum_type write_status;

            nvi.sms_bc_service_table = *service_ptr;

            MSG_HIGH_2( "Update service table entry %d with index = %d",
                        tbl_index,
                        service_ptr->index );

            write_status = wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, &nvi );

            if (NV_DONE_S != write_status)
            {
              MSG_ERROR_0("BC Table NV write failed");
              cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
              // A failure to write the NV does not stop the loop
            }
          }
        } // active service
      } // for each service entry

      // Allocate the event and populate the service table
      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }

      // Allocate the event and populate the service table
      {
        boolean                      table_ok;

        table_ok = wms_bc_mm_make_table_event( appInfo_ptr,
                                               WMS_MESSAGE_MODE_CDMA,
                                               &bc_mm_event_ptr->u.bc_mm_table );
        if (!table_ok)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
          break;
        }
      }

    } while(0);

    // Send command status
    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      // Update CM with the current broadcast state
      wms_bc_update_1x_bcsms_setting(appInfo_ptr);

      // Prep the event (the service table is already populated)
      bc_mm_event_ptr->as_id        = cmd_ptr->hdr.as_id;
      bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_select_all_srvs.message_mode;

      // Send the event to client
      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SELECT_ALL_SRVS, bc_mm_event_ptr);
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_SELECT_ALL_SRVS event not sent: error %d", cmd_err);
    }

    if (NULL != bc_mm_event_ptr)
    {
      WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_table.entries);
      wms_mem_free(bc_mm_event_ptr);
    }

#endif /* FEATURE_CDSMS_BROADCAST */
  }

  return;
} /* wms_bc_mm_select_all_services_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_set_priority_for_all_services_proc()            (INTERNAL)

DESCRIPTION
  This function is used to set the priorities for all entries from the
  broadcast service table. (Specific to CDMA, Does not apply to GW)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients
=========================================================================*/
void wms_bc_mm_set_priority_all_services_proc(
  wms_cmd_type  *cmd_ptr
)
{
  MSG_HIGH_0("WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES");

  if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_set_priority_all_srvs.message_mode)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    wms_cmd_err_e_type                 cmd_err         = WMS_CMD_ERR_NONE;
    wms_bc_mm_event_info_s_type       *bc_mm_event_ptr = NULL;

    do
    {
      uint32                          tbl_index;
      nv_sms_bc_service_table_type   *service_ptr = NULL;
      wms_prov_app_info_s_type       *appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

      // Update all entries in the table
      for (tbl_index = 0; tbl_index < appInfo_ptr->bc.cd.bc_table_size; tbl_index++)
      {
        service_ptr = &appInfo_ptr->bc.cd.bc_table[tbl_index];

        // If this table entry is active, update priority if setting differs from the entry
        if ( (service_ptr->active_service) &&
             ((byte)cmd_ptr->cmd.bc_mm_set_priority_all_srvs.priority != service_ptr->priority) )
        {
          rex_enter_crit_sect(&wms_bc_crit_sect);  // Exclusive write
          service_ptr->priority = (byte)cmd_ptr->cmd.bc_mm_set_priority_all_srvs.priority; // Used by modem api
          rex_leave_crit_sect(&wms_bc_crit_sect);

          // Write to NV
          {
            nv_stat_enum_type               write_status;
            nv_item_type                    nvi;

            nvi.sms_bc_service_table = *service_ptr;

            MSG_HIGH_2( "Update service table entry i with index = %d",
                        tbl_index,
                        service_ptr->index );

            write_status = wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, &nvi );

            if (NV_DONE_S != write_status)
            {
              MSG_ERROR_1("BC Table NV write failed: error %d", write_status);
              cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
              // A failure to write the NV does not stop the loop
            }
          }
        } // active service
      } // for each service entry

      // Allocate the event and populate the service table
      bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

      if (NULL == bc_mm_event_ptr)
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        break;
      }

      // Populate the service table
      {
        boolean                      table_ok;

        table_ok = wms_bc_mm_make_table_event( appInfo_ptr,
                                               WMS_MESSAGE_MODE_CDMA,
                                               &bc_mm_event_ptr->u.bc_mm_table );
        if (!table_ok)
        {
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
          break;
        }
      }

    } while (0);

    // Send command status
    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
        bc_mm_event_ptr->as_id        = cmd_ptr->hdr.as_id;
        bc_mm_event_ptr->message_mode = cmd_ptr->cmd.bc_mm_set_priority_all_srvs.message_mode;

        // Send the event to client
        wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS, bc_mm_event_ptr);
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS event not sent: error %d", cmd_err);
    }

    if (NULL != bc_mm_event_ptr)
    {
      WMS_MEM_FREEIF(bc_mm_event_ptr->u.bc_mm_table.entries);
      wms_mem_free(bc_mm_event_ptr);
    }

#endif /* FEATURE_CDSMS_BROADCAST */
  }

  return;
} /* wms_bc_mm_set_priority_all_services_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_msg_delete_indication_proc()                    (INTERNAL)

DESCRIPTION
  This function is used to notify the lower layer of Message Deletion.
  This is applicable to GW mode only and is used by the WMS Clients to
  indicate the deletion of a GW CB page.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_bc_mm_msg_delete_indication_proc(
  wms_cmd_type  *cmd_ptr
)
{
#ifdef FEATURE_GWSMS_BROADCAST
  sys_modem_as_id_e_type    as_id = cmd_ptr->hdr.as_id;
  wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_NONE;

  MSG_HIGH_0("Sending to CB: Delete Page Request");
  cmd_err = wms_cm_gw_cb_delete_message(as_id,
                                        cmd_ptr->cmd.bc_mm_msg_delete_indication.page_hdr.cb_srv_id,
                                        cmd_ptr->cmd.bc_mm_msg_delete_indication.page_hdr.message_code,
                                        cmd_ptr->cmd.bc_mm_msg_delete_indication.page_hdr.page_number);

  wms_client_cmd_status(cmd_ptr, cmd_err);

#else /* FEATURE_GWSMS_BROADCAST */
  /* Send Command Status */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
#endif /* FEATURE_GWSMS_BROADCAST */

  return;
} /* wms_bc_mm_msg_delete_indication_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_msg_delete_all_indication_proc()                (INTERNAL)

DESCRIPTION
  This function is used to notify the lower layer of Message Deletion.
  This is applicable to GW mode only and is used by the WMS Client to
  indicate the deletion of ALL GW CB page.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_bc_mm_msg_delete_all_indication_proc(
  wms_cmd_type  *cmd_ptr
)
{
#ifdef FEATURE_GWSMS_BROADCAST
  sys_modem_as_id_e_type    as_id = cmd_ptr->hdr.as_id;
  wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_NONE;

  MSG_HIGH_0("Sending to CB: Delete ALL Page Request");
  cmd_err = wms_cm_gw_cb_delete_all_messages(as_id);

  wms_client_cmd_status(cmd_ptr, cmd_err);

#else /* FEATURE_GWSMS_BROADCAST */
  /* Send Command Status */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
#endif /* FEATURE_GWSMS_BROADCAST */

  return;
} /* wms_bc_mm_msg_delete_all_indication_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_get_reading_pref_proc()                         (INTERNAL)

DESCRIPTION
  This function is used to Get the Reading Preferences - Reading Advised
  and Reading Optional for UMTS CBS Messages
  This is applicable to GW mode only.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients
=========================================================================*/
void wms_bc_mm_get_reading_pref_proc(
  wms_cmd_type  *cmd_ptr
)
{
  MSG_HIGH_0("WMS_CMD_BC_MM_GET_READING_PREF");

  if (WMS_MESSAGE_MODE_CDMA == cmd_ptr->cmd.bc_mm_get_reading_pref.message_mode)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  }
  else if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_get_reading_pref.message_mode)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    wms_prov_app_info_s_type     *appInfo_ptr     = cmd_ptr->hdr.appInfo_ptr;
    wms_cmd_err_e_type            cmd_err         = WMS_CMD_ERR_NONE;
    wms_bc_mm_event_info_s_type  *bc_mm_event_ptr = NULL;

    if (NULL == appInfo_ptr)
    {
      wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
      return;
    }

    bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

    if (NULL == bc_mm_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
    }

    // Send command status
    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      // Prep event
      bc_mm_event_ptr->as_id                                 = cmd_ptr->hdr.as_id;
      bc_mm_event_ptr->message_mode                          = cmd_ptr->cmd.bc_mm_get_reading_pref.message_mode;
      bc_mm_event_ptr->u.bc_mm_reading_pref.reading_advised  = appInfo_ptr->bc.gw.reading_advised;
      bc_mm_event_ptr->u.bc_mm_reading_pref.reading_optional = appInfo_ptr->bc.gw.reading_optional;

      // Send event to clients
      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_READING_PREF, bc_mm_event_ptr);
    }
    else
    {
      MSG_ERROR_1( "WMS_BC_MM_EVENT_READING_PREF event not sent: error %d", cmd_err);
    }

    WMS_MEM_FREEIF(bc_mm_event_ptr);

#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
    MSG_ERROR_0("Invalid Message Mode for BC MM Get Reading Pref");
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  }

  return;
} /* wms_bc_mm_get_reading_pref_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_set_reading_pref_proc()                         (INTERNAL)

DESCRIPTION
  This function is used to Set the Reading Preferences - Reading Advised
  and Reading Optional for UMTS CBS Messages
  This is applicable to GW mode only.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients
=========================================================================*/
void wms_bc_mm_set_reading_pref_proc(
  wms_cmd_type  *cmd_ptr
)
{
  MSG_HIGH_0("WMS_CMD_BC_MM_SET_READING_PREF");

  if (WMS_MESSAGE_MODE_CDMA == cmd_ptr->cmd.bc_mm_set_reading_pref.message_mode)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  else if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.bc_mm_set_reading_pref.message_mode)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    boolean                       notify_nas      = FALSE;
    wms_cmd_err_e_type            cmd_err         = WMS_CMD_ERR_NONE;
    sys_modem_as_id_e_type        as_id           = cmd_ptr->hdr.as_id;
    wms_prov_app_info_s_type     *appInfo_ptr     = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);
    wms_bc_mm_event_info_s_type  *bc_mm_event_ptr = NULL;

    if (NULL == appInfo_ptr)
    {
      wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
      return;
    }

    bc_mm_event_ptr = (wms_bc_mm_event_info_s_type *)wms_mem_malloc(sizeof(wms_bc_mm_event_info_s_type));

    if (NULL == bc_mm_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
    }

    // Send command status
    wms_client_cmd_status( cmd_ptr, cmd_err );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      if (appInfo_ptr->bc.gw.reading_advised != cmd_ptr->cmd.bc_mm_set_reading_pref.reading_advised)
      {
        appInfo_ptr->bc.gw.reading_advised = cmd_ptr->cmd.bc_mm_set_reading_pref.reading_advised; // Not used by modem api
        notify_nas = TRUE;
      }

      if (appInfo_ptr->bc.gw.reading_optional != cmd_ptr->cmd.bc_mm_set_reading_pref.reading_optional)
      {
        appInfo_ptr->bc.gw.reading_optional = cmd_ptr->cmd.bc_mm_set_reading_pref.reading_optional; // Not used by modem api
        notify_nas = TRUE;
      }

      // Notify NAS if Reading Pref has Changed
      if (notify_nas)
      {
        MSG_HIGH_2( "Reading Pref Changed Advised=%d Optional=%d",
                    appInfo_ptr->bc.gw.reading_advised,
                    appInfo_ptr->bc.gw.reading_optional );

#ifdef FEATURE_UMTS_BMC
        // Store Changes to NV
        {
          nv_item_type nvi;

          nvi.sms_bmc_reading_pref.reading_advised  = appInfo_ptr->bc.gw.reading_advised;
          nvi.sms_bmc_reading_pref.reading_optional = appInfo_ptr->bc.gw.reading_optional;

          (void) wms_nv_write_wait_per_subs(as_id, NV_SMS_BMC_READING_PREF_I, &nvi );
        }
#endif /* FEATURE_UMTS_BMC */

        wms_bc_gw_cb_enable_to_nas(appInfo_ptr);
      }

      // Prep the event
      bc_mm_event_ptr->as_id                                 = cmd_ptr->hdr.as_id;
      bc_mm_event_ptr->message_mode                          = cmd_ptr->cmd.bc_mm_set_reading_pref.message_mode;
      bc_mm_event_ptr->u.bc_mm_reading_pref.reading_advised  = appInfo_ptr->bc.gw.reading_advised;
      bc_mm_event_ptr->u.bc_mm_reading_pref.reading_optional = appInfo_ptr->bc.gw.reading_optional;

      // Send event to clients
      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_READING_PREF, bc_mm_event_ptr);
    }

    WMS_MEM_FREEIF(bc_mm_event_ptr);

#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
    MSG_ERROR_0("Invalid Message Mode for BC MM Set Reading Pref");
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  }

  return;
} /* wms_bc_mm_set_reading_pref_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_process_cmd()                                   (INTERNAL)

DESCRIPTION
  Dispatch the BC command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_bc_mm_process_cmd(
  wms_cmd_type  *cmd_ptr
)
{

  switch( cmd_ptr->hdr.cmd_id )
  {
    /* BC commands
    */
    case WMS_CMD_BC_MM_GET_CONFIG:
      wms_bc_mm_get_config_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_GET_PREF:
      wms_bc_mm_get_pref_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_SET_PREF:
      wms_bc_mm_set_pref_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_GET_TABLE:
      wms_bc_mm_get_table_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_SELECT_SRV:
      wms_bc_mm_select_srv_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_GET_ALL_SRV_IDS:
      wms_bc_mm_get_all_srv_ids_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_GET_SRV_INFO:
      wms_bc_mm_get_srv_info_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_ADD_SRV:
      wms_bc_mm_add_srv_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_DELETE_SRV:
      wms_bc_mm_delete_srv_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_CHANGE_SRV_INFO:
      wms_bc_mm_change_srv_info_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_DELETE_ALL_SERVICES:
      wms_bc_mm_delete_all_services_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_SELECT_ALL_SERVICES:
      wms_bc_mm_select_all_services_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES:
      wms_bc_mm_set_priority_all_services_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_MSG_DELETE_INDICATION:
      wms_bc_mm_msg_delete_indication_proc( cmd_ptr );
      break;

   case WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION:
      wms_bc_mm_msg_delete_all_indication_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_GET_READING_PREF:
      wms_bc_mm_get_reading_pref_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_SET_READING_PREF:
      wms_bc_mm_set_reading_pref_proc( cmd_ptr );
      break;

#ifdef FEATURE_CDSMS_BROADCAST
    case WMS_CMD_BC_SCPT_DATA:
      wms_bc_scpt_data_proc( cmd_ptr );
      break;
#endif /* FEATURE_CDSMS_BROADCAST */

    default:
      break;
  }

  return;
} /* wms_bc_mm_process_cmd() */


#ifdef FEATURE_GWSMS_BROADCAST

/*===========================================================================
FUNCTION wms_bc_gw_cb_init()                                       (INTERNAL)

DESCRIPTION
  Do BC-related initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  BC data initialized.
===========================================================================*/
void wms_bc_gw_cb_init(
  wms_prov_app_info_s_type     *appInfo_ptr,
  boolean                       do_sim_initialization
)
{
  uint32                           tbl_idx;
  nv_item_type                     nvi;
  wms_bc_config_e_type             local_gw_cb_config     = WMS_BC_CONFIG_DISALLOW;
  wms_bc_pref_e_type               local_gw_cb_pref       = WMS_BC_PREF_DEACTIVATE;
  uint16                           local_gw_cb_table_size = 0;
  nv_stat_enum_type                nv_status;

  if (NULL == appInfo_ptr)
  {
    MSG_ERROR_0("appInfo_ptr is NULL");
    return;
  }

  MSG_HIGH_2( "wms_bc_gw_cb_init() asid %d, init_sim %d",
              appInfo_ptr->asid,
              do_sim_initialization );

  /* --------------- read all GW CB SMS items from NV ---------------- */

  // Read NV_SMS_GW_CB_CONFIG_I: Carrier specified broadcast configuration
  nv_status = wms_nv_read_wait_per_subs(appInfo_ptr->asid, NV_SMS_GW_CB_CONFIG_I, &nvi);

  if (NV_DONE_S == nv_status)
  {
    local_gw_cb_config = (wms_bc_config_e_type)nvi.sms_gw_cb_config;
  }
  else
  {
    // If the item can't be read, use WMS_BC_CONFIG_DISALLOW
    local_gw_cb_config = WMS_BC_CONFIG_DISALLOW;

    MSG_HIGH_1("wms_bc_gw_cb_init(): couldn't read NV_SMS_GW_CB_CONFIG, error %d", nv_status);

    { // Initialize/update NV
      nvi.sms_gw_cb_config = (byte)WMS_BC_CONFIG_DISALLOW;

      nv_status = wms_nv_write_wait_per_subs(appInfo_ptr->asid, NV_SMS_GW_CB_CONFIG_I, &nvi);

      if (NV_DONE_S != nv_status)
      {
        MSG_ERROR_1("Error while writing GW_CB_CONFIG, %d", nv_status);
      }
    }
  }

  // Read NV_SMS_GW_CB_USER_PREF_I: User preference for broadcast configuration
  nv_status = wms_nv_read_wait_per_subs(appInfo_ptr->asid, NV_SMS_GW_CB_USER_PREF_I, &nvi);

  if (NV_DONE_S == nv_status)
  {
    local_gw_cb_pref = (wms_bc_pref_e_type)nvi.sms_gw_cb_user_pref;
  }
  else
  {
    // If the item can't be read, use WMS_BC_PREF_DEACTIVATE
    local_gw_cb_pref = WMS_BC_PREF_DEACTIVATE;

    MSG_HIGH_1("wms_bc_gw_cb_init(): couldn't read NV_SMS_GW_CB_USER_PREF, error %d", nv_status);

    { // Initialize/update NV
      nvi.sms_gw_cb_user_pref = (byte)WMS_BC_PREF_DEACTIVATE;

      if (wms_nv_write_wait_per_subs(appInfo_ptr->asid, NV_SMS_GW_CB_USER_PREF_I, &nvi) != NV_DONE_S)
      {
        MSG_ERROR_1("Error while writing GW_CB_USER_PREF, %d", nv_status);
      }
    }
  }

  // Ensure that USER_PREF can't be higher than what is configured (CONFIG)
  if ( (WMS_BC_CONFIG_DISALLOW == local_gw_cb_config) &&
       (WMS_BC_PREF_DEACTIVATE != local_gw_cb_pref ) )
  {
    MSG_HIGH_0("GW CB SMS disabled in NV");

    local_gw_cb_pref = WMS_BC_PREF_DEACTIVATE;

    { // Initialize/update NV
      nvi.sms_gw_cb_user_pref   = (byte)WMS_BC_PREF_DEACTIVATE;

      nv_status = wms_nv_write_wait_per_subs(appInfo_ptr->asid, NV_SMS_GW_CB_USER_PREF_I, &nvi);

      if (NV_DONE_S != nv_status)
      {
        MSG_ERROR_1("Error while writing GW_CB_USER_PREF, %d", nv_status);
      }
    }
  }

  // Read NV_SMS_GW_CB_SERVICE_TABLE_SIZE_I: Size of the NV broadcast table
  nv_status = wms_nv_read_wait_per_subs(appInfo_ptr->asid, NV_SMS_GW_CB_SERVICE_TABLE_SIZE_I, &nvi);

  if (NV_DONE_S == nv_status)
  {
    local_gw_cb_table_size = nvi.sms_gw_cb_service_table_size;
  }
  else
  {
    // If the item can't be read, the table size is 0
    local_gw_cb_table_size = 0;

    MSG_HIGH_1("wms_bc_gw_cb_init(): couldn't read NV_SMS_GW_CB_SERVICE_TABLE_SIZE, error %d", nv_status);

    { // Initialize/update NV
      nvi.sms_gw_cb_service_table_size = 0;

      nv_status = wms_nv_write_wait_per_subs(appInfo_ptr->asid, NV_SMS_GW_CB_SERVICE_TABLE_SIZE_I, &nvi);

      if (NV_DONE_S != nv_status)
      {
        MSG_ERROR_1("Error while writing GW_CB_SERVICE_TABLE_SIZE, %d", nv_status);
      }
    }
  }

  MSG_HIGH_3( "wms_bc_gw_cb_init() config %d, pref %d, size %d",
              local_gw_cb_config, local_gw_cb_pref, local_gw_cb_table_size);

  // Read the NV-based broadcast table.
  // Entries are read one at a time and inserted into the table.
  // As each entry is added, track the number of active and selected entries.
  // The selected entry count is used to determine whether the table is effectively
  // enabled or disabled.  A table filled with unselected entries is considered
  // to have broadcast disabled.
  appInfo_ptr->bc.gw.gw_cb_num_active = 0;                           // Not used by modem api
  appInfo_ptr->bc.gw.gw_cb_num_selected = 0;                         // Not used by modem api

  for (tbl_idx = 0; tbl_idx < local_gw_cb_table_size && tbl_idx < WMS_BC_MM_TABLE_MAX; tbl_idx++)
  {
    nvi.sms_gw_cb_service_table.index = (byte)tbl_idx;

    nv_status = wms_nv_read_wait_per_subs(appInfo_ptr->asid, NV_SMS_GW_CB_SERVICE_TABLE_I, &nvi);

    if (NV_DONE_S != nv_status)
    {
      /* Cannot read this Entry in the Service Table */
      MSG_HIGH_2( "wms_bc_gw_cb_init() Could not read BC table entry at idx %d, error %d",
                  tbl_idx,
                  nv_status );

      rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
      appInfo_ptr->bc.gw.gw_cb_table[tbl_idx].active_service = FALSE;      // Used by modem api
      rex_leave_crit_sect(&wms_bc_crit_sect);
      continue;
    }

    MSG_HIGH_5( "Entry: idx %d, range %d-%d, active %d, selected %d",
                tbl_idx,
                nvi.sms_gw_cb_service_table.service_from,
                nvi.sms_gw_cb_service_table.service_to,
                nvi.sms_gw_cb_service_table.active_service,
                nvi.sms_gw_cb_service_table.selected );

    appInfo_ptr->bc.gw.in_nv[tbl_idx]  = TRUE;                             // Not used by modem api
    appInfo_ptr->bc.gw.in_sim[tbl_idx] = FALSE;                            // Not used by modem api

    if (TRUE == nvi.sms_gw_cb_service_table.active_service)
    {
      if (do_sim_initialization)
      {
        boolean                          is_in_sim;

        /* Search the SIM to see if this entry is present in SIM */
        is_in_sim = wms_bc_search_gw_cb_sim_list( appInfo_ptr,
                                                  nvi.sms_gw_cb_service_table.service_from,
                                                  nvi.sms_gw_cb_service_table.service_to );
        appInfo_ptr->bc.gw.in_sim[tbl_idx] = is_in_sim;                      // Not used by modem api

        /* Update NV table entry based on presence in SIM */
        if ((TRUE == is_in_sim) &&
            (FALSE == nvi.sms_gw_cb_service_table.selected))
        {
          nvi.sms_gw_cb_service_table.selected = TRUE;        // Used by modem api
        }
      } // do_sim_initialization

      /* Update num Active */
      appInfo_ptr->bc.gw.gw_cb_num_active ++;                        // Not used by modem api

      /* Update num_selected */
      if (TRUE == nvi.sms_gw_cb_service_table.selected)
      {
        appInfo_ptr->bc.gw.gw_cb_num_selected ++;                    // Not used by modem api
      }
    }

    rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
    appInfo_ptr->bc.gw.gw_cb_table[tbl_idx] = nvi.sms_gw_cb_service_table; // Used by modem api
    rex_leave_crit_sect(&wms_bc_crit_sect);
  } /* for loop */

  rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
  appInfo_ptr->bc.gw.gw_cb_config = local_gw_cb_config;              // Not used by modem api
  appInfo_ptr->bc.gw.gw_cb_pref = local_gw_cb_pref;                  // Used by modem api
  appInfo_ptr->bc.gw.gw_cb_table_size = local_gw_cb_table_size;      // Used by modem api
  rex_leave_crit_sect(&wms_bc_crit_sect);

  /* Initialize the UMTS BMC Reading Preferences */
  appInfo_ptr->bc.gw.reading_advised  = FALSE;  // Not used by modem api
  appInfo_ptr->bc.gw.reading_optional = FALSE;  // Not used by modem api

#ifdef FEATURE_UMTS_BMC
  // Read NV_SMS_BMC_READING_PREF:
  nv_status = wms_nv_read_wait_per_subs(appInfo_ptr->asid, NV_SMS_BMC_READING_PREF_I, &nvi);

  if (NV_DONE_S == nv_status)
  {
    appInfo_ptr->bc.gw.reading_advised  = nvi.sms_bmc_reading_pref.reading_advised;   // Not used by modem api
    appInfo_ptr->bc.gw.reading_optional = nvi.sms_bmc_reading_pref.reading_optional;  // Not used by modem api
  }
  else
  {
    MSG_HIGH_1("wms_bc_gw_cb_init(): couldn't read NV_SMS_BMC_READING_PREF, error %d", nv_status);

    { // Initialize/update NV
      nvi.sms_bmc_reading_pref.reading_optional = FALSE;
      nvi.sms_bmc_reading_pref.reading_optional = FALSE;

      nv_status = wms_nv_write_wait_per_subs(appInfo_ptr->asid, NV_SMS_BMC_READING_PREF_I, &nvi);
      if (NV_DONE_S != nv_status)
      {
        MSG_ERROR_1("Error while writing NV_SMS_BMC_READING_PREF, %d", nv_status);
      }
    }
  }
#endif /* FEATURE_UMTS_BMC */

  // Read the SIM CBMI and CBMIR records and add to the broadcast table
  if (do_sim_initialization)
  {
    wms_gw_cb_srv_range_s_type       range;
    boolean                          found_free;
    boolean                          found_dup;
    nv_sms_gw_cb_service_table_type *gw_cb_service_ptr = NULL;
    wms_bc_mm_service_info_s_type    srv_info;
    uint32                           lst_idx;
    uint32                           found_idx;

    // Update the table based on CBMI from the SIM
    for (lst_idx = 0; lst_idx < appInfo_ptr->bc.gw.sim_cbmi_list_size; lst_idx++)
    {
      range.to = appInfo_ptr->bc.gw.sim_cbmi_list[lst_idx];
      range.from = range.to;

      wms_bc_search_gw_cb_table_entry( appInfo_ptr,
                                       &range,
                                       &found_free,
                                       &found_dup,
                                       &found_idx,
                                       &gw_cb_service_ptr );

      if (!found_dup)
      {
        wms_cmd_err_e_type               ret_val;

        // This CBMI entry is not in the table; add and select it in table
        srv_info.srv_id.gw_cb_range.from = appInfo_ptr->bc.gw.sim_cbmi_list[lst_idx];
        srv_info.srv_id.gw_cb_range.to   = appInfo_ptr->bc.gw.sim_cbmi_list[lst_idx];
        srv_info.selected                = TRUE;
        srv_info.priority                = WMS_PRIORITY_NORMAL;
        srv_info.label[0]                = 0;
        srv_info.label_encoding          = WMS_ENCODING_GSM_7_BIT_DEFAULT;
        srv_info.alert                   = WMS_BC_ALERT_DEFAULT;
        srv_info.max_messages            = 20;

        ret_val = wms_bc_mm_add_single_srv( appInfo_ptr,
                                            WMS_MESSAGE_MODE_GW,
                                            &srv_info,
                                            FALSE);

        if (WMS_CMD_ERR_NONE != ret_val)
        {
          MSG_ERROR_1("Cannot add CBMI entry to NV table, err %d", ret_val);
        }
      }
    }

    // Update the table based on CBMIR from the SIM
    for (lst_idx = 0; lst_idx < appInfo_ptr->bc.gw.sim_cbmir_list_size; lst_idx++)
    {
      range = appInfo_ptr->bc.gw.sim_cbmir_list[lst_idx];

      wms_bc_search_gw_cb_table_entry( appInfo_ptr,
                                       &range,
                                       &found_free,
                                       &found_dup,
                                       &found_idx,
                                       &gw_cb_service_ptr );

      if (!found_dup)
      {
        wms_cmd_err_e_type               ret_val;

        // This CBMIR entry is not in the table; add and select it in table
        srv_info.srv_id.gw_cb_range = appInfo_ptr->bc.gw.sim_cbmir_list[lst_idx];
        srv_info.selected           = TRUE;
        srv_info.priority           = WMS_PRIORITY_NORMAL;
        srv_info.label[0]           = 0;
        srv_info.label_encoding     = WMS_ENCODING_GSM_7_BIT_DEFAULT;
        srv_info.alert              = WMS_BC_ALERT_DEFAULT;
        srv_info.max_messages       = 20;

        ret_val = wms_bc_mm_add_single_srv( appInfo_ptr,
                                            WMS_MESSAGE_MODE_GW,
                                            &srv_info,
                                            FALSE );

        if (WMS_CMD_ERR_NONE != ret_val)
        {
          MSG_ERROR_1("Cannot add CBMIR entry to NV table, err %d", ret_val);
        }
      }
    }
  } // do_sim_initialization for CBMI and CBMIR records

  /* ---- Finally, send GW CB enable command to lower layers ---- */
  {
    boolean gw_cb_enable_new = wms_bc_gw_cb_enable_check( appInfo_ptr->bc.gw.gw_cb_config,
                                                          appInfo_ptr->bc.gw.gw_cb_pref,
                                                          appInfo_ptr->bc.gw.gw_cb_num_selected );

    rex_enter_crit_sect(&wms_bc_crit_sect); // Exclusive write
    appInfo_ptr->bc.gw.gw_cb_enable = gw_cb_enable_new;   // Used by modem api
    rex_leave_crit_sect(&wms_bc_crit_sect);
  }

  MSG_HIGH_1("GW CB enable=%d", appInfo_ptr->bc.gw.gw_cb_enable);

  wms_bc_gw_cb_enable_to_nas(appInfo_ptr);

  return;
} /* wms_bc_gw_cb_init */

/*=========================================================================
FUNCTION wms_bc_gw_cb_filter                                (WMS_MODEM API)

DESCRIPTION
  This function checks the first block of the CB page
  to see if it passes the filter.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - the mobile should process the received message
  FALSE - the mobile should discard the received message

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_bc_gw_cb_filter(
  sys_modem_as_id_e_type  as_id,
  uint8                  *first_block
)
{
  boolean                       result      = FALSE;
  boolean                       msg_is_dup = FALSE;
  uint16                        lang_filter;
  uint32                        i;
  wms_gw_cb_page_header_s_type  page_header;
  wms_gw_cb_srv_id_type         srv_id;
  wms_prov_app_info_s_type     *appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);
#ifdef FEATURE_SMS_PWS_OVER_LTE
  wms_cfg_s_type	*cfg_s_ptr=NULL;
#endif
#ifndef FEATURE_NAS_CBS_DSDS
  as_id       = SYS_MODEM_AS_ID_1;
#endif /* FEATURE_NAS_CBS_DSDS */

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return FALSE;
  }

  /* Decode the page header first
  */
  if( wms_ts_decode_gw_cb_page_header(first_block, &page_header) == 0 )
  {
    MSG_ERROR_0("CB Page Header decoding failed");
    return FALSE; /* SHORT_RETURN */
  }

  MSG_HIGH_1("===== CB Page Header (as_id=%d): ====", as_id);
  MSG_HIGH_1("Msg ID=%ld", page_header.cb_srv_id);
  MSG_HIGH_1("Total pages: %d", page_header.total_pages);
  MSG_HIGH_1("Page number: %d", page_header.page_number);
  MSG_HIGH_1("Geo Scope: %d", page_header.geo_scope);
  MSG_HIGH_1("Msg Code: %d", page_header.message_code);
  MSG_HIGH_1("Update Number: %d", page_header.update_number);
  MSG_HIGH_1("Dcs.class=%d", page_header.cb_dcs.msg_class);
  MSG_HIGH_1("Dcs.alphabet=%d", page_header.cb_dcs.alphabet);
  MSG_HIGH_1("Dcs.language=%d", page_header.cb_dcs.language);
  if (WMS_GW_CB_LANGUAGE_ISO_639 == page_header.cb_dcs.language)
  {
    MSG_HIGH_1("Xlate ISO639 language=%s", page_header.cb_dcs.iso_639_lang);
    lang_filter = wms_sim_xlate_iso639_to_lang_filter( page_header.cb_dcs.iso_639_lang[0],
                                                       page_header.cb_dcs.iso_639_lang[1] ) ;
  }
  else
  {
    MSG_HIGH_1("Xlate DCS language = %d", page_header.cb_dcs.language);
    lang_filter = wms_sim_xlate_dcs_to_lang_filter(page_header.cb_dcs.language);
  }

  MSG_HIGH_1("Language Filter = 0x%04x", lang_filter);

  /* --------- Now do the filtering --------- */
  srv_id = page_header.cb_srv_id;

  rex_enter_crit_sect(&wms_bc_crit_sect); // MODEM_API: Ensure stable data is being read

  if (FALSE == appInfo_ptr->bc.gw.gw_cb_enable)
  {
    result = FALSE;
  }
  else
  {
    int num_languages_read = 0;

    if (WMS_BC_PREF_ACTIVATE_TABLE == appInfo_ptr->bc.gw.gw_cb_pref)
    {
      wms_cmd_err_e_type cmd_err;

      cmd_err = wms_sim_mmgsdi_read_lang_files( appInfo_ptr,
                                                appInfo_ptr->bc.gw.gw_cb_languages,
                                                ARRAY_SIZE(appInfo_ptr->bc.gw.gw_cb_languages),
                                                &num_languages_read );

      if (WMS_CMD_ERR_NONE != cmd_err)
      {
        MSG_HIGH_1("Could not read lang files: error=%d", cmd_err);
        num_languages_read = 0;
      }

      MSG_HIGH_1("Read %d languages", num_languages_read);
    }

    /* Now we need to check all table entries:
    */
    for( i=0; i < appInfo_ptr->bc.gw.gw_cb_table_size && i < WMS_BC_MM_TABLE_MAX; i++ )
    {
      if( (TRUE   == appInfo_ptr->bc.gw.gw_cb_table[i].active_service)  &&
          (TRUE   == appInfo_ptr->bc.gw.gw_cb_table[i].selected)        &&
          (srv_id >= appInfo_ptr->bc.gw.gw_cb_table[i].service_from)    &&
          (srv_id <= appInfo_ptr->bc.gw.gw_cb_table[i].service_to)
        )
      {
        if (WMS_BC_PREF_ACTIVATE_ALL == appInfo_ptr->bc.gw.gw_cb_pref)
        {
          /* Ignore language preferences */
          MSG_HIGH_0("WMS_BC_PREF_ACTIVATE_ALL: ignoring language preferences");
          result = TRUE;
        }
        else if (WMS_BC_PREF_ACTIVATE_TABLE == appInfo_ptr->bc.gw.gw_cb_pref)
        {
          // Check edge cases: CB message does not specify language, or no languages on card
          if ( (WMS_GW_CB_LANGUAGE_UNSPECIFIED == page_header.cb_dcs.language) ||
              (0 == num_languages_read) )
          {
            MSG_HIGH_0("WMS_BC_PREF_ACTIVATE_TABLE: lang unspecified or no langs read");

            result = TRUE;
          }
          else
          {
            int    lang_index;

            MSG_HIGH_0("WMS_BC_PREF_ACTIVATE_TABLE: checking lang entries");

            // Check language preferences
            for (lang_index = 0; lang_index < num_languages_read; lang_index++)
            {
              if (lang_filter == appInfo_ptr->bc.gw.gw_cb_languages[lang_index])
              {
                result = TRUE;
                break;  // for (lang_index = 0;
              }
            }
          }
        } // if (appInfo_ptr->bc.gw.gw_cb_pref == WMS_BC_PREF_ACTIVATE_TABLE)
        break; // for( i=0; i < appInfo_ptr->bc.gw.gw_cb_table_size
      } // if (cb_table entry matches
    }

    if (!result)
    {
      for (i = 0; i < appInfo_ptr->bc.gw.sim_cbmid_list_size; i++)
      {
        if (srv_id == appInfo_ptr->bc.gw.sim_cbmid_list[i])
        {
          result = TRUE;
        }
      }
    }
  }

  rex_leave_crit_sect(&wms_bc_crit_sect);


#ifdef FEATURE_SMS_PWS_OVER_LTE
if (result==TRUE)
 {
   cfg_s_ptr = wms_cfg_s_ptr();

   if (!cfg_s_ptr->lte_cb_dup_detection_is_disabled)
   {
	
	 if (WMS_GW_CB_GEO_SCOPE_PLMN == page_header.geo_scope &&
			   TRUE == wms_msg_gw_cb_msg_is_cmas(page_header.cb_srv_id))
		  {
			time_t					  search_time;
			lte_rrc_cmas_ind_s		  lte_cmas_rec;
			wms_cell_info_s_type	  curr_cell_info = wms_msg_gw_cb_get_cell_info_by_appInfo( appInfo_ptr );
			
			lte_cmas_rec.msg_id = (lte_rrc_msg_id_t) page_header.cb_srv_id;
			lte_cmas_rec.sr_num = page_header.raw_serial_number;
			
			lte_cmas_rec.plmn = curr_cell_info.selected_plmn;
			
			// Get timestamp of the duplicate search
			search_time = time_get_secs();
  
			// Check for duplicate message
			msg_is_dup = wms_bc_lte_message_is_duplicate( WMS_LTE_DUPLIST_CMAS,
														  (void *)(&lte_cmas_rec),
														  search_time,
														  curr_cell_info );
  
			MSG_HIGH_3( "CMASDupDet returns %d for msg_id 0x%04x with serial 0x%04x",
						msg_is_dup,
						lte_cmas_rec.msg_id,
						lte_cmas_rec.sr_num );
  
		  }
   }
}
#endif /* FEATURE_SMS_PWS_OVER_LTE */  


  

  if ((result == TRUE)&&(msg_is_dup == FALSE))
  {
    MSG_HIGH_0("GW CB hdr MATCHED and Not duplicate");
  }
  else
  {
    MSG_HIGH_0("GW CB hdr NO match or Duplicate");
    result = FALSE;
  }

  return result;
} /* wms_bc_gw_cb_filter() */

#endif /* FEATURE_GWSMS_BROADCAST */

#ifdef FEATURE_SMS_PWS_OVER_LTE

/*===========================================================================
FUNCTION wms_msg_get_expiration_from_plmn()                        (INTERNAL)

DESCRIPTION
  Inspects the plmn contents to determine the expiration time (in seconds)
  to be used for the entry.

  According to 23.041 v11.3 (23.041 b03) section 8.2, the expiration of a
  message shall be 24 hours unless the PLMN MCC is 440 or 441 (Japan) where
  the expiration shall be 1 hour.

  This function is used by the ETWS duplicate detection implementation
  to set the expiration time of a received message.

DEPENDENCIES
  None

RETURN VALUE
  time_t: The expiration time (in seconds)

SIDE EFFECTS
  None
===========================================================================*/
time_t wms_msg_get_expiration_from_plmn(
  lte_rrc_plmn_s   *plmn
)
{
  time_t   expiration_seconds = (WMS_BC_SECS_IN_HOUR * 24); // 24-hours of seconds

  if (NULL != plmn)
  {
    if ( (WMS_BC_MCC_0_JA_MASK == plmn->mcc[0])  &&
         (WMS_BC_MCC_1_JA_MASK == plmn->mcc[1])  &&
         ( (WMS_BC_MCC_2_JA_MASK1 == plmn->mcc[2])  || (WMS_BC_MCC_2_JA_MASK2 == plmn->mcc[2])) )
    {
      expiration_seconds = WMS_BC_SECS_IN_HOUR; // 1-hour of seconds
    }
  }

  return expiration_seconds;
} /* wms_msg_get_expiration_from_plmn() */

/*===========================================================================
FUNCTION boolean wms_bc_plmns_are_equal()                          (INTERNAL)

DESCRIPTION
  Compares two plmns to see if they are the same

DEPENDENCIES
  None

PARAMETERS
  lte_rrc_plmn_s     *plmn1     [IN]:
  lte_rrc_plmn_s     *plmn2     [IN]:

RETURNS
  boolean: TRUE; plmns are equal
           FALSE; plmns are not equal

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_bc_plmns_are_equal(
  lte_rrc_plmn_s *plmn1,
  lte_rrc_plmn_s *plmn2
)
{
  return (memcmp(plmn1, plmn2, sizeof(lte_rrc_plmn_s)) == 0);
} /* wms_bc_plmns_are_equal() */

/*===========================================================================
FUNCTION wms_bc_lte_bc_message_compare()                           (INTERNAL)

DESCRIPTION
  Compares two messages to determine if there is a duplicate.

DEPENDENCIES
  None

PARAMETERS
  wms_duplist_entry_type    *list_entry [IN]: An entry from the dup list
  wms_duplist_entry_type    *new_entry  [IN]: The message to be searched for

RETURNS
  boolean: TRUE; message is duplicate
           FALSE; message is not duplicate

SIDE EFFECTS
  None

===========================================================================*/
boolean wms_bc_lte_bc_message_compare(
  wms_duplist_entry_type    *list_entry,
  wms_duplist_entry_type    *new_entry
)
{
  boolean is_duplicate = FALSE;

  if (list_entry->serial_no == new_entry->serial_no)
  {
    MSG_HIGH_1("SER Match found: ser=%d", new_entry->serial_no);

    is_duplicate = TRUE;  // Message is a duplicate

    { // Optional additional checks using Geographical Scope
      int scope;

      scope = list_entry->serial_no >> 14; // Take the upper 2 bits

      /*
      |===========================================================|
      | GS Code  | Display Mode  | Geographical Scope             |
      |===========================================================|
      |   00     |   Immediate   | Cell wide                      |
      |-----------------------------------------------------------|
      |   01     |   Normal      | PLMN wide                      |
      |-----------------------------------------------------------|
      |          |               | Location Area wide in GSM,     |
      |   10     |   Normal      | Service Area wide in UMTS,     |
      |          |               | Tracking Area wide in E-UTRAN  |
      |-----------------------------------------------------------|
      |   11     |   Normal      | Cell wide                      |
      |===========================================================|
      */

      switch (scope)
      {
      case WMS_BC_GEO_SCOPE_HIGH_PRIORITY_CELL_WIDE:
      case WMS_BC_GEO_SCOPE_LOW_PRIORITY_CELL_WIDE:
        // Cell-wide
        if (!wms_bc_plmns_are_equal(&(list_entry->plmn), &(new_entry->plmn)) ||
            (list_entry->cell_id != new_entry->cell_id) ||
            (list_entry->tac != new_entry->tac))
        {
          MSG_HIGH_0("Cell-ids not equal");
          is_duplicate = FALSE;
        }
        break;
      case WMS_BC_GEO_SCOPE_PLMN_WIDE:
        // PLMN-wide
        if (!wms_bc_plmns_are_equal(&(list_entry->plmn), &(new_entry->plmn)))
        {
          MSG_HIGH_0("PLMNs are not equal");
          is_duplicate = FALSE;
        }
        break;
      case WMS_BC_GEO_SCOPE_TA_WIDE:
        // Location Area wide in GSM,
        // Service Area wide in UMTS,
        // Tracking Area wide in E-UTRAN
        if (!wms_bc_plmns_are_equal(&(list_entry->plmn), &(new_entry->plmn)) ||
            list_entry->tac != new_entry->tac)
        {
          MSG_HIGH_0("Tracking Area Codes are not equal");
          is_duplicate = FALSE;
        }
        break;
      } // switch (scope)
    } // Optional additional checks using Geographical Scope
  }

  MSG_HIGH_1("message_compare: is_duplicate=%d", is_duplicate);

  return is_duplicate;
} /* wms_bc_lte_bc_message_compare() */

/*===========================================================================
FUNCTION wms_bc_lte_message_is_duplicate()                         (INTERNAL)

DESCRIPTION
  Checks if a received ETWS/CMAS message is duplicate.

DEPENDENCIES
  None

PARAMETERS
  wms_duplist_ident_t      list_id      [IN]: Specifies primary or secondary list
  void                    *msg_ptr      [IN]: A pointer to the message to be searched for.
                                              The opaque pointer will reconstructed based on list_id
  time_t                   search_time  [IN]: Timestamp of the search
  wms_cell_info_s_type     cell_info    [IN]: Current cell information (PLMN/ TAC/ Cell Id), in which the CB SMS is received

RETURNS
  boolean: TRUE; message is duplicate
           FALSE; message is not duplicate

SIDE EFFECTS
  When a message is not a duplicate, the duplicate detection history will
  be updated with the the message information contained in 'entry'.

  Any duplicate detection history entry found with an expiration timestamp
  less than the value in 'search_time' is purged during the search.

===========================================================================*/
boolean wms_bc_lte_message_is_duplicate(
  wms_duplist_ident_t           list_id,
  void                         *msg_ptr,
  time_t                        search_time,
  wms_cell_info_s_type          cell_info
)
{
  boolean                    duplicate   = FALSE;
  wms_duplist_list_type    **list_to_use = NULL;
  wms_duplist_entry_type     entry;
  time_t                     msg_time;
  wms_bc_s_type             *bc_s_ptr    = wms_bc_s_ptr();

  // Get arrival time of the message
  msg_time = time_get_secs();

  if (WMS_LTE_DUPLIST_PRIMARY == list_id)
  {
    lte_rrc_etws_prim_ind_s   *lte_etws_prim_ptr;

    lte_etws_prim_ptr = (lte_rrc_etws_prim_ind_s *)msg_ptr;

    // Fill in the entry
    entry.serial_no   = lte_etws_prim_ptr->sr_num;
    entry.message_id  = lte_etws_prim_ptr->msg_id;
    entry.cell_id     = cell_info.phy_cell_id;
    entry.tac         = cell_info.tac;
    entry.plmn        = cell_info.selected_plmn;
    entry.timestamp   = msg_time + wms_msg_get_expiration_from_plmn(&(lte_etws_prim_ptr->plmn));

    list_to_use = &bc_s_ptr->primary_lte_duplist;
  }
  else if (WMS_LTE_DUPLIST_SECONDARY == list_id)
  {
    lte_rrc_etws_sec_ind_s   *lte_etws_sec_ptr;

    lte_etws_sec_ptr = (lte_rrc_etws_sec_ind_s *)msg_ptr;

    // Fill in the entry
    entry.serial_no   = lte_etws_sec_ptr->sr_num;
    entry.message_id  = lte_etws_sec_ptr->msg_id;
    entry.cell_id     = cell_info.phy_cell_id;
    entry.tac         = cell_info.tac;
    entry.plmn        = cell_info.selected_plmn;
    entry.timestamp   = msg_time + wms_msg_get_expiration_from_plmn(&(lte_etws_sec_ptr->plmn));

    list_to_use = &bc_s_ptr->secondary_lte_duplist;
  }
  else if (WMS_LTE_DUPLIST_CMAS == list_id)
  {
    lte_rrc_cmas_ind_s       *lte_cmas_ptr;

    lte_cmas_ptr = (lte_rrc_cmas_ind_s *)msg_ptr;

    // Fill in the entry
    entry.serial_no   = lte_cmas_ptr->sr_num;
    entry.message_id  = lte_cmas_ptr->msg_id;
    entry.cell_id     = cell_info.phy_cell_id;
    entry.tac         = cell_info.tac;
    entry.plmn        = cell_info.selected_plmn;
    entry.timestamp   = msg_time + wms_msg_get_expiration_from_plmn(&(lte_cmas_ptr->plmn));

    list_to_use = &bc_s_ptr->cmas_lte_duplist;
  }

  if (NULL != list_to_use)
  {
    duplicate = wms_duplist_search( *list_to_use,
                                    &entry,
                                    search_time,
                                    wms_bc_lte_bc_message_compare );
    if (!duplicate)
    {
      wms_duplist_ret_status_t res;

      // Add the new entry to the list
      res = wms_duplist_add(list_to_use, &entry);

      if (WMS_DUPLIST_SUCCESS <= res)
      {
        MSG_HIGH_1("Added non-duplicate message to list: res=%d", res);
      }
      else
      {
        MSG_ERROR_1("Failed to add non-duplicate message to list: res=%d", res);
      }
    }
  }

  return duplicate;
} /* wms_bc_lte_message_is_duplicate() */

/*===========================================================================
FUNCTION wms_bc_clear_duplists()                                   (INTERNAL)

DESCRIPTION
  Expires all entries in all duplists (ETWS primary, ETWS secondary, CMAS)

DEPENDENCIES
  None

PARAMETERS
  None

RETURNS
  Nothing

SIDE EFFECTS
  All duplicate detection history entries will be expired/purged.

===========================================================================*/
void wms_bc_clear_duplists(
  void
)
{
  wms_duplist_entry_type     entry;
  lte_rrc_plmn_s             plmn      = { {0, 0, 0}, 0, {0, 0, 0} };
  time_t                     cur_time;
  wms_bc_s_type             *bc_s_ptr  = wms_bc_s_ptr();

  memset(&entry, 0, sizeof(entry));

  // Get arrival time of the request and add max expiry plus one
  // This should create a timestamp greater than any existing entry's timestamp
  cur_time = time_get_secs() + wms_msg_get_expiration_from_plmn(&plmn) + 1;

  // Search using the large timestamp.
  // This will purge any entry with a smaller timestamp.
  // The second and fourth arguments are dummies and won't be used in the search.
  (void)wms_duplist_search( bc_s_ptr->primary_lte_duplist,
                            &entry,
                            cur_time,
                            wms_bc_lte_bc_message_compare );

  (void)wms_duplist_search( bc_s_ptr->secondary_lte_duplist,
                            &entry,
                            cur_time,
                            wms_bc_lte_bc_message_compare );

  (void)wms_duplist_search( bc_s_ptr->cmas_lte_duplist,
                            &entry,
                            cur_time,
                            wms_bc_lte_bc_message_compare );

} /* wms_bc_clear_duplists() */

#endif /* FEATURE_SMS_PWS_OVER_LTE */

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/* END of wmsbc.c */

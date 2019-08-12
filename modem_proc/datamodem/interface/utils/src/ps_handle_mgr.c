/*===========================================================================

                    D S S I _ H A N D L E _ M G R . C

DESCRIPTION
  This module contains the API which generates a handle given an index in to
  a pool of items.

EXTERNAL FUNCTIONS
  ps_handle_mgr_init_client()
    Initializes a client

  ps_handle_mgr_get_handle()
    Generates a handle given an index

  ps_handle_mgr_get_index()
    Returns an index given a handle

Copyright (c) 2007 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_handle_mgr.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/27/11    bd     Changed ps_handle_mgr_init_client to accept max possible 
                   handle instead of max base handle
04/17/09    pp     assert.h=>amssassert.h
01/09/09    ar     Added curr_base_handle initialization.
04/02/07    msr    Created the module
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "ps_handle_mgr.h"
#include "amssassert.h"
#include "err.h"
#include "msg.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

                               DATA STRUCTURES

===========================================================================*/
/*---------------------------------------------------------------------------
  Structure containing all the info related to a client

  Fields are
    num_items              : Number of items that are managed by the client
    min_base_handle        : Min base value that the client wants to use,
                             i.e. minimum value of a handle is "min_base_handle"
    max_base_handle        : Max base value that the client wants to use,
                             i.e. maximum value of a handle is
                             "max_base_handle + num_items"
    curr_base_handle       : Current base value used for the client
    max_handle_used_so_far : Max assigned value so far for the client
    is_inited              : Did the client call the init function?
---------------------------------------------------------------------------*/
typedef struct
{
  int16    num_items;
  int16    min_base_handle;
  int32    max_base_handle;
  int32    curr_base_handle;
  int32    max_handle_used_so_far;
  boolean  is_inited;
} ps_handle_mgr_client_info_type;

static ps_handle_mgr_client_info_type
  ps_handle_mgr_client_info[PS_HANDLE_MGR_CLIENT_MAX];



/*===========================================================================

                           EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_HANDLE_MGR_INIT_CLIENT()

DESCRIPTION
  This function initializes a client

PARAMETERS
  client_id            : ID of the client
  num_items            : Number of items that are managed by the client
  min_base_handle      : Minimum base value that client wants to use for a 
                         handle
  max_possible_handle  : Maximum possible value of handle that client wants  
                         to use

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_handle_mgr_init_client
(
  ps_handle_mgr_client_enum_type  client_id,
  int16                           num_items,
  int16                           min_base_handle,
  int32                           max_possible_handle
)
{
  ps_handle_mgr_client_info_type  * client_info_ptr;
  int32                             max_base_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (client_id >= PS_HANDLE_MGR_CLIENT_MAX)
  {
    LOG_MSG_ERROR_1("ps_handle_mgr_init_client(): "
                    "Invalid ID %d", client_id);
    ASSERT(0);
    return;
  }

  if (num_items <= 0 || min_base_handle < 0 || max_possible_handle < 0)
  {
    LOG_MSG_ERROR_3("ps_handle_mgr_init_client(): "
                    "Invalid input, # items %d, min_base %d, max_possible %d",
                    num_items, min_base_handle, max_possible_handle);
    ASSERT(0);
    return;
  }

  client_info_ptr = &ps_handle_mgr_client_info[client_id];

  if (client_info_ptr->is_inited)
  {
    LOG_MSG_ERROR_1("ps_handle_mgr_init_client(): "
                    "Client %d is inited already", client_id);
    ASSERT(0);
    return;
  }

  min_base_handle = MAX( min_base_handle, num_items);

  if (max_possible_handle != 0)
  {
    if (( max_possible_handle - min_base_handle) / num_items < 2)
    {
      LOG_MSG_INVALID_INPUT_1("ps_handle_mgr_init_client(): "
                              "max possible handle %d is not large enough",
                              max_possible_handle);
      ASSERT(0);
      return;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Setting max_possible_handle to pow(2, 31) - 1 as that is the maximum
      value, a int32 can support
    -----------------------------------------------------------------------*/
    max_possible_handle = (1 << 30) - 1;
  }

  /*-------------------------------------------------------------------------
    Shrink the max_possible_handle so that it is
    min_base_handle + num_items * n, where n is the largest possible value
   ------------------------------------------------------------------------*/
  max_possible_handle =
    min_base_handle +
      num_items * ((max_possible_handle - min_base_handle) / num_items);

  max_base_handle = max_possible_handle - num_items;

  if ((max_base_handle + num_items) > ((1 << 30) - 1))
  {
    DATA_ERR_FATAL("ps_handle_mgr_init_client(): Max Base handle + # of items > pow(2, 31) - 1");
    return;
  }

  client_info_ptr->is_inited              = TRUE;
  client_info_ptr->num_items              = num_items;
  client_info_ptr->min_base_handle        = min_base_handle;
  client_info_ptr->max_base_handle        = max_base_handle;
  client_info_ptr->curr_base_handle       = min_base_handle;
  client_info_ptr->max_handle_used_so_far = 0;

  return;
} /* ps_handle_mgr_init_client() */



/*===========================================================================
FUNCTION PS_HANDLE_MGR_GET_HANDLE()

DESCRIPTION
  This function is used to get a handle given a index. A client uses a pool of
  "n" items to allocate memory and "index" tells whether client wants a
  handle for 1st, 2nd, or xth item among these n items.

  A client initializes "num_items", "min_base", and "max_base". So possible
  values for a handle range from "min_base" to "max_base + num_items".
  This module maintains a value "curr_base" and given an "index", value of a
  handle is "curr_base + index". Whenever "index" is re-used, "curr_base" is
  incremented by "num_items". So valid values for curr_base are
  {
    min_base + 1 * num_items,
    min_base + 2 * num_items,
    min_base + 3 * num_items,
    ...
    ...
    min_base + n * num_items (which is <= max_base)
  }

  To help in updating "curr_base", this module maintain the maximum value
  assigned so far, "max_handle_used_so_far", which essentially is
  "curr_base + index". For example, assume that curr_base is 100. If client
  allocates 0th item in its pool, handle for that would be "100 + 0" = 100.
  If client allocates 1st item, handle for that would be 101 and
  "max_handle_used_so_far" becomes 101. If client frees 0th item and
  re-allocates it, then handle would be "100 + 0" = 100. But since 100 < 101,
  the module knows that this index had been used already and increments
  "curr_base".

  Mathematically, handle = (( min_base + x * num_items) + index)

PARAMETERS
  client_id : ID of the client
  index     : Index of the allocated buffer in the pool of n items

DEPENDENCIES
  Client must have been initialized

RETURN VALUE
  A handle                     : On success
  PS_HANDLE_MGR_INVALID_HANDLE : On error

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_handle_mgr_get_handle
(
  ps_handle_mgr_client_enum_type  client_id,
  int16                           index
)
{
  ps_handle_mgr_client_info_type  * client_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (client_id >= PS_HANDLE_MGR_CLIENT_MAX)
  {
    LOG_MSG_ERROR_1("ps_handle_mgr_get_handle(): "
                    "Invalid ID %d", client_id);
    ASSERT(0);
    return PS_HANDLE_MGR_INVALID_HANDLE;
  }

  if (index < 0)
  {
    LOG_MSG_ERROR_2("ps_handle_mgr_get_handle(): "
                    "Invalid index %d, client %d", index, client_id);
    ASSERT(0);
    return PS_HANDLE_MGR_INVALID_HANDLE;
  }

  client_info_ptr = &ps_handle_mgr_client_info[client_id];

  if (!( client_info_ptr->is_inited))
  {
    LOG_MSG_ERROR_1("ps_handle_mgr_get_handle(): "
                    "Client %d is not inited", client_id);
    ASSERT(0);
    return PS_HANDLE_MGR_INVALID_HANDLE;
  }

  /*-------------------------------------------------------------------------
    Make sure that index is not re-used. If so increment curr_base_handle
  -------------------------------------------------------------------------*/
  if (( index + client_info_ptr->curr_base_handle) <=
        client_info_ptr->max_handle_used_so_far)
  {
    client_info_ptr->curr_base_handle += client_info_ptr->num_items;

    if (client_info_ptr->curr_base_handle >= client_info_ptr->max_base_handle)
    {
      LOG_MSG_INFO2_3("ps_handle_mgr_get_handle(): "
                      "Changing handle base of client %d from %d to %d",
                      client_id,
                      client_info_ptr->curr_base_handle,
                      client_info_ptr->min_base_handle);
      client_info_ptr->curr_base_handle = client_info_ptr->min_base_handle;
      client_info_ptr->max_handle_used_so_far = 0;
    }
    else
    {
      client_info_ptr->max_handle_used_so_far =
        index + client_info_ptr->curr_base_handle;
    }
  }
  else
  {
    client_info_ptr->max_handle_used_so_far =
      index + client_info_ptr->curr_base_handle;
  }

  return index + client_info_ptr->curr_base_handle;

} /* ps_handle_mgr_get_handle() */



/*===========================================================================
FUNCTION PS_HANDLE_MGR_GET_INDEX()

DESCRIPTION
  This function is used to get index given a handle

  Since handle = (( min_base + x * num_items) + index),
  index = (handle - min_base) % num_items

PARAMETERS
  client_id : ID of the client
  handle    : Handle to an item

DEPENDENCIES
  Client must have been initialized

RETURN VALUE
  Index corresponding to the handle : On success
  PS_HANDLE_MGR_INVALID_INDEX       : On error

SIDE EFFECTS
  None
===========================================================================*/
int16 ps_handle_mgr_get_index
(
  ps_handle_mgr_client_enum_type  client_id,
  int32                           handle
)
{
  ps_handle_mgr_client_info_type  * client_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (client_id >= PS_HANDLE_MGR_CLIENT_MAX)
  {
    LOG_MSG_ERROR_1("ps_handle_mgr_get_index(): "
                    "Invalid ID %d", client_id);
    ASSERT(0);
    return PS_HANDLE_MGR_INVALID_INDEX;
  }

  client_info_ptr = &ps_handle_mgr_client_info[client_id];

  if (!( client_info_ptr->is_inited))
  {
    LOG_MSG_ERROR_1("ps_handle_mgr_get_index(): "
                    "Client %d is not inited", client_id);
    ASSERT(0);
    return PS_HANDLE_MGR_INVALID_INDEX;
  }

  if (handle < client_info_ptr->min_base_handle ||
      handle >=
        ( client_info_ptr->max_base_handle + client_info_ptr->num_items))
  {
    LOG_MSG_ERROR_1("ps_handle_mgr_get_index(): "
                    "Invalid handle %d", handle);
    return PS_HANDLE_MGR_INVALID_INDEX;
  }

  /*-------------------------------------------------------------------------
    A handle is nothing but ( min_base_handle + n * num_items) + index
  -------------------------------------------------------------------------*/
  return (int16)
    ((handle - client_info_ptr->min_base_handle) % client_info_ptr->num_items);

} /* ps_handle_mgr_get_index() */


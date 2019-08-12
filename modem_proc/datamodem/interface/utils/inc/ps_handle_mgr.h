#ifndef PS_HANDLE_MGR_H
#define PS_HANDLE_MGR_H
/*===========================================================================

                  D S S I _ H A N D L E _ M G R . H

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

Copyright (c) 2007 - 2011 Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ps_handle_mgr.h#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "customer.h"



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define DSS_NET_MGR_MAX_SESSIONS  (5)


typedef enum
{
  PS_HANDLE_MGR_CLIENT_MIN              = 0,
  PS_HANDLE_MGR_CLIENT_SOCK             = PS_HANDLE_MGR_CLIENT_MIN,
  PS_HANDLE_MGR_CLIENT_APP              = 1,
  PS_HANDLE_MGR_CLIENT_NET_MGR          = 2,
  PS_HANDLE_MGR_CLIENT_DNS_API          = 3,
  PS_HANDLE_MGR_CLIENT_DNS_SESSION_MGR  = 4,
  PS_HANDLE_MGR_CLIENT_DNS_RESOLVER     = 5,
  PS_HANDLE_MGR_CLIENT_DNS_IO_MGR       = 6,
  PS_HANDLE_MGR_CLIENT_IPHC             = 7,
  PS_HANDLE_MGR_CLIENT_ROHC             = 8,
#ifdef FEATURE_DATA_PS_ADDR_MGMT
  PS_HANDLE_MGR_CLIENT_IPV6_ADDR        = 9,
#endif
  PS_HANDLE_MGR_CLIENT_SOCKET_PLATFORM  = 10,

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  PS_HANDLE_MGR_CLIENT_DSS_SOCK      = 12,
  PS_HANDLE_MGR_CLIENT_DSS_NETAPP    = 13,
  PS_HANDLE_MGR_CLIENT_AUDIO_OFFLOAD_CLIENT = 14,
  PS_HANDLE_MGR_CLIENT_DPM_UM        = 15,
  PS_HANDLE_MGR_CLIENT_DPM_RM        = 16,
  PS_HANDLE_MGR_CLIENT_DPM_BEARER    = 17,
  PS_HANDLE_MGR_CLIENT_QOS           = 18,
  PS_HANDLE_MGR_CLIENT_MAX
} ps_handle_mgr_client_enum_type;

#define PS_HANDLE_MGR_INVALID_HANDLE  ((int32)-1)
#define PS_HANDLE_MGR_INVALID_INDEX   ((int16)-1)


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                           EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_HANDLE_MGR_INIT_CLIENT()

DESCRIPTION
  This function initializes a client

PARAMETERS
  client_id       : ID of the client
  num_items       : Number of items that are managed by the client
  min_base_handle      : Minimum base value that client wants to use 
                         for a handle
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
  ps_handle_mgr_client_enum_type    client_id,
  int16                             num_items,
  int16                             min_base_handle,
  int32                             max_possible_handle
);


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
  A handle                       : On success
  PS_HANDLE_MGR_INVALID_HANDLE : On error

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_handle_mgr_get_handle
(
  ps_handle_mgr_client_enum_type    client_id,
  int16                             index
);


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
  PS_HANDLE_MGR_INVALID_INDEX     : On error

SIDE EFFECTS
  None
===========================================================================*/
int16 ps_handle_mgr_get_index
(
  ps_handle_mgr_client_enum_type    client_id,
  int32                             handle
);

#ifdef __cplusplus
}
#endif

#endif /* PS_HANDLE_MGR_H */

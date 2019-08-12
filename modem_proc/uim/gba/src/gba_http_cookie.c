/*===========================================================================


            G B A   H T T P  C O O K I E   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/15   tkl     Initial revision
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "amssassert.h"
#include <stringl/stringl.h>

#include "gba.h"
#include "gba_http_cookie.h"
#include "gba_platform.h"
#include "gba_storage.h"
#include "gba_time.h"
#include "gba_util.h"

/*=============================================================================

                       TYPE DECLARATIONS

=============================================================================*/

typedef struct gba_http_cookie_node
{
  struct gba_http_cookie_node  * next_ptr;
  gba_http_cookie_data_type      cookie;
}gba_http_cookie_node_type;

typedef struct
{
  boolean                        init;
  gba_http_header_value_type     impi;
  gba_session_type               session_type;
  uint16                         num_cookies;
  gba_http_cookie_node_type    * cookie_list_ptr;
} gba_http_cookie_info_type;

static gba_http_cookie_info_type gba_http_cookie_info = {FALSE, {0}, 0, 0, NULL};

/*=============================================================================

                       FUNCTIONS

=============================================================================*/
/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_FREE

DESCRIPTION
  Function clears and then frees the linked list of cookies

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_http_cookie_free (
  void
)
{
  gba_http_cookie_node_type  * node_ptr      = NULL;
  gba_http_cookie_node_type  * temp_node_ptr = NULL;

  node_ptr = gba_http_cookie_info.cookie_list_ptr;

  while(node_ptr != NULL)
  {
    temp_node_ptr = node_ptr->next_ptr;
    GBA_FREE(node_ptr);
    node_ptr = temp_node_ptr;
  }

  memset(&gba_http_cookie_info, 0x00, sizeof(gba_http_cookie_info_type));
} /* gba_http_cookie_free */


/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_DELETE_EXPIRED_COOKIE

DESCRIPTION
  This function checks and delete expired cookies in the linked list

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gba_http_cookie_delete_expired_cookie (
  void
)
{
  gba_http_cookie_node_type  * node_ptr          = NULL;
  gba_http_cookie_node_type  * prev_node_ptr     = NULL;
  gba_http_cookie_node_type  * temp_node_ptr     = NULL;

  node_ptr = gba_http_cookie_info.cookie_list_ptr;

  while(node_ptr != NULL && gba_http_cookie_info.num_cookies > 0)
  {
    if(!node_ptr->cookie.expiration_set ||
       (node_ptr->cookie.expiration_set &&
        !gba_time_is_time_expired(node_ptr->cookie.expiration, NULL)))
    {
      prev_node_ptr = node_ptr;
      node_ptr = node_ptr->next_ptr;
      continue;
    }

    /* delete expired cookie */
    gba_http_cookie_info.num_cookies--;
    GBA_MSG_SPRINTF_2("delete cookie : %s = %s",
                      node_ptr->cookie.name_value.name,
                      node_ptr->cookie.name_value.value);

    temp_node_ptr = node_ptr->next_ptr;

    GBA_FREE(node_ptr);

    node_ptr = temp_node_ptr;
    if(prev_node_ptr == NULL)
    {
      gba_http_cookie_info.cookie_list_ptr = node_ptr;
    }
    else
    {
      prev_node_ptr->next_ptr = node_ptr;
    }
  }
} /* gba_http_cookie_delete_expired_cookie */


/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_READ_FROM_STORAGE

DESCRIPTION
  This function read Cookie data from storage to linked list

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gba_http_cookie_read_from_storage (
  void
)
{
  uint8                            * cookie_storage_ptr = NULL;
  uint16                             storage_data_size  = 0;
  uint16                             offset             = 0;
  uint16                             i                  = 0;

  if(gba_http_cookie_info.init)
  {
    return;
  }

  gba_http_cookie_info.init = TRUE;

  if(gba_http_cookie_info.cookie_list_ptr != NULL)
  {
    gba_http_cookie_free();
  }

  /* data format in storage : | impi | session_type | num of cookies | cookies data | */
  cookie_storage_ptr = gba_storage_get_cookies(&storage_data_size);

  /* also check for min. size of data stored with at least 1 cookie data */
  if(cookie_storage_ptr == NULL ||
     storage_data_size < (sizeof(gba_http_header_value_type) +
                          sizeof(gba_session_type) +
                          sizeof(uint16) +
                          sizeof(gba_http_cookie_data_type)))
  {
    GBA_FREE(cookie_storage_ptr);
    return;
  }

  /* impi */
  (void)memscpy(gba_http_cookie_info.impi, sizeof(gba_http_header_value_type),
                &cookie_storage_ptr[offset], sizeof(gba_http_header_value_type));
  offset = offset + sizeof(gba_http_header_value_type);

  /* session type */
  (void)memscpy(&gba_http_cookie_info.session_type, sizeof(gba_session_type),
                &cookie_storage_ptr[offset], sizeof(gba_session_type));
  offset = offset + sizeof(gba_session_type);

  /* num of cookies */
  (void)memscpy(&gba_http_cookie_info.num_cookies, sizeof(uint16),
                &cookie_storage_ptr[offset], sizeof(uint16));
  offset = offset + sizeof(uint16);

  /* check storage size aganist with size of cookie data */
  if(gba_http_cookie_info.num_cookies == 0 ||
     ((storage_data_size - offset) <
       (gba_http_cookie_info.num_cookies * (uint16)(sizeof(gba_http_cookie_data_type)))))
  {
    GBA_FREE(cookie_storage_ptr);
    memset(&gba_http_cookie_info, 0x00, sizeof(gba_http_cookie_info_type));
    return;
  }

  /* convert cookie from storage to linked list */
  for(i = 0; i < gba_http_cookie_info.num_cookies; i++)
  {
    gba_http_cookie_node_type        * node_ptr = NULL;

    node_ptr = (gba_http_cookie_node_type *)GBA_MALLOC(sizeof(gba_http_cookie_node_type));
    if(node_ptr == NULL)
    {
      GBA_FREE(cookie_storage_ptr);
      return;
    }

    /* add new node to head of linked list */
    if(gba_http_cookie_info.cookie_list_ptr == NULL)
    {
      gba_http_cookie_info.cookie_list_ptr = node_ptr;
    }
    else
    {
      node_ptr->next_ptr = gba_http_cookie_info.cookie_list_ptr;
      gba_http_cookie_info.cookie_list_ptr = node_ptr;
    }

    (void)memscpy(&node_ptr->cookie, sizeof(gba_http_cookie_data_type),
                  &cookie_storage_ptr[offset], sizeof(gba_http_cookie_data_type));
    offset = offset + sizeof(gba_http_cookie_data_type);
  }

  GBA_FREE(cookie_storage_ptr);
} /* gba_http_cookie_read_from_storage */


/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_WRITE_TO_STORAGE

DESCRIPTION
  This function write Cookie data from linked list to storage

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_http_cookie_write_to_storage (
  void
)
{
  /* take the cookies from linked list and write to storage */
  uint8                     * storage_data_ptr  = NULL;
  uint16                      data_size         = 0;
  uint16                      offset            = 0;
  gba_http_cookie_node_type * node_ptr          = gba_http_cookie_info.cookie_list_ptr;
  gba_result_enum_type        gba_status        = GBA_SUCCESS;
  uint16                      i                 = 0;

  if(node_ptr == NULL)
  {
    /* no cookie to write */
    return GBA_SUCCESS;
  }

  /* data format in storage : | impi | session_type | num of cookies | cookies data | */
  data_size = sizeof(gba_http_header_value_type) + sizeof(gba_session_type) + sizeof(uint16) +
              (gba_http_cookie_info.num_cookies * sizeof(gba_http_cookie_data_type));

  storage_data_ptr = (void *)GBA_MALLOC(data_size);

  if(storage_data_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* impi */
  (void)memscpy(&storage_data_ptr[offset], data_size - offset,
                gba_http_cookie_info.impi, sizeof(gba_http_header_value_type));
  offset = offset + sizeof(gba_http_header_value_type);

  /* session type */
  (void)memscpy(&storage_data_ptr[offset], data_size - offset,
                &gba_http_cookie_info.session_type, sizeof(gba_session_type));
  offset = offset + sizeof(gba_session_type);

  /* number of cookies */
  (void)memscpy(&storage_data_ptr[offset], data_size - offset,
                &gba_http_cookie_info.num_cookies, sizeof(uint16));
  offset = offset + sizeof(uint16);

  /* cookies data */
  for(i = 0;
      (i < gba_http_cookie_info.num_cookies && node_ptr != NULL &&
       offset <= (data_size - sizeof(gba_http_cookie_data_type)));
      i++)
  {
    (void)memscpy(&storage_data_ptr[offset], data_size - offset,
                  &node_ptr->cookie, sizeof(gba_http_cookie_data_type));
    node_ptr = node_ptr->next_ptr;
    offset = offset + sizeof(gba_http_cookie_data_type);
  }

  gba_status = gba_storage_put_cookies((const void *)storage_data_ptr, data_size);

  GBA_FREE(storage_data_ptr);

  return gba_status;
} /* gba_http_cookie_write_to_storage */


/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_VALIDATE_COOKIES

DESCRIPTION
  This function validate for the Session type & IMPI assoicated with cookie list,
  and delete expired cookies on the list.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gba_http_cookie_validate_cookies (
  gba_http_header_value_type            impi,
  gba_session_type                      session_type
)
{
  if(!gba_http_cookie_info.init)
  {
    gba_http_cookie_read_from_storage();
  }

  /* check against Session type and IMPI */
  if(session_type != gba_http_cookie_info.session_type ||
     ((strlen(impi) != strlen(gba_http_cookie_info.impi)) ||
      (strcmp(impi, gba_http_cookie_info.impi) != 0)))
  {
    /* IMPI for Ub procedure has changed, ok to clear the storage */
    gba_storage_clear_cookies();
    gba_http_cookie_free();

    gba_http_cookie_info.init         = TRUE;
    gba_http_cookie_info.session_type = session_type;
    memscpy(gba_http_cookie_info.impi, sizeof(gba_http_header_value_type),
            impi, sizeof(gba_http_header_value_type));
    GBA_MSG_SPRINTF_2("switch cookie for IMPI : %s; session_type : 0x%x",
                      gba_http_cookie_info.impi,
                      session_type);
    return;
  }

  if(gba_http_cookie_info.num_cookies == 0)
  {
    return;
  }

  /* delete expired cookies */
  gba_http_cookie_delete_expired_cookie();
} /* gba_http_cookie_validate_cookies */


/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_GET_COOKIE_HEADER

DESCRIPTION
  This function checks for the IMPI assoicated with cookie, returns
  total number of cookies and copies name/value of cookies to cookie_list_ptr. Caller
  should allocate memory with size >= max_num_cookies * ds_http_cookie_name_s_type
  for cookie_list_ptr

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_http_cookie_get_cookie_header (
  gba_http_header_value_type            impi,
  gba_session_type                      session_type,
  uint16                                max_num_cookies,
  uint16                              * num_cookies_ptr,
  ds_http_cookie_name_s_type          * cookie_list_ptr
)
{
  gba_http_cookie_node_type    * node_ptr       = NULL;

  if(num_cookies_ptr == NULL || cookie_list_ptr == NULL || max_num_cookies == 0)
  {
    return;
  }

  * num_cookies_ptr = 0;
  gba_http_cookie_validate_cookies(impi, session_type);

  if(gba_http_cookie_info.num_cookies == 0)
  {
    return;
  }

  memset(cookie_list_ptr,
         0x00,
         max_num_cookies * sizeof(ds_http_cookie_name_s_type));

  node_ptr = gba_http_cookie_info.cookie_list_ptr;

  while(node_ptr != NULL && *num_cookies_ptr < max_num_cookies)
  {
    /* since Ub only uses HTTP(s), there's no need to check http_only flag
       also no need to check secure flag assume that GBA will use the same
       bsf protocol for the BSF */
    cookie_list_ptr[*num_cookies_ptr] = node_ptr->cookie.name_value;
    *num_cookies_ptr = *num_cookies_ptr + 1;
    GBA_MSG_SPRINTF_2("GBA get cookie header : %s = %s",
                      node_ptr->cookie.name_value.name,
                      node_ptr->cookie.name_value.value);
    node_ptr = node_ptr->next_ptr;
  }
} /* gba_http_cookie_get_cookie_header */


/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_ADD_COOKIE

DESCRIPTION
  This function add a cookie in cookie list

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_http_cookie_add_cookie (
  gba_http_header_value_type           impi,
  gba_session_type                     session_type,
  const gba_http_cookie_data_type    * cookie_ptr
)
{
  gba_http_cookie_node_type  * node_ptr          = NULL;

  if(NULL == cookie_ptr)
  {
    return GBA_INCORRECT_PARAMS;
  }

  GBA_MSG_SPRINTF_3("add GBA cookie : %s = %s; expiration: 0x%x",
                    cookie_ptr->name_value.name,
                    cookie_ptr->name_value.value,
                    cookie_ptr->expiration_set);

  if(cookie_ptr->expiration_set)
  {
    GBA_MSG_LOW_6("cookie time: year %d, month %d, day %d, hour %d, min %d, sec %d",
                  cookie_ptr->expiration.year, cookie_ptr->expiration.month, cookie_ptr->expiration.day,
                  cookie_ptr->expiration.hour, cookie_ptr->expiration.minute, cookie_ptr->expiration.second);
  }

  gba_http_cookie_validate_cookies(impi, session_type);

  node_ptr = gba_http_cookie_info.cookie_list_ptr;

  /* From RFC 6265 S5.3 #11: cookie with the same name, domain, and path as
     the newly created cookie. copy the new-cookie and remove old-cookie from
     cookie store.
     since 11.2 & 11.3(check for non-HTTP API and cookie creation-time) are not
     applicable here, those checks are not performed. */
  while(node_ptr != NULL)
  {
    /* search for cookie with same name */
    if((strlen(cookie_ptr->name_value.name) ==
        strlen(node_ptr->cookie.name_value.name)) &&
       (strcmp(cookie_ptr->name_value.name,
               node_ptr->cookie.name_value.name) == 0))
    {
      /* if node with same name is found, reuse the same node ptr
         for new cookie data */
      break;
    }
    node_ptr = node_ptr->next_ptr;
  }

  if(node_ptr == NULL)
  {
    /* No node with same name was found, allocate a new node
       and add to head of linked list */
    node_ptr = (gba_http_cookie_node_type *)
                        GBA_MALLOC(sizeof(gba_http_cookie_node_type));
    if(node_ptr == NULL)
    {
      return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    node_ptr->next_ptr = gba_http_cookie_info.cookie_list_ptr;
    gba_http_cookie_info.cookie_list_ptr = node_ptr;
    gba_http_cookie_info.num_cookies++;
  }

  (void)memscpy(&node_ptr->cookie, sizeof(gba_http_cookie_data_type),
                cookie_ptr, sizeof(gba_http_cookie_data_type));

  GBA_MSG_LOW_1("num_cookies on the list: 0x%x",
                gba_http_cookie_info.num_cookies);

  return GBA_SUCCESS;
} /* gba_http_cookie_add_cookie */


/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_CLEANUP

DESCRIPTION
  Function writes cookies to storage and frees the list of cookies

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_http_cookie_cleanup (
  void
)
{
  (void)gba_http_cookie_write_to_storage();
  gba_http_cookie_free();
} /* gba_http_cookie_cleanup */


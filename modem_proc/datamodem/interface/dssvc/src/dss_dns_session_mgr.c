/*===========================================================================

              D N S   S E S S I O N   M A N A G E R   M O D U L E

DESCRIPTION

  The Data Services DNS Subsystem Session manager module. Contains
  declarations of functions, variables, macros, structs and enums
  used by Session manager.

EXTERNALIZED FUNCTIONS

  dss_dns_create_session()
    Creates a session corresponding to a set of DNS lookups.

  dss_dns_set_config_params()
    Sets the specified configuration options for a given session.

  dss_dns_get_config_params()
    Retrieves the configuration option for a given session.

  dss_dns_delete_session()
    Deletes a specified session.


Copyright (c) 2007-2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                             EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Note that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssvc/src/dss_dns_session_mgr.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/11   ea      Add a locking to the access to config params.
04/26/11   ea      Add to dss_dns_session_mgr_is_handle_valid() a check if the
                   session is currently being deleted.
08/07/08   dm      Klocwork fixes
06/11/08   hm      Supporting net policy as session config param
06/18/07   hm      Returning EBADF on invalid handle.
06/11/07   hm      Un-included dss_dns_config.h
03/26/07   hm      Initial development work.

===========================================================================*/


/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>
#include "dssdns.h"
#include "dss_dns_config.h"
#include "dss_dns_priv.h"
#include "dssocket.h"
#include "ps_dnsi_defs.h"
#include "ps_mem.h"
#include "ps_handle_mgr.h"
#include "ps_dnsi_resolver.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                            PUBLIC DATA DEFINITIONS

===========================================================================*/
#define SESSION_CLIENT                (PS_HANDLE_MGR_CLIENT_DNS_SESSION_MGR)


/*===========================================================================

                         INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Forward declarations for internal session management functions
---------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION PS_DNSI_API_DELETE_INSTANCE()

DESCRIPTION
  Deletes the API control block instance specified by the given handle.

PARAMETERS
  api_cb_handle - Handle to the API control block to delete.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void ps_dnsi_api_delete_instance
(
  dss_dns_query_handle_type   api_cb_handle
);

/*===========================================================================
FUNCTION DSS_DNS_SESSION_MGRI_GET_CB_PTR

DESCRIPTION
  Checks if the specified handle is valid and if it is valid gets the
  control block corresponding to the handle.

PARAMETERS
  session_handle  - Session manager control block handle

RETURN VALUE
  Pointer to the control block if the specified handle is valid.
  NULL if the specified handle is not valid.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
ps_dnsi_session_mgr_cb_type  * dss_dns_session_mgri_get_cb_ptr
(
  dss_dns_session_mgr_handle_type     session_handle
)
{
  ps_dnsi_session_mgr_cb_type       * session_mgr_ptr;
  int16                               buf_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (DSS_DNS_SESSION_MGR_INVALID_HANDLE == session_handle)
  {
    LOG_MSG_ERROR_1 ("Invalid session handle %d", session_handle);
    return NULL;
  }

  buf_index = ps_handle_mgr_get_index(SESSION_CLIENT,
                                       session_handle);
  if (PS_HANDLE_MGR_INVALID_INDEX == buf_index)
  {
    LOG_MSG_ERROR_1 ("Invalid index for handle %d", buf_index);
    return NULL;
  }

  session_mgr_ptr =
    (ps_dnsi_session_mgr_cb_type *)
      ps_mem_index_to_buf(buf_index, PS_MEM_DNSI_SESSION_MGR_CB_TYPE);

  if (NULL == session_mgr_ptr || session_mgr_ptr->session_handle != session_handle)
  {
    LOG_MSG_ERROR_2 ("Inv session CB 0x%p from handle %d",
                     session_mgr_ptr, session_handle);
    return NULL;
  }

  return session_mgr_ptr;

} /* dss_dns_session_mgri_get_cb_ptr() */

/*===========================================================================
STATIC FUNCTION DSS_DNS_SESSION_MGRI_DELETE_CMD_HANDLER

DESCRIPTION
  This function processes PS_DNS_DELETE_SESSION_CMD.

PARAMETERS
  ps_cmd   - Must be PS_DNS_DELETE_SESSION_CMD.
  session_handle - Handle to the session to be deleted.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dss_dns_session_mgri_delete_cmd_handler
(
  ps_cmd_enum_type                  ps_cmd,
  void                            * session_data_ptr
)
{
  ps_dnsi_session_mgr_cb_type     * session_mgr_ptr = NULL;
  int16                             retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == session_data_ptr || PS_DNS_DELETE_SESSION_CMD != ps_cmd)
  {
    LOG_MSG_ERROR_0 ("dss_dns_session_mgri_delete_cmd_handler(): wrong parameters");
    return;
  }

  session_mgr_ptr = (ps_dnsi_session_mgr_cb_type *)session_data_ptr;
  LOG_MSG_INFO2_2 ("Processing PS_DNS_DELETE_SESSION_CMD for %d (addr %x)",
                   session_mgr_ptr->session_handle, session_mgr_ptr);


  /*------------------------------------------------------------------------
    Delete all resolver instances associated with this session.
  -------------------------------------------------------------------------*/
  ps_dnsi_resolver_delete_all_instances(session_mgr_ptr->session_handle);
  if (NULL != session_mgr_ptr->api_cb_ptr)
  {
    ps_dnsi_api_delete_instance(session_mgr_ptr->api_cb_ptr->handle);
  }
  
  /*------------------------------------------------------------------------
    Clean the DNS server queues
  -------------------------------------------------------------------------*/
  (void)dss_dns_priv_clear_server_queue(session_mgr_ptr->session_handle, &retval);
  q_destroy(&session_mgr_ptr->dns_ipv4_servers_q);
  q_destroy(&session_mgr_ptr->dns_ipv6_servers_q);
  memset(session_mgr_ptr, 0, sizeof(ps_dnsi_session_mgr_cb_type));
  session_mgr_ptr->session_handle = DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  PS_MEM_FREE(session_mgr_ptr);

  return;

} /* dss_dnsi_delete_session_cmd_handler() */


/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION  DSS_DNS_SESSION_MGR_INIT

DESCRIPTION
  Initializes the session manager module.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void dss_dns_session_mgr_init
(
  void
)
{
  /*------------------------------------------------------------------------
    Set command handler for Session manager module.
  -------------------------------------------------------------------------*/
  (void) ps_set_cmd_handler(PS_DNS_DELETE_SESSION_CMD,
                             dss_dns_session_mgri_delete_cmd_handler);

} /* dss_dns_session_mgr_init() */


/*===========================================================================
FUNCTION DSS_DNS_SESSION_MGR_IS_HANDLE_VALID

DESCRIPTION
  Checks if the specified session handle is valid or not.

PARAMETERS
  session_handle  - Session manager control block handle

RETURN VALUE
  TRUE if the handle is valid
  FALSE if the handle is not valid.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dss_dns_session_mgr_is_handle_valid
(
  dss_dns_session_mgr_handle_type     session_handle
)
{
  ps_dnsi_session_mgr_cb_type  *session_mgr_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);

  if ((NULL == session_mgr_ptr) || (session_mgr_ptr->session_deleted))
  {
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return FALSE;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;
} /* dss_dns_session_mgr_is_handle_valid() */


/*===========================================================================
FUNCTION DSS_DNS_SESSION_MGR_GET_CBACK_INFO

DESCRIPTION
  Gets the callback information from the session manager.

PARAMETERS
  session_handle    - Session manager control block handle
  cback_f_ptr_ptr   - Placeholder for callback function
  user_data_ptr_ptr - Placeholder for application user data

RETURN VALUE
  DSS_SUCCESS - if the session is valid.
  DSS_ERROR   - if the session handle specified is invalid.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_session_mgr_get_cback_info
(
  dss_dns_session_mgr_handle_type      session_handle,
  dss_dns_cback_f_type               * cback_f_ptr_ptr,
  void                              ** user_data_ptr_ptr
)
{
  ps_dnsi_session_mgr_cb_type  *session_mgr_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == cback_f_ptr_ptr )
  {
    LOG_MSG_ERROR_0("Null cback_f_ptr_ptr passed" );
    return DSS_ERROR;
  }
  if( NULL == user_data_ptr_ptr )
  {
    LOG_MSG_ERROR_0("Null user_data_ptr_ptr passed" );
    return DSS_ERROR;
  }

  if (NULL ==
      (session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle)))
  {
    LOG_MSG_ERROR_1 ("Invalid session handle %d", session_handle);
    return DSS_ERROR;
  }

  /*------------------------------------------------------------------------
    Check if the session is valid (e.g. has not already been deleted.
    If no then the application callback should not be set
  -------------------------------------------------------------------------*/

  if (dss_dns_session_mgr_is_handle_valid(session_handle))
  {
    *cback_f_ptr_ptr   = session_mgr_ptr->cback_f_ptr;
    *user_data_ptr_ptr = session_mgr_ptr->user_data_ptr;
    return DSS_SUCCESS;
  }

  LOG_MSG_INFO1_1 ("Session %d has already been deleted", session_handle);
  return DSS_ERROR;

} /* dss_dns_session_mgr_get_cback_info() */

/*===========================================================================
FUNCTION DSS_DNS_SESSION_MGR_IS_NET_POLICY_SET

DESCRIPTION
  This function determines if app has set network policy or iface id for
  a particular session.

PARAMETERS
  session_handle    - Session manager control block handle

RETURN VALUE
  TRUE - if network policy or iface id is set.
  FALSE - otherwise.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dss_dns_session_mgr_is_net_policy_set
(
  dss_dns_session_mgr_handle_type      session_handle
)
{
  ps_dnsi_session_mgr_cb_type  *session_mgr_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);

  if ((NULL == session_mgr_ptr) ||
      (!dss_dns_session_mgr_is_handle_valid(session_handle)))
  /* the check if the pointer is NULL is actually included in the second check,
     and was added just to avoid KW error */
  {
    return FALSE;
  }

  return session_mgr_ptr->is_net_policy_set;
} /* dss_dns_session_mgr_is_net_policy_set() */

/*===========================================================================
FUNCTION  DSS_DNS_CREATE_SESSION()

DESCRIPTION
  The function creates a session control block corresponding to a set of DNS
  lookups. The created session must be cleaned by the application after
  all the DNS lookups are complete. The application MUST specify callback
  information while creating a session.

PARAMETERS
  app_cback_f_ptr   - Application registered callback function pointer.
  user_data_ptr     - Application user data. (Optional)
  dss_errno             - Error code returned in case of error.

RETURN VALUE
  Session handle on success
  DSS_DNS_SESSION_MGR_INVALID_HANDLE on error and sets the dss_errno to error code.

  dss_errno values
  ------------
  DS_EFAULT     - Invalid parameters.
  DS_ENOMEM     - No memory available to allocate control block.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
dss_dns_session_mgr_handle_type   dss_dns_create_session
(
  dss_dns_cback_f_type            app_cback_f_ptr,
  void                          * user_data_ptr,
  int16                         * dss_errno
)
{
  ps_dnsi_session_mgr_cb_type   * session_mgr_ptr = NULL;
  int32                           retval = DSS_ERROR;
  int16                           buf_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
  LOG_MSG_FUNCTION_ENTRY_2("dss_dns_create_session(): "
    "cback 0x%p, caller 0x%p",
    app_cback_f_ptr, caller_ptr);
  /*-------------------------------------------------------------------------
    Validate arguments.
  -------------------------------------------------------------------------*/
  if (NULL == dss_errno)
  {
    LOG_MSG_ERROR_0 ("NULL errno");
    return DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  }

  if (NULL == app_cback_f_ptr)
  {
    LOG_MSG_ERROR_0 ("NULL application callback");
    *dss_errno = DS_EFAULT;
    return DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  }

  /*-------------------------------------------------------------------------
    Get memory for Session manager control block.
  -------------------------------------------------------------------------*/
  session_mgr_ptr = (ps_dnsi_session_mgr_cb_type *)
                    ps_mem_get_buf(PS_MEM_DNSI_SESSION_MGR_CB_TYPE);
  if (NULL == session_mgr_ptr)
  {
    *dss_errno = DS_ENOMEM;
    return DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  } else {
     LOG_MSG_INFO2_1("Allocated session mgr cb %x", session_mgr_ptr);
  }

  /*------------------------------------------------------------------------
    Fill Session manager control block.
  -------------------------------------------------------------------------*/
  memset(session_mgr_ptr, 0, sizeof(ps_dnsi_session_mgr_cb_type));

  buf_index = ps_mem_buf_to_index((void*)session_mgr_ptr);
  if (-1 == buf_index)
  {
    LOG_MSG_ERROR_1 ("Cant get index for session block 0x%p", session_mgr_ptr);
    *dss_errno = DS_ENOMEM;
    PS_MEM_FREE(session_mgr_ptr);
    return DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  }

  retval = ps_handle_mgr_get_handle(SESSION_CLIENT, buf_index);
  if (PS_HANDLE_MGR_INVALID_HANDLE == retval)
  {
    LOG_MSG_ERROR_2 ("Invalid handle %d for index %d", retval, buf_index);
    *dss_errno = DS_ENOMEM;
    PS_MEM_FREE(session_mgr_ptr);
    return DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  }

  session_mgr_ptr->session_handle       = retval;
  session_mgr_ptr->cback_f_ptr          = app_cback_f_ptr;
  session_mgr_ptr->user_data_ptr        = user_data_ptr;
  session_mgr_ptr->add_to_cache         = TRUE;
  session_mgr_ptr->use_cache_results    = TRUE;
  session_mgr_ptr->session_deleted      = FALSE;
  session_mgr_ptr->is_recursion_desired = TRUE;

  retval = 
    dss_dns_getconst(DSS_DNS_CONST_DEFAULT_RETRIES_BACKOFF,
                     &(session_mgr_ptr->linear_backoff_retry_parameters_info.backoff),
                     sizeof(session_mgr_ptr->linear_backoff_retry_parameters_info.backoff));
  
  if (DSS_SUCCESS != retval)
  {
    LOG_MSG_ERROR_0 ("Failed to get const DSS_DNS_CONST_DEFAULT_RETRIES_BACKOFF");
    *dss_errno = DS_EFAULT;
    PS_MEM_FREE(session_mgr_ptr);
    return DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  }
  
  retval = 
    dss_dns_getconst(DSS_DNS_CONST_DEFAULT_FIRST_QUERY_TIMEOUT,
              &(session_mgr_ptr->linear_backoff_retry_parameters_info.first_retry_time),
              sizeof(session_mgr_ptr->linear_backoff_retry_parameters_info.first_retry_time));

  if (DSS_SUCCESS != retval)
  {
    LOG_MSG_ERROR_0 ("Failed to get const DSS_DNS_CONST_DEFAULT_FIRST_QUERY_TIMEOUT");
    *dss_errno = DS_EFAULT;
    PS_MEM_FREE(session_mgr_ptr);
    return DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  }

  retval = 
    dss_dns_getconst(DSS_DNS_CONST_DEFAULT_MAX_QUERY_RETRIES,
                     &(session_mgr_ptr->linear_backoff_retry_parameters_info.max_retries),
                     sizeof(session_mgr_ptr->linear_backoff_retry_parameters_info.max_retries));

  if (DSS_SUCCESS != retval)
  {
    LOG_MSG_ERROR_0 ("Failed to get const DSS_DNS_CONST_DEFAULT_MAX_QUERY_RETRIES");
    *dss_errno = DS_EFAULT;
    PS_MEM_FREE(session_mgr_ptr);
    return DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  }

  /* Initialize the network policy to default network policy */
  session_mgr_ptr->is_net_policy_set    = FALSE;
  dss_init_net_policy_info(&(session_mgr_ptr->net_policy));

  session_mgr_ptr->api_cb_ptr           = NULL;
  
  /* Initialize the DNS servers queues and related parameters */
  session_mgr_ptr->client_specified_servers_behavior = USE_LAST;
  memset(&(session_mgr_ptr->dns_ipv4_servers_q), 0,
         sizeof(session_mgr_ptr->dns_ipv4_servers_q));
  (void) q_init(&(session_mgr_ptr->dns_ipv4_servers_q));
  memset(&(session_mgr_ptr->dns_ipv6_servers_q), 0,
         sizeof(session_mgr_ptr->dns_ipv6_servers_q));
  (void) q_init(&(session_mgr_ptr->dns_ipv6_servers_q));

  return session_mgr_ptr->session_handle;
}


/*===========================================================================
FUNCTION  DSS_DNS_SET_CONFIG_PARAMS

DESCRIPTION
  This function sets a specified configuration parameter for a given session.

PARAMETERS
  session_handle    - Identifies the session.
  param_name        - Name of the configuration parameter to be set.
  param_val_ptr     - Value of the parameter to be set.
  param_len         - Identifies the length of the param_val_ptr that
                      can be used.
  dss_errno             - Error code in case of error.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on failure and sets error code in dss_errno.

  dss_errno values
  ------------
  DS_EBADF    - Invalid session handle.
  DS_EFAULT   - Invalid arguments.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16  dss_dns_set_config_params
(
  dss_dns_session_mgr_handle_type   session_handle,
  dss_dns_config_params_enum_type   param_name,
  void                            * param_val_ptr,
  uint32                            param_len,
  int16                           * dss_errno
)
{
  ps_dnsi_session_mgr_cb_type     * session_mgr_ptr = NULL;
  int16                             retval = DSS_ERROR;
  dss_iface_id_type                 iface_id;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("dss_dns_set_config_params(): "
    "Session %d, caller 0x%p",
    session_handle, caller_ptr);

  /*-------------------------------------------------------------------------
    Validate arguments.
  -------------------------------------------------------------------------*/
  if (NULL == dss_errno)
  {
    LOG_MSG_ERROR_0 ("NULL errno");
    return DSS_ERROR;
  }

  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);
  if ((NULL == session_mgr_ptr) ||
      (!dss_dns_session_mgr_is_handle_valid(session_handle)))
  /* the check if the pointer is NULL is actually included in the second check,
     and was added just to avoid KW error */
  {
    LOG_MSG_ERROR_1 ("Invalid session handle %d", session_handle);
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }

  if (NULL == param_val_ptr)
  {
    LOG_MSG_ERROR_0 ("Null param val ptr");
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if (DSS_DNS_CONFIG_PARAMS_MAX <= param_name)
  {
    LOG_MSG_ERROR_1 ("Invalid param name %d", param_name);
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  switch (param_name)
  {
    case DSS_DNS_CONFIG_PARAMS_IFACE_ID:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(dss_iface_id_type) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else
      {
        iface_id = (* (dss_iface_id_type *) param_val_ptr);
        if (session_mgr_ptr->is_net_policy_set)
        {
          /* Netpolicy or iface id is already set */
          LOG_MSG_ERROR_0 ("Net policy or iface id already set");
          *dss_errno = DS_EFAULT;
        }
        else
        {
          session_mgr_ptr->is_net_policy_set = TRUE;
          session_mgr_ptr->net_policy.iface.kind = DSS_IFACE_ID;
          session_mgr_ptr->net_policy.iface.info.id = iface_id;
          retval = DSS_SUCCESS;
        }
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_ROUTEABLE_IFACE_MODE:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(boolean) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else if (!session_mgr_ptr->is_net_policy_set)
      {
        /* Net policy or iface id not set */
        LOG_MSG_ERROR_0 ("Net policy/iface id not set");
        *dss_errno = DS_EFAULT;
      }
      else if (session_mgr_ptr->net_policy.iface.kind != DSS_IFACE_ID)
      {
        LOG_MSG_ERROR_0 ("Iface id not set in net policy");
        *dss_errno = DS_EFAULT;
      }
      else
      {
        session_mgr_ptr->net_policy.is_routeable =
          (* (boolean *) param_val_ptr);
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_NET_POLICY:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(dss_net_policy_info_type) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else if (session_mgr_ptr->is_net_policy_set)
      {
        /* Net policy or iface id is already set */
        LOG_MSG_ERROR_0 ("Net policy or iface id already set");
        *dss_errno = DS_EFAULT;
      }
      else
      {
        session_mgr_ptr->is_net_policy_set = TRUE;
        memscpy (&session_mgr_ptr->net_policy, param_len,
          param_val_ptr, param_len);
        LOG_MSG_INFO1_1 ("Net policy was set, iface kind is %d",
                         session_mgr_ptr->net_policy.iface.kind);
        if (DSS_IFACE_ID == session_mgr_ptr->net_policy.iface.kind)
        {
           LOG_MSG_INFO1_1 ("Iface id is %d",
                            session_mgr_ptr->net_policy.iface.info.id);
        }
        else
        {
           LOG_MSG_INFO1_1 ("Iface name is %d",
                            session_mgr_ptr->net_policy.iface.info.name);
        }
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_USE_CACHE_RESULTS:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(boolean) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else
      {
        session_mgr_ptr->use_cache_results = (* (boolean *) param_val_ptr);
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_ADD_TO_CACHE:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(boolean) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else
      {
        session_mgr_ptr->add_to_cache =(* (boolean *) param_val_ptr);
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_RECURSION_DESIRED:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(boolean) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else
      {
        session_mgr_ptr->is_recursion_desired = (* (boolean *) param_val_ptr);
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_LINEAR_BACKOFF_RETRY_INFO:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(dss_dns_linear_backoff_retry_info_type) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else
      {
        if (DSS_DNS_DEFAULT_MAX_QUERY_RETRIES <
          ((dss_dns_linear_backoff_retry_info_type *)param_val_ptr)->max_retries)
        {
          LOG_MSG_ERROR_2 ("max_retries is larger the MAX supported %d > %d",
                           ((dss_dns_linear_backoff_retry_info_type*)param_val_ptr)->max_retries,
                           DSS_DNS_DEFAULT_MAX_QUERY_RETRIES);
          *dss_errno = DS_EFAULT;
        }
        else
        {
          memscpy (&session_mgr_ptr->linear_backoff_retry_parameters_info,
            param_len, param_val_ptr, param_len);
          retval = DSS_SUCCESS;
        }
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_CLIENT_SPECIFIED_SERVERS:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(dss_dns_client_specified_servers_enum_type) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else
      {
        session_mgr_ptr->client_specified_servers_behavior =(* (dss_dns_client_specified_servers_enum_type *) param_val_ptr);
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    default:
      LOG_MSG_ERROR_1 ("Invalid param name %d", param_name);
      *dss_errno = DS_EFAULT;
      retval = DSS_ERROR;
  }

  return retval;
} /* dss_dns_set_config_params() */


/*===========================================================================
FUNCTION  DSS_DNS_GET_CONFIG_PARAMS()

DESCRIPTION
  This function gets a specified configuration parameter for a given session.

PARAMETERS
  session_handle    - Identifies the session.
  param_name        - Name of the configuration parameter to get.
  param_val_ptr     - Value of the parameter to be set.
  param_len         - Identifies the length of the param_val_ptr that
                      can be filled in.
  dss_errno             - Error code in case of error.

RETURN VALUE

  DSS_SUCCESS on success: param_val_ptr contains the result and param_len
  contains the length of the results.
  DSS_ERROR on failure and sets error code in dss_errno.

  dss_errno values
  ------------
  DS_EBADF    - Invalid session handle.
  DS_EFAULT   - Invalid arguments.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16  dss_dns_get_config_params
(
  dss_dns_session_mgr_handle_type   session_handle,
  dss_dns_config_params_enum_type   param_name,
  void                            * param_val_ptr,
  uint32                            param_len,
  int16                           * dss_errno
)
{
  ps_dnsi_session_mgr_cb_type   * session_mgr_ptr = NULL;
  int16                            retval = DSS_ERROR;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("dss_dns_get_config_params(): "
    "Session %d, caller 0x%p",
    session_handle, caller_ptr);
  /*-------------------------------------------------------------------------
    Validate arguments.
  -------------------------------------------------------------------------*/
  if (NULL == dss_errno)
  {
    LOG_MSG_ERROR_0 ("NULL errno");
    return DSS_ERROR;
  }

  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);
  if ((NULL == session_mgr_ptr) ||
      (!dss_dns_session_mgr_is_handle_valid(session_handle)))
  /* the check if the pointer is NULL is actually included in the second check,
     and was added just to avoid KW error */
  {
    LOG_MSG_ERROR_1 ("Invalid session handle %d", session_handle);
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }

  if (NULL == param_val_ptr)
  {
    LOG_MSG_ERROR_0 ("Null param val ptr");
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if (DSS_DNS_CONFIG_PARAMS_MAX <= param_name)
  {
    LOG_MSG_ERROR_1 ("Invalid param name %d", param_name);
    *dss_errno = DS_EFAULT;
    return retval;
  }

  switch (param_name)
  {
    case DSS_DNS_CONFIG_PARAMS_IFACE_ID:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(dss_iface_id_type) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else if (! ((session_mgr_ptr->is_net_policy_set) && 
                  (DSS_IFACE_ID == session_mgr_ptr->net_policy.iface.kind)) )
      {
        LOG_MSG_ERROR_0 ("Iface id not set");
        *dss_errno = DS_EFAULT;
      }
      else
      {
        (* (dss_iface_id_type *) param_val_ptr) =
          session_mgr_ptr->net_policy.iface.info.id;
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_ROUTEABLE_IFACE_MODE:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(boolean) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else if (!session_mgr_ptr->is_net_policy_set)
      {
        LOG_MSG_ERROR_0 ("Net policy is not set");
        *dss_errno = DS_EFAULT;
      }
      else
      {
        (* (boolean *) param_val_ptr) =
          session_mgr_ptr->net_policy.is_routeable;
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_NET_POLICY:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(dss_net_policy_info_type) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else if (!session_mgr_ptr->is_net_policy_set)
      {
        LOG_MSG_ERROR_0 ("Net policy is not set");
        *dss_errno = DS_EFAULT;
      }
      else
      {
        memscpy (param_val_ptr, param_len, 
          &(session_mgr_ptr->net_policy), param_len);
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_USE_CACHE_RESULTS:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(boolean) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else
      {
        (* (boolean *) param_val_ptr) = session_mgr_ptr->use_cache_results;
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_ADD_TO_CACHE:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(boolean) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else
      {
        (* (boolean *) param_val_ptr) = session_mgr_ptr->add_to_cache;
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_RECURSION_DESIRED:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(boolean) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else
      {
        (* (boolean *) param_val_ptr) = session_mgr_ptr->is_recursion_desired;
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;
  
    case DSS_DNS_CONFIG_PARAMS_LINEAR_BACKOFF_RETRY_INFO:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(dss_dns_linear_backoff_retry_info_type) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else
      {
        memscpy (param_val_ptr, param_len,
          &(session_mgr_ptr->linear_backoff_retry_parameters_info),
          param_len);
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_DNS_CONFIG_PARAMS_CLIENT_SPECIFIED_SERVERS:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if (sizeof(dss_dns_client_specified_servers_enum_type) != param_len)
      {
        LOG_MSG_ERROR_1 ("Invalid param length for %d", param_name);
        *dss_errno = DS_EFAULT;
      }
      else
      {
        (* (dss_dns_client_specified_servers_enum_type *) param_val_ptr) = 
                                       session_mgr_ptr->client_specified_servers_behavior;
        retval = DSS_SUCCESS;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;
  
    default:
      LOG_MSG_ERROR_1 ("Invalid param name %d", param_name);
      *dss_errno = DS_EFAULT;
  }
  return retval;
} /* dss_dns_get_config_params() */



/*===========================================================================
FUNCTION  DSS_DNS_DELETE_SESSION

DESCRIPTION
  This function deletes a session specified by the session handle.
  This API is an asynchronous API. The caller gets a DSS_SUCCESS
  immediately whereas the actual delete happens in the command handler.

PARAMETERS
  session_handle  - Session identifier
  dss_errno           - Error code in case of error.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on failure and sets the error code in dss_errno.

  dss_errno values
  ------------
  DS_EBADF       - Wrong session handle

DEPENDENCIES
  None.

SIDE EFFECTS
  All the active queries associated with the session will be silently
  dropped without notifying the application.
===========================================================================*/
int16 dss_dns_delete_session
(
  dss_dns_session_mgr_handle_type   session_handle,
  int16                           * dss_errno
)
{
  ps_dnsi_session_mgr_cb_type     * session_mgr_ptr = NULL;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("dss_dns_delete_session(): "
    "Session %d, caller 0x%p",
    session_handle, caller_ptr);

  /*-------------------------------------------------------------------------
    Validate arguments.
  -------------------------------------------------------------------------*/
  if (NULL == dss_errno)
  {
    LOG_MSG_ERROR_0 ("NULL errno");
    return DSS_ERROR;
  }

  /*------------------------------------------------------------------------
    The critical section ensures that the delete API is blocked if we are
    in the middle of invoking the application callback function in the
    PS task.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);

  if ((NULL == session_mgr_ptr) ||
      (!dss_dns_session_mgr_is_handle_valid(session_handle)))
  /* the check if the pointer is NULL is actually included in the second check,
     and was added just to avoid KW error */
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_0 ("Invalid session handle");
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }

  /*------------------------------------------------------------------------
    If the delete API is already processed then session_deleted flag ensures
    that the application callback is not called.
  -------------------------------------------------------------------------*/
  session_mgr_ptr->session_deleted = TRUE;

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*------------------------------------------------------------------------
    Posting a command to PS ensures that delete session and access to control
    blocks happens in PS task context.
  -------------------------------------------------------------------------*/
  ps_send_cmd(PS_DNS_DELETE_SESSION_CMD, (void *) session_mgr_ptr);

  return DSS_SUCCESS;
}

/*===========================================================================
FUNCTION  DSS_DNS_GET_SERVER_ADDR

DESCRIPTION
  This function get a server address from the DNS servers configured by
  the user for this session. 

PARAMETERS
  session_handle  - Session identifier
  server_index    - Requested server index in the queue
  dns_sa_ptr      - Server address. This is an input and output parameter.
                    Client is responsible for allocating the memory.
                    Client shall also set the ss_family field in 
                    dns_sa_ptr->ss_family. The family will be used to 
                    determine if an IPV6 or IPV4 is requested.                   
  dss_errno       - Error code in case of error.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on failure and sets the error code in dss_errno.

  dss_errno values
  ------------
  DS_EBADF         - Wrong session handle
  DS_EAFNOSUPPORT  - Address family not supported
  DS_EFAULT        - Invalid arguments
  DS_EINVAL        - Invalid operation in case no server exists in this index

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_get_server_addr
(
  dss_dns_session_mgr_handle_type   session_handle,
  uint16                            server_index,
  struct ps_sockaddr_storage      * dns_sa_ptr,
  int16                           * dss_errno  
)
{
  ps_dnsi_session_mgr_cb_type     * session_mgr_ptr = NULL;
  q_type                          * dns_servers_q = NULL;
  int16                             index;
  ps_dnsi_addr_node_type          * addr_node;
  int                               dns_servers_q_size;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate arguments.
  -------------------------------------------------------------------------*/
  if (NULL == dss_errno)
  {
    LOG_MSG_ERROR_0 ("NULL errno");
    return DSS_ERROR;
  }
  
  if (NULL == dns_sa_ptr)
  {
    LOG_MSG_ERROR_0 ("NULL dns_sa_ptr");
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }
  
  if ( (DSS_AF_INET != dns_sa_ptr->ps_ss_family ) &&
       (DSS_AF_INET6 != dns_sa_ptr->ps_ss_family ) )
  {
    LOG_MSG_ERROR_1 ("Wrong address family %d", dns_sa_ptr->ps_ss_family);
    *dss_errno = DS_EAFNOSUPPORT;
    return DSS_ERROR;
  }

  /*------------------------------------------------------------------------
    The critical section ensures no one change the queue during our search
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);

  if (NULL == session_mgr_ptr)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_0 ("Invalid session handle");
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }
  dns_servers_q = (DSS_AF_INET == dns_sa_ptr->ps_ss_family )?
                          &(session_mgr_ptr->dns_ipv4_servers_q):
                          &(session_mgr_ptr->dns_ipv6_servers_q);
  dns_servers_q_size = q_cnt(dns_servers_q);
  if ( server_index >= dns_servers_q_size )
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_3 ("Invalid DNS server index %d.Family %d size:d",
                     server_index,
                     dns_sa_ptr->ps_ss_family,
                     dns_servers_q_size);
    *dss_errno = DS_EINVAL;
    return DSS_ERROR;
  }
  //Get to index server_index
  addr_node = (ps_dnsi_addr_node_type *)q_check(dns_servers_q);
  if ( NULL == addr_node )
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_0 ("Servers queue is empty");
    *dss_errno = DS_EINVAL;
    return DSS_ERROR;
  }
  for ( index = 0; index < server_index; index++ )
  {
    addr_node = q_next( dns_servers_q, &(addr_node->link) );
    if ( NULL == addr_node )
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_0 ("addr_node is NULL");
      *dss_errno = DS_EINVAL;
      return DSS_ERROR;
    }
  }
  //copy the addr to dns_sa_ptr
  memscpy( dns_sa_ptr, sizeof( struct ps_sockaddr_storage ),
    &(addr_node->dns_serv_addr), sizeof( struct ps_sockaddr_storage ));
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return DSS_SUCCESS;
}

/*===========================================================================
FUNCTION  DSS_DNS_PRIV_ADD_SERVER_ADDR()

DESCRIPTION
  This function adds a DNS server address to the list of the 
  session's DNS servers.
  The DNS servers are used for sending queries according to the order
  they were added.
  The first server that was added is the first to be used.
  NOTES:
  1. In addition to the DNS servers in this list there are servers which are
  defined by low layers. Use the session's parameter CLIENT_SPECIFIED_SERVERS
  to configure if the low layer servers should be used and if they should be 
  used before or after the session's DNS servers.

  2. There is no check if such server was already added. In such case, the same server
  will be used twice (treated as a different server).

PARAMETERS
  session_handle     -  Session handle.
  dns_sa_ptr         -  DNS server to add to the session's DNS servers.
  dss_errno          -  Error code.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on error.

  dss_errno values
  ----------------
  DS_EFAULT       - Invalid arguments.
  DS_EAFNOSUPPORT - Address family is not IPV4 or IPV6
  DS_ENOMEM       - No memory for adding the server.

DEPENDENCIES
None.

SIDE EFFECTS
None.
===========================================================================*/
int16 dss_dns_priv_add_server_addr
(
  dss_dns_session_mgr_handle_type     session_handle,
  struct ps_sockaddr_storage        * dns_sa_ptr,
  int16                             * dss_errno
)
{
  ps_dnsi_session_mgr_cb_type     * session_mgr_ptr = NULL;
  q_type                          * dns_servers_q = NULL;
  ps_dnsi_addr_node_type          * addr_node;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate arguments.
  -------------------------------------------------------------------------*/
  if (NULL == dss_errno)
  {
    LOG_MSG_ERROR_0 ("NULL errno");
    return DSS_ERROR;
  }
  
  if (NULL == dns_sa_ptr)
  {
    LOG_MSG_ERROR_0 ("NULL dns_sa_ptr");
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }
  
  if ( (DSS_AF_INET != dns_sa_ptr->ps_ss_family ) &&
       (DSS_AF_INET6 != dns_sa_ptr->ps_ss_family ) )
  {
    LOG_MSG_ERROR_1 ("Invalid family type %d", dns_sa_ptr->ps_ss_family);
    *dss_errno = DS_EAFNOSUPPORT;
    return DSS_ERROR;
  }

  /*------------------------------------------------------------------------
    The critical section ensures no one change the queue during our operation
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);

  if (NULL == session_mgr_ptr)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_0 ("Invalid session handle");
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }
  dns_servers_q = (DSS_AF_INET == dns_sa_ptr->ps_ss_family )?
                          &(session_mgr_ptr->dns_ipv4_servers_q):
                          &(session_mgr_ptr->dns_ipv6_servers_q);
  
  /* Allocate memory for the new server */
  addr_node = ps_mem_get_buf(PS_MEM_DNSI_SESSION_DNS_SERVERS_TYPE);

  if(NULL == addr_node)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    *dss_errno = DS_ENOMEM;
    return DSS_ERROR;
  }

  memset(addr_node, 0, sizeof(ps_dnsi_addr_node_type));
  (void) q_link( addr_node, &addr_node->link );

  /*Store dns_sa_ptr*/
  memscpy( &(addr_node->dns_serv_addr), sizeof( struct ps_sockaddr_storage ),
    dns_sa_ptr, sizeof( struct ps_sockaddr_storage ));
  
  /*Add to the queue*/
  q_put(dns_servers_q, &( addr_node->link ));

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  LOG_MSG_INFO1_0("Added DNS server to Session's DNS servers");

  return DSS_SUCCESS;
}

/*===========================================================================
FUNCTION  DSS_DNS_PRIV_CLEAR_SERVER_QUEUE

DESCRIPTION
  This function clears the DNS servers queue (IPV4 and IPV6).

PARAMETERS
  session_handle     -  Session handle.
  dss_errno          -  Error code.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on error.

  dss_errno values
  ----------------
  DS_EBADF       - Invalid session handle.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_priv_clear_server_queue
(
  dss_dns_session_mgr_handle_type      session_handle,
  int16                              * dss_errno
)
{
  ps_dnsi_session_mgr_cb_type     * session_mgr_ptr = NULL;
  ps_dnsi_addr_node_type          * addr_node;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Validate arguments.
  -------------------------------------------------------------------------*/
  if (NULL == dss_errno)
  {
    LOG_MSG_ERROR_0 ("NULL errno");
    return DSS_ERROR;
  }

  /*------------------------------------------------------------------------
  Clean the DNS server queues
  -------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
  The critical section ensures no one change the queue during our operation
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);

  if (NULL == session_mgr_ptr)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_0 ("Invalid session handle");
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }

  while( NULL != (addr_node = q_get( &session_mgr_ptr->dns_ipv4_servers_q ) ) )
  {
    PS_MEM_FREE(addr_node);
  }

  while( NULL != (addr_node = q_get( &session_mgr_ptr->dns_ipv6_servers_q ) ) )
  {
    PS_MEM_FREE(addr_node);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return DSS_SUCCESS;

}

/*===========================================================================
FUNCTION  DSS_DNS_SESSION_MGR_GET_SERVERS_COUNT

DESCRIPTION
  Get the count of servers in the session's DNS servers queue.
  The count of the IPV4 queue or IPV6 is returned based on the 
  address family parameter.

PARAMETERS
  session_handle     -  Session handle.
  addr_family        -  Address family. 
  count              -  Returns the count.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on error.

  dss_errno values
  ----------------
  DS_EFAULT          - Invalid arguments.
  DS_EAFNOSUPPORT    - Address family is not IPV4 or IPV6.
  DS_EBADF           - Invalid session handle.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_session_mgr_get_servers_count
(  
  dss_dns_session_mgr_handle_type   session_handle,
  uint16                            addr_family,
  uint16                          * count,
  int16                           * dss_errno
)
{
  ps_dnsi_session_mgr_cb_type     * session_mgr_ptr = NULL;
  q_type                          * dns_servers_q = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Validate arguments.
  -------------------------------------------------------------------------*/
  if (NULL == dss_errno)
  {
    LOG_MSG_ERROR_0 ("NULL errno");
    return DSS_ERROR;
  }

  if (NULL == count)
  {
    LOG_MSG_ERROR_0 ("NULL count");
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if ( (DSS_AF_INET != addr_family ) &&
       (DSS_AF_INET6 != addr_family ) )
  {
    LOG_MSG_ERROR_1 ("Invalid family type %d", addr_family);
    *dss_errno = DS_EAFNOSUPPORT;
    return DSS_ERROR;
  }

  /*------------------------------------------------------------------------
  The critical section ensures no one change the queue during our operation
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);

  if (NULL == session_mgr_ptr)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_0 ("Invalid session handle");
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }
  dns_servers_q = (DSS_AF_INET == addr_family )?
                      &(session_mgr_ptr->dns_ipv4_servers_q):
                      &(session_mgr_ptr->dns_ipv6_servers_q);

  *count = (uint16)q_cnt(dns_servers_q);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return DSS_SUCCESS;
}


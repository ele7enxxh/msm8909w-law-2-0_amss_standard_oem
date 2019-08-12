/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S

                        A T   C O M M A N D

                        D Y N A M I C   L O A D I N G

GENERAL DESCRIPTION
  This module contains functions that are used for dynamic loading of
  AT commands.

EXTERNALIZED FUNCTIONS

  dsatdl_init
     This function performs initializations for dynamic loading of ATCoP DLL.

  dsatdl_lock
     This function locks the access of DL info.

  dsatdl_unlock
     This function unlocks the access of DL info.

  dsatdl_check_symbol_addr
     This function checks a symbol address and calls ERR_FATAL if it's invalid.

  dsatdl_validate_symbol_addr
     This function validates a symbol address and returns if it's valid.

  dsatdl_cmd_handler
     This function is used for handling ATCoP dynamic loading commands.

  dsatdl_get_status
     This function returns the dynamic loading status of ATCoP DLL.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2014 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatdl_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
04/06/15   sc      Initialized symbol table when ATCoP is disabled.
06/27/14   tk      Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "dsatdl.h"

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
#include "dlfcn.h"
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
typedef enum dsatdl_error_e
{
  DSAT_DL_ERROR_NONE = -1,
  DSAT_DL_ERROR_LOAD,
  DSAT_DL_ERROR_UNLOAD,
  DSAT_DL_ERROR_SYMBOL,
  DSAT_DL_ERROR_MAX
} dsatdl_error_e_type;

typedef struct dsatdl_info_s
{
  rex_crit_sect_type                crit_sect;
  void                             *handle;
  const dsatdl_symbol_table_s_type *symbol_table_ptr;
  dsatdl_status_e_type              status;
  dsatdl_error_e_type               error;
  uint8                             block_unload_cntr;
  boolean                           is_unload_pending;
} dsatdl_info_s_type;
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

#define DSAT_DL_BLOCK_UNLOAD_CNTR_MAX DS3G_SIOLIB_PORT_MAX

/*-------------------------------------------------------------------------
            Import External Variables
-------------------------------------------------------------------------*/
#ifdef FEATURE_DSAT_DYNAMIC_LOADING
#ifdef FEATURE_ETSI_SMS
extern const dsatsms_msg_handler_tab_type dsatetsisms_msg_handler_table_ex;
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
extern const dsatsms_msg_handler_tab_type dsat707sms_msg_handler_table_ex;
#endif /* FEATURE_CDMA_SMS */

#else

extern const dsatdl_symbol_table_s_type dsatdl_symbol_table;
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

/*-------------------------------------------------------------------------
            Local Variables
-------------------------------------------------------------------------*/

dsatdl_vtable_s_type dsatdl_vtable;

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
LOCAL dsatdl_info_s_type dsatdl_info;

LOCAL const dsatdl_symbol_table_entry_s_type dsatdl_symbol_table_entries_ex[] =
{
  { (void *) &async_event_table_ex,
    (void **) &dsatdl_vtable.async_event_table_ptr },

  { (void *) &pbm_cb_handler_table_ex,
    (void **) &dsatdl_vtable.pbm_cb_handler_table_ptr },

#ifdef FEATURE_ETSI_SMS
  { (void *) &dsatetsisms_msg_handler_table_ex,
    (void **) &dsatdl_vtable.dsatetsisms_msg_handler_table_ptr },
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

#ifdef FEATURE_CDMA_SMS
  { (void *) &dsat707sms_msg_handler_table_ex,
    (void **) &dsatdl_vtable.dsat707sms_msg_handler_table_ptr },
#endif /* FEATURE_CDMA_SMS */
};

const dsatdl_symbol_table_s_type dsatdl_symbol_table_ex =
{
  dsatdl_symbol_table_entries_ex, ARR_SIZE(dsatdl_symbol_table_entries_ex)
};
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

/*-------------------------------------------------------------------------
    Local Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION  DSATDL_SYMBOL_TABLE_CTOR

DESCRIPTION
  This function constructs the symbol table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatdl_symbol_table_ctor
(
  const dsatdl_symbol_table_s_type *symbol_table_ptr
)
{
  size_t idx;

  ASSERT(symbol_table_ptr);

  for (idx = 0; idx < symbol_table_ptr->size; idx++)
  {
    *symbol_table_ptr->base_ptr[idx].vptr_addr = symbol_table_ptr->base_ptr[idx].addr;
  }

  return;
} /* dsatdl_symbol_table_ctor( ) */

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
/*===========================================================================

FUNCTION  DSATDL_SYMBOL_TABLE_DTOR

DESCRIPTION
  This function destructs the symbol table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatdl_symbol_table_dtor
(
  const dsatdl_symbol_table_s_type *symbol_table_ptr
)
{
  size_t idx;

  ASSERT(symbol_table_ptr);

  for (idx = 0; idx < symbol_table_ptr->size; idx++)
  {
    *symbol_table_ptr->base_ptr[idx].vptr_addr = NULL;
  }

  return;
} /* dsatdl_symbol_table_dtor( ) */

/*===========================================================================
FUNCTION  DSATDL_LOG

DESCRIPTION
  This function logs DL info.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatdl_log(void)
{
  DS_AT_MSG5_MED("DL Info: handle = %p, symbol_table_ptr = %p, status = %d, "
                 "block_unload_cntr = %d, is_unload_pending = %d",
                 dsatdl_info.handle,
                 dsatdl_info.symbol_table_ptr,
                 dsatdl_info.status,
                 dsatdl_info.block_unload_cntr,
                 dsatdl_info.is_unload_pending);

  if (DSAT_DL_ERROR_NONE != dsatdl_info.error)
  {
    DS_AT_MSG1_ERROR("DL Info: error = %d", dsatdl_info.error);
  }

  return;
} /* dsatdl_log() */

/*===========================================================================
FUNCTION  DSATDL_ERROR

DESCRIPTION
  This function is a wrapper for dlerror().

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatdl_error
(
  dsatdl_error_e_type error
)
{
  char       *dl_error_str   = dlerror();
  const char *dl_error_const = "dlerror returned NULL";

  if (NULL == dl_error_str)
  {
    dl_error_str = (char *) dl_error_const;
  }

  DS_AT_MSG_SPRINTF_2_ERROR("DL Error %d: %s", error, dl_error_str);

  if (DSAT_DL_ERROR_LOAD != error)
  {
    ASSERT(0);
  }

  return;
} /* dsatdl_error() */

/*===========================================================================
FUNCTION  DSATDL_LOAD

DESCRIPTION
  This function is used for dynamic loading of ATCoP DLL.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatdl_load(void)
{
  if (NULL == dsatdl_info.handle)
  {
    dlerror();

    dsatdl_info.handle = dlopen("/atcop.so", 2);

    if (NULL != dsatdl_info.handle)
    {
      dsatdl_info.symbol_table_ptr = dlsym(dsatdl_info.handle,
                                           "dsatdl_symbol_table");

      if (NULL != dsatdl_info.symbol_table_ptr)
      {
        dsatdl_symbol_table_ctor(dsatdl_info.symbol_table_ptr);

        DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatcmdp_init_fp);
        dsatdl_vtable.dsatcmdp_init_fp();

        dsatdl_info.status = DSAT_DL_STATUS_LOADED;
        dsatdl_info.error  = DSAT_DL_ERROR_NONE;
      }
      else
      {
        dsatdl_info.status = DSAT_DL_STATUS_ERROR;
        dsatdl_info.error  = DSAT_DL_ERROR_SYMBOL;

        dsatdl_error(DSAT_DL_ERROR_SYMBOL);
      }
    }
    else
    {
      dsatdl_info.status = DSAT_DL_STATUS_ERROR;
      dsatdl_info.error  = DSAT_DL_ERROR_LOAD;

      dsatdl_error(DSAT_DL_ERROR_LOAD);
    }
  }

  return;
} /* dsatdl_load() */

/*===========================================================================
FUNCTION  DSATDL_UNLOAD

DESCRIPTION
  This function is used for dynamic unloading of ATCoP DLL.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatdl_unload(void)
{
  int dlclose_ret;

  if (NULL != dsatdl_info.handle)
  {
    DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatcmdp_deinit_fp);
    dsatdl_vtable.dsatcmdp_deinit_fp();

    dsatdl_symbol_table_dtor(dsatdl_info.symbol_table_ptr);

    dlerror();

    dlclose_ret = dlclose(dsatdl_info.handle);

    if (0 == dlclose_ret)
    {
      dsatdl_info.handle = NULL;
      dsatdl_info.status = DSAT_DL_STATUS_UNLOADED;
      dsatdl_info.error  = DSAT_DL_ERROR_NONE;

      dsatdl_symbol_table_ctor(&dsatdl_symbol_table_ex);
    }
    else
    {
      DS_AT_MSG1_ERROR("dlclose returned %d", dlclose_ret);

      dsatdl_info.status = DSAT_DL_STATUS_ERROR;
      dsatdl_info.error  = DSAT_DL_ERROR_UNLOAD;

      dsatdl_error(DSAT_DL_ERROR_UNLOAD);
    }
  }

  return;
} /* dsatdl_unload() */
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

/*-------------------------------------------------------------------------
    Internal Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION  DSATDL_INIT

DESCRIPTION
  This function performs initializations for dynamic loading of ATCoP DLL.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatdl_init(void)
{
  memset(&dsatdl_vtable, 0x0, sizeof(dsatdl_vtable_s_type));

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
  dsatdl_symbol_table_ctor(&dsatdl_symbol_table_ex);

  rex_init_crit_sect(&dsatdl_info.crit_sect);

  dsatdl_info.handle            = NULL;
  dsatdl_info.symbol_table_ptr  = NULL;
  dsatdl_info.status            = DSAT_DL_STATUS_UNLOADED;
  dsatdl_info.error             = DSAT_DL_ERROR_NONE;
  dsatdl_info.block_unload_cntr = 0;
  dsatdl_info.is_unload_pending = FALSE;
#else
  dsatdl_symbol_table_ctor(&dsatdl_symbol_table);
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

  return;
} /* dsatdl_init() */

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
/*===========================================================================
FUNCTION  DSATDL_LOCK

DESCRIPTION
  This function locks the access of DL info.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatdl_lock(void)
{
  rex_enter_crit_sect(&dsatdl_info.crit_sect);

  return;
} /* dsatdl_lock() */

/*===========================================================================
FUNCTION  DSATDL_UNLOCK

DESCRIPTION
  This function unlocks the access of DL info.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatdl_unlock(void)
{
  rex_leave_crit_sect(&dsatdl_info.crit_sect);

  return;
} /* dsatdl_unlock() */
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

/*===========================================================================
FUNCTION  DSATDL_CHECK_SYMBOL_ADDR

DESCRIPTION
  This function checks a symbol address and calls ERR_FATAL if it's invalid.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatdl_check_symbol_addr
(
  void **vptr_addr
)
{
  if (NULL == *vptr_addr)
  {
    ERR_FATAL("Invalid symbol address: vptr_addr = %p", vptr_addr, 0, 0);
  }

  return;
} /* dsatdl_check_symbol_addr() */

/*===========================================================================
FUNCTION  DSATDL_VALIDATE_SYMBOL_ADDR

DESCRIPTION
  This function validates a symbol address and returns if it's valid.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatdl_validate_symbol_addr
(
  void **vptr_addr
)
{
  boolean is_valid = TRUE;

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
  if (DSAT_DL_STATUS_LOADED == dsatdl_info.status)
  {
    dsatdl_check_symbol_addr(vptr_addr);
  }
  else if (NULL == *vptr_addr)
  {
    is_valid = FALSE;
  }
#else
  dsatdl_check_symbol_addr(vptr_addr);
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

  return is_valid;
} /* dsatdl_validate_symbol_addr() */

/*-------------------------------------------------------------------------
    Public Function Definitions:
-------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
/*===========================================================================
FUNCTION  DSATDL_CMD_HANDLER

DESCRIPTION
  This function is used for handling ATCoP dynamic loading commands.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatdl_cmd_handler
(
  dsatdl_cmd_e_type cmd
)
{
  boolean is_valid_cmd = TRUE;

  /***************************************************************************
      Handle the commands based on various sequences of commands after
      initialization: -

      1) Below sequence is a simple scenario without an AT command or
         a data call in processing
          DSAT_DL_CMD_LOAD           -> Perform loading
          DSAT_DL_CMD_UNLOAD         -> Perform unloading

      2) Below sequence is a scenario with an AT command or a data call in
         processing between loading and unloading
          DSAT_DL_CMD_LOAD           -> Perform loading
          DSAT_DL_CMD_BLOCK_UNLOAD   -> Block unloading
          DSAT_DL_CMD_UNBLOCK_UNLOAD -> Unblock unloading
          DSAT_DL_CMD_UNLOAD         -> Perform unloading

      3) Below sequence is a scenario with unloading when an AT command or
         a data call in processing
          DSAT_DL_CMD_LOAD           -> Perform loading
          DSAT_DL_CMD_BLOCK_UNLOAD   -> Block unloading
          DSAT_DL_CMD_UNLOAD         -> Unloading is pending
          DSAT_DL_CMD_UNBLOCK_UNLOAD -> Unblock and perform unloading

      4) Below sequence is a scenario with unloading and loading when an
         AT command or a data call in processing
          DSAT_DL_CMD_LOAD           -> Perform Loading
          DSAT_DL_CMD_BLOCK_UNLOAD   -> Unloading is blocked
          DSAT_DL_CMD_UNLOAD         -> Unloading is pending
          DSAT_DL_CMD_LOAD           -> Unloading is no more pending
          DSAT_DL_CMD_UNBLOCK_UNLOAD -> Unblock unloading

   ***************************************************************************/

  DSAT_DL_LOCK();

  switch (cmd)
  {
    case DSAT_DL_CMD_LOAD:
      if (DSAT_DL_STATUS_LOADED != dsatdl_info.status)
      {
        dsatdl_log();

        if (FALSE == dsatdl_info.is_unload_pending)
        {
          dsatdl_load();
        }
        else
        {
          dsatdl_info.is_unload_pending = FALSE;
        }

        dsatdl_log();
      }
      break;

    case DSAT_DL_CMD_UNLOAD:
      if (DSAT_DL_STATUS_UNLOADED != dsatdl_info.status)
      {
        dsatdl_log();

        if (0 == dsatdl_info.block_unload_cntr)
        {
          dsatdl_unload();
        }
        else
        {
          dsatdl_info.is_unload_pending = TRUE;
        }

        dsatdl_log();
      }
      break;

    case DSAT_DL_CMD_BLOCK_UNLOAD:
      if (dsatdl_info.block_unload_cntr < DSAT_DL_BLOCK_UNLOAD_CNTR_MAX)
      {
        dsatdl_info.block_unload_cntr++;
      }
      else
      {
        dsatdl_log();

        is_valid_cmd = FALSE;
      }
      break;

    case DSAT_DL_CMD_UNBLOCK_UNLOAD:
      if (dsatdl_info.block_unload_cntr > 0)
      {
        dsatdl_info.block_unload_cntr--;

        if (TRUE == dsatdl_info.is_unload_pending)
        {
          dsatdl_log();

          dsatdl_info.is_unload_pending = FALSE;

          dsatdl_unload();

          dsatdl_log();
        }
      }
      else
      {
        dsatdl_log();

        is_valid_cmd = FALSE;
      }
      break;

    default:
      is_valid_cmd = FALSE;
      break;
  }

  DSAT_DL_UNLOCK();

  if (FALSE == is_valid_cmd)
  {
    ERR_FATAL("Invalid DL command %d", cmd, 0, 0);
  }

  return;
} /* dsatdl_cmd_handler() */

/*===========================================================================
FUNCTION  DSATDL_GET_STATUS

DESCRIPTION
  This function returns the dynamic loading status of ATCoP DLL.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
dsatdl_status_e_type dsatdl_get_status(void)
{
  dsatdl_status_e_type status;

  DSAT_DL_LOCK();

  status = dsatdl_info.status;

  DSAT_DL_UNLOCK();

  return status;
} /* dsatdl_get_status() */
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */


#ifndef MCFG_NV_API_H
#define MCFG_NV_API_H
/*==============================================================================

        MODEM CFG   -   N O N - V O L A T I L E   S T O R A G E  API

GENERAL DESCRIPTION
  Simple APIs to access ModemCfg-specific NV settings.  Supports both new-style 
  Modem Cfg specific NV items and old-style NV items, though it is recommended that 
  both exisiting and new code accessing old-style items use the legacy NV API
  when possible.  

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/api/mcfg_nv_api.h#1 $
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
03/24/10 sbt   Initial creation

==============================================================================*/

#include "fs_public.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
 
==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/
/* Maximum size of an NV item - applications with requirements for more
 * should use EFS regular file APIs directly */
#define MCFG_NV_ITEM_SIZE_MAX (128)

/* Maximum length of an EFS path for an NV item, including null terminator.
 * This MUST guarantee enough room for the directory + item + index + muxing */
#define MCFG_NV_MAX_EFS_PATH_LEN  (72)

/*==============================================================================
  Typedefs
==============================================================================*/

/* Enum for new-style MCFG NV items (used in API calls) */
typedef enum mcfg_nv_item_e {
  MCFG_NVI_ITEM_FILES_START  =  10000, /* reserved for internal use only */

  /**************************************************************************\
  **             ---- Instructions for adding new NV items ----             **
  **  1. Add new MCFG_NV_ITEM_* enum value at the end of this list          **
  **  2. Add a comment giving the QXDM ID (or 'pending' if none yet) and    **
  **     data type of the item                                              **
  **  3. Update any existing comments with 'pending' as the QXDM ID         **
  **  4. Add the filename of the new NV item to the mcfg_nv_filenames list  **
  **     located in modem\mcfg\mcfg_proc\mcfg_nv.c                           **
  **  5. If the new item is to be indexed (i.e. an array of NV items), then **
  **     update the table mcfg_nv_indexed_items in mcfg_nv.c                **
  **  6. Add the directory and filename on a new line at the end of the     **
  **     mcfg_nv_init to make sure it gets included in mcfg.conf            **
  **  7. Send the new NV item info to be added to QXDM                      **
  **                                                                        **
  **  A note about indexed/arrayed items: their data structure MUST begin   **
  **  with a uint8 field used to store the index.  This is to maintain      **
  **  consistency with the legacy NV API.                                   **
  \**************************************************************************/

  /* <10001>                                 <QXDM ID> | <data type> */
  MCFG_NV_ITEM_SW_VERSION_I,                 /* pendin)| uint32 */

  /* <10002>                                 <QXDM ID> | <data type> */
  MCFG_NV_ITEM_HW_VERSION_I,                 /* pendin)| uint32 */

  /* <10003> */
  MCFG_NV_ITEM_MUXD_CONSL_PROFILE_DIR_INDEX, /* 65596 | uint8 */

  /* <10004> */
  MCFG_NV_ITEM_MCFG_SW_MUXD_VERSION_I,       /* 65597 | uint32 */

  /* <10005>                                 <QXDM ID> | <data type> */
  MCFG_NV_ITEM_MCFG_SW_PREF,                 /* pendin)| mcfg_pref_s_type */

  /* <10006>                                 <QXDM ID> | <data type> */
  MCFG_NV_ITEM_MCFG_HW_PREF,                 /* pendin)| mcfg_pref_s_type */

  /* <10007>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_TEST_MCFG_INDEXED_PROF,       /* pending | mcfg_test_indexed_type */

  /* <10008>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_AUTOSELECT_BY_UIM,            /* 71546  | uint8 */

  /* <10009>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_LAST_AUTOSELECT_ICCID,        /* pending | char[21] */

  /* <10010>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_DEF_CONFIG_SW_VERSION_I,      /* pending | uint32 */

  /* <10011>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_DEF_CONFIG_HW_VERSION_I,      /* pending | uint32 */

  /* <10012>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_DEF_CONFIG_ENABLE_I,          /* pending | uint8 */

  /* <10013>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_DEF_CONFIG_FORCE_LOAD_I,      /* pending | uint8 */

  /* <10014>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_CONFIG_FORCE_LOAD_I,          /* pending | uint8 */

  /* 10015>                                   <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_SEGLOAD_CONFIG_I,             /* pending | segload_config_t */       
  /* Add new items here */
  
  /* <10016>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_LAST_AUTOSELECT_PLMN,         /* pending | plmn_t */

  /* <10017>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_UIM_SLOT_MAPPING,             /* pending | uint8 */

  /* <10018>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_REFRESH_TIMER,                /* pending | uint8 */

  /* <10019>                                 <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_UIM_SLOT_MAPPING_TIMER,       /* pending | uint8 */

  /*<10020>                                  <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_MULTI_MBN_CONFIG_VERSION_I,    /* pending | uint32 */

  /*<10021>                                  <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_SETTING,                      /* pending | uint32 */

   /*<10022>                                  <QXDM ID> |  <data type> */
  MCFG_NV_ITEM_MCFG_POWER_DOWN_REASON,        /* pending | uint32 */

  /* below items reserved for internal use only */
  MCFG_NVI_ITEM_FILES_END, 
  MCFG_NVI_ITEM_BIG = 0x7FFFFFFF
} mcfg_nv_item_e_type;

/* 
** Status enum returned by mcfg_nv_read & mcfg_nv_write. 
** Applications wishing to compare against NV status enum settings can use 
** a uint32 instead of  mcfg_nv_status_e_type, for example:
**   uint32 retval = mcfg_nv_write(...);  
**   if (retval == NV_DONE_S) ...
** This behavior is supported but not recommended for new code.
*/
typedef enum mcfg_nv_status_e {

  /* The values of the following enums are equal in value and (mostly) meaning
   * of the listed nv_stat_enum_type */

  MCFG_NV_STATUS_OK = 0,
    /* Request completed successfully.  Equivalent to NV_DONE_S */

  MCFG_NV_STATUS_EFS_ERR = 4,
    /* Internal EFS error.  Equivalent to NV_FAIL_S */

  MCFG_NV_STATUS_NOT_ACTIVE = 5,
    /* NV item file doesn't exist.  Equivalent to NV_NOTACTIVE_S */

  MCFG_NV_STATUS_BAD_ITEM = 6,
    /* NV item number not in allowed range: must be < NV_MAX_I for old items, 
     * or > MCFG_NVI_ITEM_FILES_START and < MCFG_NVI_ITEM_FILES_END for new.
     * Somewhat equivalent to NV_BADPARM_S */

  MCFG_NV_STATUS_UNSUPPORTED = 11,
    /* NV item number not supported by either RUIM or
     * by currently active subscription 
     * Somewhat equivalent to NV_RUIM_NOT_SUPPORTED_S */

  /* The following enums are not included in nv_stat_enum_type */

  MCFG_NV_STATUS_BAD_SIZE = 128,
    /* On read: data size doesn't match current item size
     * On write: data size is > MCFG_NV_ITEM_SIZE_MAX
     * Not equivalent to any nv_stat_enum_type value */

  MCFG_NV_NULL_PTR = 129,
    /* NULL passed as data pointer field to write or read function */

  MCFG_NVI_STATUS_BIG = 0x7FFFFFFF 
    /* Reserved (force enum to 32-bit width) */
} mcfg_nv_status_e_type;

/*==============================================================================
  Variables
==============================================================================*/

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==========================================================================

  FUNCTION mcfg_nv_delete

  DESCRIPTION
    This function deletes an NV item from the EFS.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] NV item to delete: can be nv_items_enum_type or 
                   mcfg_nv_item_e_type
    index     [in] index of the NV item to delete, or 0 if no index
    subs_id   [in] Subscription ID for the Multi-SIM NV item to delete ,
                   or 0 if not a multi-SIM NV (if unsure, use 0)
    
  RETURN VALUE
    MCFG_NV_STATUS_OK on success or if file does not exist
    MCFG_NV_STATUS_BAD_ITEM if item is not in old NV or new MCFG NV range,
        or index is outside defined limit (new-style items only)
    MCFG_NV_STATUS_EFS_ERR on EFS error

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_delete(
  uint32  item,
  uint8   index,
  uint8   subs_id
  );

/*==========================================================================

  FUNCTION mcfg_nv_is_new_indexed

  DESCRIPTION
    Returns whether the new-style MCFG NV item is indexed (i.e. is an array
    of NV items).

  DEPENDENCIES
    None.

  PARAMETERS
    item  [in] NV item to check: can be nv_items_enum_type or 
               mcfg_nv_item_e_type
    
  RETURN VALUE
    TRUE if item is new-style AND it is indexed, otherwise FALSE

  SIDE EFFECTS
    None.

===========================================================================*/
boolean mcfg_nv_is_new_indexed(
  uint32 item
  );

/*==========================================================================

  FUNCTION mcfg_nv_read

  DESCRIPTION
    This function reads an NV item from EFS.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] NV item to read: can be nv_items_enum_type or 
                   mcfg_nv_item_e_type
    index     [in] index of the NV item to read, or 0 if no index
                   (if unsure, use 0)
    subs_id   [in] Subscription ID for the Multi-SIM NV item to read, or 0 if not a multi-SIM NV
                   (if unsure, use 0)
    data_ptr [out] location to store NV item contents
    data_size [in] size of buffer at data_ptr
    
  RETURN VALUE
    MCFG_NV_STATUS_OK on success
    MCFG_NV_STATUS_BAD_ITEM if item is not in old NV or new MCFG NV range
    MCFG_NV_STATUS_NOT_ACTIVE if item doesn't exist
    MCFG_NV_STATUS_BAD_SIZE if item size doesn't match data_size
    MCFG_NV_STATUS_EFS_ERR on EFS error
    MCFG_NV_STATUS_NULL_PTR if data_ptr is null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_read(
  uint32    item,
  uint8     index,
  uint8     subs_id,
  void *    data_ptr,
  fs_size_t data_size
  );

/*==========================================================================

  FUNCTION mcfg_nv_write

  DESCRIPTION
    This function writes an NV item to EFS.  Enforces a maximum data_size
    of MCFG_NV_ITEM_SIZE_MAX.  Applications wishing to save larger files 
    should use EFS regular file APIs directly.

    If the NV item exists and has a different file size than data_size,
    it will be deleted and recreated with length equal to data_size.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] NV item to write: can be nv_items_enum_type or 
                   mcfg_nv_item_e_type
    index     [in] index of the NV item to write, or 0 if no index
    subs_id   [in] Subscription ID for the Multi-SIM NV item to write,
              or 0 if not a multi-SIM NV (if unsure, use 0)
    data_ptr  [in] location to store NV item contents
    data_size [in] number of bytes to write, 
                   must be <= MCFG_NV_ITEM_SIZE_MAX
    
  RETURN VALUE
    MCFG_NV_STATUS_OK on success
    MCFG_NV_STATUS_BAD_ITEM if item is not in old NV or new MCFG NV range
    MCFG_NV_STATUS_BAD_SIZE if data_size > MCFG_NV_ITEM_SIZE_MAX
    MCFG_NV_STATUS_EFS_ERR on EFS error
    MCFG_NV_STATUS_NULL_PTR if data_ptr is null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_write(
  uint32    item,
  uint8     index,
  uint8     subs_id,
  void *    data_ptr,
  fs_size_t data_size
  );

#endif /* MCFG_NV_API_H */


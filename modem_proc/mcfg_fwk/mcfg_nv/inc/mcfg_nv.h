#ifndef MCFG_NV_H
#define MCFG_NV_H
/*==============================================================================

        MODEM CFG   -   N O N - V O L A T I L E   S T O R A G E

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_nv/inc/mcfg_nv.h#1 $
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
03/24/10 sbt   Initial creation

==============================================================================*/

#include "fs_public.h"
#include "mcfg_nv_api.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*===========================================================================

FUNCTION mcfg_nv_efs_update_symlink

DESCRIPTION
  Creates a symlink for the Muxd NV Items or EFS Files
  
DEPENDENCIES
  None.
 
  PARAMETERS
    org_path  [in] Pathname of the file/symlink to be updated for muxd support
 
    index     [in] index of the NV item to write, or 0 if no index
    subs_id   [in] Subscription ID for the Multi-SIM NV item to write,
              or 0 if not a multi-SIM NV (if unsure, use 0)
    size      [in] size of the item 

RETURN VALUE
  mcfg_nv_status_e_type

SIDE EFFECTS

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_efs_update_symlink(char* org_path, uint32 item_size);

/*==========================================================================

  FUNCTION mcfg_nv_efslogging_enabled

  DESCRIPTION
    Checks whether EFS logging is enalbed or not.

  DEPENDENCIES
    None.

  PARAMETERS
    None.
 
  RETURN VALUE
    TRUE if EFS logging is enabled, otherwise FALSE

  SIDE EFFECTS
    None.

===========================================================================*/
boolean mcfg_nv_efslogging_enabled();

/*===========================================================================

FUNCTION mcfg_nv_update_symlink

DESCRIPTION
  Creates/updates the symlink for the Muxd NV Items.
  
DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] MuxdNV item to update
    index     [in] index of the MuxdNV item 
    subs_id   [in] Subscription ID for the Multi-SIM NV item to write,
              or 0 if not a multi-SIM NV (if unsure, use 0)
    size      [in] size of the item 
                  

RETURN VALUE
  mcfg_nv_status_e_type

SIDE EFFECTS

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_update_symlink(
   uint32 item, 
   uint8 index, 
   uint8 subs_id,  
   uint32 size);

/*===========================================================================

FUNCTION mcfg_nv_rm_symlink

DESCRIPTION
  Removes symlink for the Muxd NV Items or EFS Files
  
DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] MuxdNV item to update
    index     [in] index of the MuxdNV item 
    subs_id   [in] Subscription ID for the Multi-SIM NV item to write,
              or 0 if not a multi-SIM NV (if unsure, use 0)

RETURN VALUE
  mcfg_nv_status_e_type

SIDE EFFECTS

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_rm_symlink(
   uint32 item, 
   uint8 index,
   uint8 subs_id);

/*===========================================================================

FUNCTION mcfg_nv_init

DESCRIPTION
  Generate mcfg.conf for QXDM and QPST to access and restore
  
DEPENDENCIES
  None.

RETURN VALUE
  booelan

SIDE EFFECTS

===========================================================================*/
boolean mcfg_nv_init(void);

/*==========================================================================

  FUNCTION mcfg_nv_build_path

  DESCRIPTION
    This function turns an NV item and an index into a path to a file in 
    EFS.  Performs validation of the item and index.

  DEPENDENCIES
    path_len must be >= MCFG_NV_MAX_EFS_PATH_LEN

  PARAMETERS
    item         [in] NV item: can be nv_items_enum_type or
                      mcfg_nv_item_e_type
    index        [in] index of the NV item, or 0 if no index
    new_indexed [out] will be set to TRUE if the item is new-style and
                      indexed. can be NULL
    path        [out] EFS path to NV item file
    path_len     [in] size of buffer at data_ptr
    
    
  RETURN VALUE
    MCFG_NV_STATUS_OK on success
    MCFG_NV_STATUS_BAD_ITEM if item is not in old NV or new MCFG NV range,
        or index is outside defined limit (new-style items only)

  SIDE EFFECTS
    None.

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_build_path(
  uint32    item,
  uint8     index,
  uint8     subs_id,
  boolean * new_indexed,
  char *    path,
  uint32    path_len
  );


#endif /* MCFG_NV_H */


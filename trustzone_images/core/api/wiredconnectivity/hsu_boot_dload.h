#ifndef _HSU_BOOT_DLOAD_H_
#define _HSU_BOOT_DLOAD_H_

/*=============================================================================

     High Speed USB Boot Dload
        H E A D E R   F I L E
=============================================================================*/
/**
@file hsu_boot_dload.h
 
Includes functions that are related to the Software Download feature.

Initialization and sequencing requirements:\n
Functions should only be called from the TMC's context.
*/
/*=============================================================================
  Copyright (c) 2007-2010 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_boot_dload.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  06/24/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
  04/15/07  me  Initial version
    
=============================================================================*/
#include "hsu_conf_sel_types.h"

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
  Forward Declarations
=============================================================================*/

/*=============================================================================
  Constants
=============================================================================*/

/*=============================================================================
  Macros
=============================================================================*/

/*=============================================================================
  Typedefs
==============================================================================*/
/** Invalid version number.
*/
#define HSU_BOOT_DLOAD_INVALID_VERSION_NUM 0xFFFFFFFF

/*=============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_boot_dload_transition
=============================================================================*/
/**
Performs all operations needed before entering Download mode (e.g., writing 
data to the IRAM).

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_boot_dload_transition(void);

/*=============================================================================
FUNCTION    hsu_boot_dload_armprg_transition
=============================================================================*/
/**
Function to be called before transitioning to ARMPRG.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_boot_dload_armprg_transition(void);

/*=============================================================================
FUNCTION    hsu_boot_dload_copy_shared_segment_to_imem
=============================================================================*/
/**
Copies the shared segment data to the IMEM to be used by ARMPRG in targets that
do not have IRAM.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_boot_dload_copy_shared_segment_to_imem(void);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_BOOT_DLOAD_H_ */

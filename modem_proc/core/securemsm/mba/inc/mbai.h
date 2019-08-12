/**
* @file mba_main.c
* @brief Modem Self Authentication implementation
*
* This file implements the core of Modem Self Authentication
*
*/
/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/inc/mbai.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $


when       who      what, where, why
--------   ---      ------------------------------------
02/28/2012 mm       Integrated with secure pil library
12/02/2011 mm       Initial version
===========================================================================*/
#include <comdef.h>
#include "mba_ac.h"

/* Extern functions */
int mba_unlock_xpu_all(void);
void mba_scribble_xpu_memory(void);
int mba_initialize (void);
int mba_bam_initialize (void);
int mba_xpu_lock_ddr_heap(void);
int mba_deinitialize (void);
uint32 mba_configure_dehr(void);
int mba_unlock_xpu_all(void);
int mba_heap_entry_delete(void);
 /* Get thes software version from the Rollback fuse */
 uint32 mba_secctrl_get_software_version(void);
 
 int mba_xpu_unlock_region(resource_id_type resource_id);
int mba_heap_initialize(void);

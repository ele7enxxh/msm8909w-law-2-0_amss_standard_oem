/*===========================================================================
                        mcfg_proc_i.h

DESCRIPTION

   Internal header file for modem_cfg_task
 
Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
 
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_proc/src/mcfg_proc_i.h#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
1/18/12      sbt   Create

===========================================================================*/

#ifndef MCFG_PROC_I_H
#define MCFG_PROC_I_H

#include "mcfg_nv.h"

#include "mcfg_refresh_i.h"

// CONFIGURATION TYPES
/* Note - For initial phase 1 do not process MCFG_TYPE_HW */
#define     MCFG_TYPE_MIN                       MCFG_TYPE_HW  
#define     MCFG_TYPE_MAX                       MCFG_TYPE_SW

#define MCFG_UPDATE_DEVCONFIG                0x1F00


/* Function prototypes*/
boolean mcfg_update_nvefs(uint8 cfg_type, uint32 num_mcfg_items, uint8* ram_cust_data_ptr, uint8 subs_id,
                          mcfg_refresh_type_e_type refresh_type);

#endif /* MCFG_PROC_I_H */

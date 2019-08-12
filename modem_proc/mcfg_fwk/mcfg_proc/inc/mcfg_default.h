/*===========================================================================
                        mcfg_default.h

DESCRIPTION

   Internal header file for modem_default
 
Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
 
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_proc/inc/mcfg_default.h#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
02/01/13   cy     Initial Creation.

===========================================================================*/

#ifndef MCFG_DEFAULT_H
#define MCFG_DEFAULT_H

#include "mcfg_common.h"


/* Function prototypes*/
boolean mcfg_default_config_enabled
(
   uint8  type
);

boolean mcfg_default_config_force_load
(
   uint8  type,
   boolean is_default,
   uint8  subs_id
);

uint32 mcfg_default_config_process
( 
  mcfg_config_type_e_type type,
  mcfg_sub_id_type_e_type subs_id
);
#endif /* MCFG_DEFAULT_H */

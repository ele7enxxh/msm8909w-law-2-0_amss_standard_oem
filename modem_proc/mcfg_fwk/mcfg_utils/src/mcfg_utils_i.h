#ifndef MCFG_UTILS_I_H
#define MCFG_UTILS_I_H
/*==============================================================================

            M O D E M   C O N F I G   U T I L S   I N T E R N A L
 
GENERAL DESCRIPTION
  Internal header MCFG Utility library

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_utils_i.h#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
09/07/12 bd   Move condition check macros to mcfg_int.h
05/08/12 cy   Initial creation

==============================================================================*/

#include "mcfg_int.h"
#include "mcfg_common.h"
#include "mcfg_utils.h"

/*==============================================================================

                PRIVATE DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
/* The following defines are file offsets based on the trailer spec.
   Note: (GCI = Get Config Info) */
#define MCFG_GCI_PREFIX_TYPE_OFFSET     4
#define MCFG_GCI_TRAILER_TYPE_OFFSET    8
#define MCFG_GCI_TRAILER_LENGTH_OFFSET  10
#define MCFG_GCI_TRAILER_DESC_OFFSET    12
#define MCFG_GCI_MAGIC_PREFIX_TYPE      0xA
#define MCFG_GCI_MAGIC_TRAILER_TYPE     0xA1

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/


/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/
#endif /* MCFG_UTILS_I_H */


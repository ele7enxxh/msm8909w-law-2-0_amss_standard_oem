/*===========================================================================

                             D S M S G R . C
 
GENERAL DESCRIPTION
  Implementation of DS MSGR interface with DS task.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/msgr/src/dsmsgr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/23/08   vk      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

 
#ifdef TEST_FRAMEWORK
#error code not present
#endif

#include "amssassert.h"
#include "dsmsgr.h"
#include "dstask_v.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

void dsmsgr_post_ds_cmd (void)
{
  ds_cmd_type * ds_cmd_p;

  ds_cmd_p = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  ASSERT(ds_cmd_p != NULL);

  ds_cmd_p->hdr.cmd_id = DS_CMD_MSGR_RECV;
  ds_put_cmd(ds_cmd_p);
}


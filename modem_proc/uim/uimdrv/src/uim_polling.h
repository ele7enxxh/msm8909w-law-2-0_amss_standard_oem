#ifndef UIM_POLLING_H
#define UIM_POLLING_H
/*==============================================================================
  FILE:         uim_polling.h

  OVERVIEW:     FIle conatins the inclusions and definitions needed for
                polling functions.

  DEPENDENCIES: N/A

                Copyright (c) 2014-2015 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_polling.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       ---------------------------------------------------------
11/05/15   ks          Rel12 polling enhancement
==============================================================================*/

#include "uimglobals.h"
#include "gstk_exp.h"
#include "estk.h"
#include "ds_sys_event.h"
#include "ds_sys_ioctl.h"


/* Type for log mask used for apdu logging*/
typedef enum
{
  UIM_POLLING_NONE,
  UIM_ICC_CARD_POLLING, /* For ICC cards, STATUS command expects 22 bytes of response data */
  UIM_UICC_PROACTIVE_POLLING,/* For UICC cards, proactive polling expects no return data */
  UIM_UICC_PRESENCE_DETECTION_POLLING /* For UICC cards, detection polling expects rsp data based on directory */
}uim_polling_type;


/*===========================================================================

FUNCTION UIM_PROCESS_POLLING

DESCRIPTION
  This procedure decides on the return type of the POLL command
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void uim_process_polling
(
  rex_sigs_type            *mask,
  uim_instance_global_type *uim_ptr
);
#endif /* UIM_POLLING_H */

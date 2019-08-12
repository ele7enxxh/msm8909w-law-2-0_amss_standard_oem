#ifndef __CVAINIT_H__
#define __CVAINIT_H__

/*===========================================================================

                    A U D I O   I N I T   S E R V I C E S

DESCRIPTION
  This header file defines APIs for initializing all cva-related modules.

REFERENCES
  None.

Copyright (C) 2008 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvainit/cvainit.h#1 $
$DateTime: 2016/12/13 07:59:59 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/15   aim     Initial revision.

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "mmdefs.h"

/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/**
 * This is the cva task entry-point.
 *
 * \param[in] params The parameters supplied by RCinit.
 *
 * \remarks
 *   This API is intended to be called only once by the RCinit
 *   during boot-up initialization.
 */
void cva_task (
  dword params
);

#endif  /* !__CVAINIT_H__ */


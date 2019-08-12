#ifndef MDSPSYNCCMD_H
#define MDSPSYNCCMD_H

/*===========================================================================
                   M D S P S Y N C C M D . H


DESCRIPTION
This file defines all the L1 local command structs that are used for 
local commands relating to the mDSP sync module.


    Copyright (c) 1999, 2000
                    by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/VCS/mdspsynccmd.h_v   1.0   11 Jan 2001 08:44:40   mlevin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mdspsynccmd.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
01/10/2001  mdl     first cut, added mdsp_sync_timeout_cmd_type
===========================================================================*/
#include "l1def.h"


/*--------------------------------------------------------------------------
                TYPEDEF: MDSP_SYNC_TIMEOUT_CMD_TYPE
--------------------------------------------------------------------------*/
typedef struct {
  l1_local_cmd_hdr_type hdr;

} mdsp_sync_timeout_cmd_type;

#endif

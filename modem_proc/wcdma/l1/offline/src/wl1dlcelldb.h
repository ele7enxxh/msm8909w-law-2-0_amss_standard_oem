#ifndef WL1DLCELLDB_H
#define WL1DLCELLDB_H
/*===========================================================================
                 WCDMA L1 DL CELL DATA BASE

GENERAL DESCRIPTION
  This file contains the code for allocate/deaalocate/provide cell db Index and maintaines necessary cell info.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1dlcelldb.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------- 
12/06/13    zr     Removing DL_MAX_NUM_CELLS 
12/10/08    ks     Mainlining FEATURE_WCDMA_DL_ENHANCED
12/04/08    vc     Modified the max number of cell for MIMO to 8.
01/25/08    vsr    Checking in Enhanced DL files onto mainline
08/30/07    ub     Added P4 file info
05/04/07    sv     Added function API to get cell frequency based on cell_db_idx
12/12/06    mc     Synced up for MBMS Phase2A integration
10/17/06    vb     Created this module

===========================================================================*/


/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "comdef.h"
#include "customer.h"

#include "l1rrcif.h"
#include "l1const.h"

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define DL_INVALID_CELL_DB_ID 0xFF

#endif  /* WL1DLCELLDB_H */

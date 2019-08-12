#ifndef L1_SC_DRV_H
#define L1_SC_DRV_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              l1_sc_drv.h

GENERAL DESCRIPTION
   This module contains I/F to the hardware used by the SCE. Functions
   starting with l1_scdrv_ are implemented in the HW driver. Functions
   starting with l1_sc_ are callbacks called by the HW driver and are
   implemented in the SCE.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L1/vcs/l1_sc_drv.h_v   1.3   22 Mar 2002 19:29:06   chatlani  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_sc_drv.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   --------  ----------------------------------------------------------
06/12/03   gw        Deleted a bunch of unused #defines.  Moved others relating 
                     to ncell receives to l1_sc.h.
03/20/02   JC        Cleanup of legacy RF constants.
03/05/02   JC        Cleanup of legacy header file.
02/07/20   gw        Removed obsolete declarations.
12/21/01   gw        Changed L1_SCDRV_DURATION_SB from 2 to 4.
12/18/01   dp        Cleaned up unused functions
10/24/01   dp        Cleaned up unused TCH related functions
10/16/01   gw        Cleaned up definitions and prototypes for SB reception.
                     Changed CCWW types to QCT types.
10/10/01   gw        Added support for message layer Ncell reception.
09/25/01   mk        Mainlined MONITOR_DEBUG.
09/11/01   JC        Added QCT header templates.
08/17/01   mk/gw     Added Ncell support, featurized via MONITOR_DEBUG.
08/16/01   mk        Initial version.  Minor Cleanup and Header Addition.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "geran_multi_sim.h"


/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*
 * Current frame number
 */

/* TODO maybe we should make this a global variable */

uint32 l1_scdrv_get_fn( gas_id_t gas_id  );

/*
 * Performing lengthy tasks
 */

void l1_scdrv_call_in_task( void ( *code )( void*, gas_id_t gas_id ), void* data, gas_id_t gas_id );
void l1_wcdma_schedule_idle( void* unused, gas_id_t gas_id );
#endif

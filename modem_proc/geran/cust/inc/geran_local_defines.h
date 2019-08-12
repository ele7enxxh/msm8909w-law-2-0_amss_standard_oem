#ifndef GERAN_LOCAL_DEFINES_H
#define GERAN_LOCAL_DEFINES_H
/*===========================================================================

          G E R A N   L O C A L   D E F I N E S   H E A D E R   F I L E

GENERAL DESCRIPTION
  This file contains FEATUREs previously defined in a target-owned cust file or
  another SUs cust file but only used by GERAN

Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
===========================================================================*/

/* <EJECT>^L */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/cust/inc/geran_local_defines.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12-07-02   da      Initial revision.

===========================================================================*/

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <geran> */
#define DEBUG_DISABLE_L1STATE_EVENT
#define DEBUG_SYSTEMS_RESELECTION_GTOW
#define FEATURE_MDSP_SVC_DOES_CLK_SWITCHING
#define FEATURE_SLEEP_CTRL_VIA_QXDM_DIPSW
#define FEATURE_SRCH_BITWIDTH_MULTI_FACTOR


/* includes for local cust files
 * Note - these files are only visible to geran.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */
#include "custgsm.h"
#include "custgsmdrv.h"

/* Include the macros that map GERAN debug messages to MSG2.0 format and prepend gas_id in Dual SIM builds.
   This is a convenient place for this as geran_local_defines.h is already included in all GERAN files */
#include "geran_msgs.h"

#endif /* GERAN_LOCAL_DEFINES_H */

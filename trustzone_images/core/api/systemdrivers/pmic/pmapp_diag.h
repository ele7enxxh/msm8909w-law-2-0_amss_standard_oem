#ifndef PM_APP_DIAG_H
#define PM_APP_DIAG_H
/*! \file 
 *  \n
 *  \brief  pmapp_diag.h ---- PMIC-DIAG
 *  \details  Packet definitions between the diagnostic subsystem
 *  and the external device.
 *
 *  !!! NOTE: All member structures of DIAG packets must be PACKED.
 * 
 *  !!! WARNING: Each command code number is part of the externalized
 *  diagnostic command interface.  This number *MUST* be assigned
 *  by a member of QCT's tools development team.
 *
 *
 *  \n &copy; Copyright (c) 2004-2011  by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                             Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pmapp_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/11   hs      Changed PACKED struct to PACK(struct) to work with Q6 compiler.
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
04/24/09   wra     Added the pm_app_set_mode diag message definition.
08/25/05   Frank   Added diag lib.
04/05/04   rmd     Featurized PMIC DIAG services under 
                   "FEATURE_PMIC_DIAG_SERVICES".
03/24/04   rmd     Updated file to follow PMIC VU coding standards.
02/29/04   st      Created file.
===========================================================================*/


#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"

#if defined(T_WINNT)
   #pragma pack(push, PM_APP_DIAG_H, 1)
#endif


/*==========================================================================
  
                  SUBSYSTEM PACKET CODE DEFINITIONS
   
============================================================================*/

#define PM_APP_DIAG_VREG_SET_F      0
#define PM_APP_DIAG_LIB_DIAG        1
#define PM_APP_SET_MODE_DIAG        2
#define PM_APP_DIAG_LIB_DIAG_EXT    3
#define PM_APP_DIAG_LIB_DIAG_DEV    4


/*==========================================================================

PACKET   PM_APP_DIAG_MSMC_VREG_SET

PURPOSE  Sent from the DM to the DMSS to set a VREG voltage.

============================================================================*/

typedef PACK(struct) 
{
  diagpkt_subsys_header_type  header;
  uint8                       vreg_id;   // pm_vreg_id_type
  uint16                      millivolts;
} pm_app_diag_vreg_set_req_type;

/*===========================================================================

FUNCTION PM_APP_DIAG_INIT

DESCRIPTION
  Register the diag packet function dispatch table.

===========================================================================*/

void pm_app_diag_init (void);


#endif /* PM_APP_DIAG_H */


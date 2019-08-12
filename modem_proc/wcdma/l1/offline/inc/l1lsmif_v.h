#ifndef L1LSMIF_V_H
#define L1LSMIF_V_H
/*==========================================================================

             L1 LSM Interface Header File

DESCRIPTION
  This file contains global declarations and external references
  for the WCDMA L1 -to- LSM (Location Services Manager) Interface

  Copyright (c) 2003 -2007 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1lsmif_v.h#1 $
  $DateTime: 2016/12/13 08:00:12 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/13   geg     WSW decoupling (except for component of QCHAT)
04/24/11   geg     Initial file
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "l1rrcif.h"
#include "l1lsmif.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

#if defined(FEATURE_MMGPS)
/*===========================================================================
FUNCTION     L1POS_SET_GPS_ACTIVE_STATUS

DESCRIPTION
  GPS (meaning Location Services Manager) calls this function to inform WCDMA
  L1 of its status.  GPS active means it requires VCTCXO to be frozen.  WL1,
  in turn, freezes VCTCXO until GPS no longer needs it or until its error
  becomes too great.  If it becomes to great, it notifies GPS that VCTCXO
  will no longer be frozen.

DEPENDENCIES
  None.

PARAMETERS
  boolean gps_active - TRUE if GPS active.
                       FALSE if GPS not active.
RETURN VALUE
  boolean - TRUE:  VCTCXO is frozen.  GPS processing may proceed.
          \ FALSE: We could NOT freeze VCTCXO.  GPS processing
                   should NOT proceed.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean l1pos_set_gps_active_status(boolean gps_active);
#endif

#endif  /* L1LSMIF_V_H */

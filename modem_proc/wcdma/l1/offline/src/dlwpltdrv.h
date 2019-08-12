#ifndef DLWPLTDRV_H
#define DLWPLTDRV_H
/*===========================================================================
                          D L W P L T D R V . H

DESCRIPTION
This file contains typedefs, constants and prototypes for functions that
make up the low level demod routines for WPLT.

    Copyright (c) 1999, 2000, 2001
                    by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dlwpltdrv.h_v   1.1   19 Mar 2001 13:50:34   mlevin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlwpltdrv.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
05/31/2005  sk      Lint Fixes
03/19/2001  mdl     added prototypes

===========================================================================*/


typedef enum {
  DL_WPLT_STATUS_PASS,
  DL_WPLT_STATUS_INVALID_CMD,
  DL_WPLT_STATUS_BAD_PARMS,
  DL_WPLT_STATUS_BAD_STATE,
  DL_WPLT_STATUS_FW_ERROR,
  DL_WPLT_STATUS_GET_AWAY_FROM_ME_BOY_YA_BOTHER_ME
} dl_wplt_status_enum_type;

#define DL_WPLT_IS_SUCCESS(status) (status == DL_WPLT_STATUS_PASS)


/* masks for determining which parameters to update in
   dl_set_cmn_fing_parm */
#define WPLT_CMN_FING_PARM_MASK_TTK1            0x01
#define WPLT_CMN_FING_PARM_MASK_TTK2            0x02
#define WPLT_CMN_FING_PARM_MASK_LOCK_HI         0x04
#define WPLT_CMN_FING_PARM_MASK_LOCK_LO         0x08
#define WPLT_CMN_FING_PARM_MASK_PCB_COMB_THESH  0x10
#define WPLT_CMN_FING_PARM_MASK_RSSI_GAIN       0x20
#define WPLT_CMN_FING_PARM_MASK_PILOT_GAIN      0x40
#define WPLT_CMN_FING_PARM_MASK_WNTIO_GAIN      0x80


/******************************************************************************
*******************************************************************************
                          FUNCTION PROTOTYPES
*******************************************************************************
******************************************************************************/

/*===========================================================================
FUNCTION        DL_WPLT_SET_CMN_FING_PARM

FILE            dlwpltdrv.c

DESCRIPTION     This function sets common finger parameters as selected by
                the parm_mask.  No checking is done on the validity of the
                parameters.

DEPENDENCIES    None

RETURN VALUE    DL_WPLT_STATUS_PASS

SIDE EFFECTS    The variable demod_fing_common_parms will be updated with
                the new parameters.  All the parameters in
                demod_fing_common_parms will be written to the firmware
                even if their value has not changed.
===========================================================================*/
dl_wplt_status_enum_type dl_wplt_set_cmn_fing_parm
(
  /* bitmask indicating which parameters to update */
  uint8 parm_mask,

  /* Time tracking filter gain constant 1 (alpha) */
  uint16 ttk1,

  /* Time tracking filter gain constant 2 (beta) */
  uint16 ttk2,

  /* Data symbol combiner upper lock threshold */
  uint16 lock_hi,

  /* Data symbol combiner lower lock threshold */
  uint16 lock_lo,

  /* Power control bit combiner lock threshold */
  uint16 pcb_comb_thresh,
#include "wcdma_variation.h"
  /* Pilot symbol filter gain constant */
  uint16 pilot_gain,

  /* Noise estimation filter gain constant */
  uint16 wntio_gain
);


/*===========================================================================
FUNCTION        DL_WPLT_LOG_FING_STATUS

FILE            dlwpltdrv.c

DESCRIPTION     This function performs a status dump and logs the finger
                status

DEPENDENCIES    None

RETURN VALUE    If the status dump is successful (and it is a bad error if
                it is not) DL_WPLT_STATUS_PASS, else DL_WPLT_STATUS_FW_ERROR

SIDE EFFECTS
===========================================================================*/
dl_wplt_status_enum_type dl_wplt_log_fing_status(void);

#endif

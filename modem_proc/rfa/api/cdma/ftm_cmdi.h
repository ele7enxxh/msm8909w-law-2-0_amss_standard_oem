#ifndef FTM_CMDI_H
#define FTM_CMDI_H

/*!
   @file
   ftm_cmdi.h

   @brief
   Temporary API file to satisfy Modem unit dependencies.

   @details
   In non-SV targets, points to the old header file.
   In SV targets, provides temporary definitions until they can be moved
   to official RFM 1x headers.
*/

/*===========================================================================
Copyright (c) 2010 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/ftm_cmdi.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/16/11   Saul    HDR non-sig support.
04/28/11   sar     CMI-4 Cleanup.
12/07/10   aro     Update for 1x
12/06/10   bmg     Initial version of new/old vector include fie.

============================================================================*/

#include "comdef.h"

/* Temporary definitions go here. */

#include "ftm_cmd.h" /* Needed for ftm_acq_mode_type */

  /* The acquisition mode */

  /* Flag indicating if the user wants to wait for a PC message */

typedef enum
{
  HDR_FTM_ENTER,                  /* FTM entry state */
  HDR_FTM_PIL_ACQ,                /* FTM pilot channel acquisition */
  HDR_FTM_SYNC_STATE,            /* FTM sync channel processing, combines
                                 CDMA_SYNC_ACQ, CDMA_SYNC_MSG */
  HDR_FTM_IDLE,                   /* FTM idle state, we have acquired system
                                 time and are demodulating the paging 
                                 channel. */
  HDR_FTM_FWD_TRAFFIC_ONLY,  
  HDR_FTM_TRAFFIC,                /* FTM traffic state */
  HDR_FTM_SYS_ACC,                /* FTM System Access state */
  HDR_FTM_REV_TRAFFIC,            /* FTM reverse traffic state */
  HDR_FTM_MAX_STATE
} hdr_ftm_state_enum_type;

extern hdr_ftm_state_enum_type hdr_ftm_fwd_curr_state;
  /* Current HDR FTM state on the forward link */

  /* Current HDR FTM state on the reverse link */

typedef enum
{
  FTM_ENTER,                  /* FTM entry state */
  FTM_INIT_CDMA,              /* FTM CDMA initialization */
  FTM_PIL_ACQ,                /* FTM pilot channel acquisition */
  FTM_SYNC,                   /* FTM sync channel processing, combines
                                 CDMA_SYNC_ACQ, CDMA_SYNC_MSG */
  FTM_IDLE,                   /* FTM idle state, we have acquired system
                                 time and are demodulating the paging 
                                 channel. */
  FTM_TRAFFIC,                /* FTM traffic state */
  FTM_SYS_ACC,                /* FTM System Access state */
  FTM_REV_TRAFFIC,            /* FTM reverse traffic state */
  FTM_EXIT          = 0x0500  /* FTM exit state - Somebody determined that this number meant exit */
} ftm_state_enum_type;

/*--------------------------------------------------------------------------
                          Type and Variables 
--------------------------------------------------------------------------*/

extern q_type            ftm_mc_cmd_free_q;

  /* Current FTM state on the forward link */

  /* Current FTM state on the reverse link */

void hdr_ftmi_mod_rtc
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
);

#endif /* FTM_CMDI_H */


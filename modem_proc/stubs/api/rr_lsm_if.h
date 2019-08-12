#ifndef RRLSMIF_H
#define RRLSMIF_H


/*============================================================================

FILE:      rr_lsm_if.h

DESCRIPTION:
  This file stubs one wcdma api structs and several RRC defines for PE playback.

PUBLIC CLASSES:
  <none>

============================================================================
        Copyright © 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/


typedef struct
{
  boolean camped_on_flag;
} current_bs_info_T;

#define  RRC_MAX_GSMMAP_IMSI_DIGITS        15
#define  RRC_MAX_GSMMAP_TMSI_LENGTH        4   /* Length of TMSI in bytes */
#define  RRC_MAX_GSMMAP_PTMSI_LENGTH       4   /* Length of PTMSI in bytes */
#define  RRC_MAX_GSMMAP_LAC_LENGTH         2   /* Length of LAC in bytes */
#define  RRC_MAX_GSMMAP_RAC_LENGTH         1   /* Length of RAC in bytes */
#define  RRC_MAX_MCC_DIGITS                3
#define  RRC_MAX_MNC_DIGITS                3


#endif /* RRLSMIF_H */

#ifndef __VSS_DELETED_PUBLIC_IF_H__
#define __VSS_DELETED_PUBLIC_IF_H__

/**
  @file vss_deleted_public_if.h
*/
/*===========================================================================
NOTE: This file contains the APR API definitions that are deleted and
      unsupported by the Core Voice Driver (CVD). The only purpose of this file
      is to allow CVD's code compilation.
=============================================================================*/
/*===========================================================================
Copyright (c) 2014 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_deleted_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/

/****************************************************************************
 * DELETED CALIBRATION FRAMEWORK DEFINITIONS                                *
 ****************************************************************************/

/** @addtogroup cvd_common_deleted
@{ */

/*--------------------------------------------------------------------------*
 * DELETED CALIBRATION COLUMN IDS                                           *
 *--------------------------------------------------------------------------*/

/** Column vocoder class. */
#define VSS_ICOMMON_CAL_COLUMN_VOC_CLASS ( 0x00011359 )

/*--------------------------------------------------------------------------*
 * DELETED CALIBRATION VOCODER CLASS COLUMN VALUES                          *
 *--------------------------------------------------------------------------*/

/** Vocoder class is NONE. */
#define VSS_ICOMMON_CAL_VOC_CLASS_NONE ( 0x00011364 )

/** CDMA vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_CDMA ( 0x00011365 )

/** GSM vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_GSM ( 0x00011366 )

/** WCDMA vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_WCDMA ( 0x00011367 )

/** @} */ /* end_addtogroup cvd_common_deleted */

#endif /* __VSS_DELETED_PUBLIC_IF_H__ */

